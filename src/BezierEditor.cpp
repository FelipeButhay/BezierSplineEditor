#include "BezierEditor.h"

BezierEditor::BezierEditor() {
	this->resolution = 4;
	this->sx = difResolutions[resolution][0];
	this->sy = difResolutions[resolution][1];
	this->u = sy/20;

	InitWindow(sx, sy, "Track Editor");
	SetTargetFPS(60);

	this->Camera.target = { 0, 0 };
	this->Camera.offset = { sx*0.5f, sy*0.5f };
	this->Camera.rotation = 0.0f;
	this->Camera.zoom = 1.0f;

	this->MovingPointSelected = -1;
	this->ExpandPointSelected = -1;

	this->Grid.Spacing = GetGridSpacing(sx, Camera.zoom);
	this->Grid.Active = false;

	this->Ruler.Active = false;
	this->PointRadius = 10;

	if (!LoadTrack()) {
		BezierCurve Curve = {
			{
				{0, 0},
				{500, 0},
				{-200, 300},
				{400, -400}
			}
		};

		this->BezierSpline.push_back(Curve);

		this->TrackWidth = 12;
	}
}

bool BezierEditor::LoadTrack() {
	std::ifstream File("load/load_track.bin", std::ios::binary);

	if (!File) {
		std::cerr << "Error al abrir el archivo.\n";
		return false;
	}

	// Leer el ancho de la pista (short)
	File.read(reinterpret_cast<char*>(&this->TrackWidth), sizeof(short));

	if (!File) {
		std::cerr << "Error al leer el ancho de la pista.\n";
		return false;
	}

	std::cout << "Ancho de la pista: " << TrackWidth << std::endl;

	File.read(reinterpret_cast<char*>(&this->ClosedTrack), sizeof(short));

	if (!File) {
		std::cerr << "Error al leer si la pista esta cerrada.\n";
		return false;
	}

	std::cout << "PistaCerrada? " << ClosedTrack << std::endl;

	// Determinar cuántas curvas hay en el archivo
	File.seekg(0, std::ios::end);
	std::streamsize FileSize = File.tellg();
	File.seekg(sizeof(short) + sizeof(bool), std::ios::beg); // Volver a la posición después del short

	std::streamsize SplineSize = FileSize - sizeof(short) - sizeof(bool);
	size_t CurveN = SplineSize / sizeof(BezierCurve);

	if (SplineSize % sizeof(BezierCurve) != 0) {
		std::cerr << "Error: el archivo parece estar corrupto.\n";
		return false;
	}

	std::cout << "Número de curvas: " << CurveN << std::endl;

	// Leer las curvas
	this->BezierSpline.resize(CurveN);
	File.read(reinterpret_cast<char*>(BezierSpline.data()), SplineSize);

	if (!File) {
		std::cerr << "Error al leer las curvas.\n";
		return false;
	}

	File.close();

	return true;
}

void BezierEditor::SaveTrack() {
	std::time_t t = std::time(nullptr);
	std::tm now;
	localtime_s(&now, &t);

	std::string DateAndTime = 
		IntToStr(now.tm_mday) + "-" +
		IntToStr(now.tm_mon + 1)  + "-" +
		IntToStr(now.tm_year + 1900) + "-" +

		IntToStr(now.tm_hour) + "_" +
		IntToStr(now.tm_min)  + "-" +
		IntToStr(now.tm_sec);

	std::ofstream archivo("saves/track_" + DateAndTime + ".bin", std::ios::binary);

	if (archivo.is_open()) {
		archivo.write(reinterpret_cast<char*>(&TrackWidth) , sizeof(TrackWidth) );
		archivo.write(reinterpret_cast<char*>(&ClosedTrack), sizeof(ClosedTrack));

		for (int i = 0; i < BezierSpline.size(); i++) {
			archivo.write(reinterpret_cast<char*>(&BezierSpline[i]), sizeof(BezierSpline[i]));
		}
		archivo.close();

		std::cout << "Archivo binario escrito correctamente.\n";
	} else {
		std::cerr << "Error al abrir el archivo.\n";
	}
}

void BezierEditor::Loop() {
	// >> CAMERA MOVEMENT <<
	float WheelMove = GetMouseWheelMove();
	this->Camera.zoom *= 1 + WheelMove/10.0f;

	if (WheelMove != 0) {
		this->MovingPointSelected = -1;
		this->ExpandPointSelected = -1;

		this->Grid.Spacing = GetGridSpacing(sx, Camera.zoom);
	}

	if (IsKeyPressed(KEY_UP)   && TrackWidth < 30) this->TrackWidth++;
	if (IsKeyPressed(KEY_DOWN) && TrackWidth >  1) this->TrackWidth--;

	if (IsKeyDown(KEY_W)) this->Camera.target.y -= GetFrameTime() * 1000 / Camera.zoom;
	if (IsKeyDown(KEY_A)) this->Camera.target.x -= GetFrameTime() * 1000 / Camera.zoom;
	if (IsKeyDown(KEY_S)) this->Camera.target.y += GetFrameTime() * 1000 / Camera.zoom;
	if (IsKeyDown(KEY_D)) this->Camera.target.x += GetFrameTime() * 1000 / Camera.zoom;

	if (IsKeyPressed(KEY_LEFT_ALT) || IsKeyPressed(KEY_RIGHT_ALT)) {
		// ABRIENDO
		if (ClosedTrack) {
			this->BezierSpline.back() = LastCurveSave;

			this->ClosedTrack = false;

		// CERRANDO
		} else if (BezierSpline.size() > 1) {
			this->LastCurveSave = BezierSpline.back();
			this->BezierSpline.back().P[1] = BezierSpline.front().P[0];
			this->BezierSpline.back().P[3] = GetMirrorPoint(BezierSpline.front().P[2], BezierSpline.front().P[0]);

			this->ClosedTrack = true;
		}
	}

	if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL)) {
		this->Grid.Active = !Grid.Active;
	}

	// >> MOVER PUNTOS <<

	// ACTUALIZA LA POSICION DEL PUNTO SELECCIONADO
	if (IsMouseButtonDown(0) && MovingPointSelected != -1) {
		Vector2 MousePos = GetScreenToWorld2D(GetMousePosition(), Camera);

		if (Grid.Active && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
			MousePos = GetClosestGridPoint(&Grid, MousePos);
		}

		int CurveNumber = MovingPointSelected / 4;
		int LocalPoint = MovingPointSelected % 4;

		this->BezierSpline[CurveNumber].P[LocalPoint] = MousePos;

		if (MovingPointSelected % 4 < 2) {
			// MUEVE LOS CURVE CON SU TRIAL
			
			this->BezierSpline[CurveNumber].P[LocalPoint + 2] = {
				MousePos.x + PointOffset.x,
				MousePos.y + PointOffset.y
			};

			// MUEVE LOS TRIALS CONTIGUOS
			if (MovingPointSelected != 0 && 
				MovingPointSelected != 1 + 4 * (BezierSpline.size() - 1)) {

				int AdyacentCurveNumber = CurveNumber + 2 * LocalPoint - 1;
				int AdyacentPointNumber = LocalPoint + std::abs(LocalPoint - 1);

				this->BezierSpline[AdyacentCurveNumber].P[AdyacentPointNumber] = MousePos;

				this->BezierSpline[AdyacentCurveNumber].P[AdyacentPointNumber + 2] = {
					MousePos.x - PointOffset.x,
					MousePos.y - PointOffset.y
				};

			} else if (ClosedTrack) {
				if (MovingPointSelected == 0) {
					this->BezierSpline.back().P[1] = MousePos;
					this->BezierSpline.back().P[3] = {
						MousePos.x - PointOffset.x,
						MousePos.y - PointOffset.y
					};
				}

				if (MovingPointSelected == 1 + 4 * (BezierSpline.size() - 1)) {
					this->BezierSpline.front().P[0] = MousePos;
					this->BezierSpline.front().P[2] = {
						MousePos.x - PointOffset.x,
						MousePos.y - PointOffset.y
					};
				}
			}

		} else {
			// MANTIENE EL MIRRORING
			if (MovingPointSelected != 2 &&
				MovingPointSelected != 3 + 4 * (BezierSpline.size() - 1)) {

				int MirrorCurve = CurveNumber + (LocalPoint == 2 ? -1 : 1);
				int MirrorPoint = LocalPoint == 2 ? 3 : 2;

				this->BezierSpline[MirrorCurve].P[MirrorPoint] = GetMirrorPoint(
					BezierSpline[CurveNumber].P[LocalPoint],
					BezierSpline[CurveNumber].P[LocalPoint - 2]
				);
			} else if (ClosedTrack) {
				if (MovingPointSelected == 2) {
					this->BezierSpline.back().P[3] = GetMirrorPoint(
						BezierSpline.front().P[2],
						BezierSpline.front().P[0]
					);
				}
				if (MovingPointSelected == 3 + 4 * (BezierSpline.size() - 1)) {
					this->BezierSpline.front().P[2] = GetMirrorPoint(
						BezierSpline.back().P[3],
						BezierSpline.back().P[1]
					);
				}
			}
		}
	}

	// SELECCIONA UN PUNTO PARA MOVERLO O CREA EL P1 DE LA REGLA
	if (IsMouseButtonPressed(0)) {
		for (int i = 0; i < BezierSpline.size(); i++) {
			for (int j = 0; j < 4; j++) {
				if (MouseInCircV(&Camera, BezierSpline[i].P[j], PointRadius)) {
					this->MovingPointSelected = i*4 + j;
					
					Vector2 MousePos = GetScreenToWorld2D(GetMousePosition(), Camera);

					if (Grid.Active && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
						MousePos = GetClosestGridPoint(&Grid, MousePos);
					}

					if (j < 2) {
						this->PointOffset = {
							BezierSpline[i].P[j + 2].x - MousePos.x,
							BezierSpline[i].P[j + 2].y - MousePos.y
						};
					}
				}
			}
		}

		// SI NO TOCASTE NINGUN PUNTO DE LAS CURVAS CREA FLAG 1;
		if (MovingPointSelected == -1) {
			Vector2 MousePos = GetScreenToWorld2D(GetMousePosition(), Camera);

			if (Grid.Active && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
				MousePos = GetClosestGridPoint(&Grid, MousePos);
			}

			Ruler.Flag1 = MousePos;
			Ruler.Active = true;
		}
	}

	// DESELECCIONA EL PUNTO CUANDO SOLTAS EL CLICK IZQUIERDO
	if (IsMouseButtonReleased(0)) {
		this->MovingPointSelected = -1;
	}

	// >> RULER MANAGMENT <<

	if (IsMouseButtonDown(0) && MovingPointSelected == -1) {
		Vector2 MousePos = GetScreenToWorld2D(GetMousePosition(), Camera);

		if (Grid.Active && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
			MousePos = GetClosestGridPoint(&Grid, MousePos);
		}

		Ruler.Flag2 = MousePos;
		Ruler.Length = Vector2Distance(Ruler.Flag1, Ruler.Flag2);
	}

	// >> CREAR PUNTOS << 

	// SELECCIONA UN TRIAL POINT PARA EXPANDIR LA SPLINE
	if (IsMouseButtonPressed(1) && !ClosedTrack) {
		int LastCurve = BezierSpline.size() - 1;
		if (MouseInCircV(&Camera, BezierSpline[LastCurve].P[1], PointRadius)) {
			this->ExpandPointSelected = LastCurve*4 + 1;
		}
	}

	// DESELECCIONA EL PUNTO CUANDO SOLTAS EL CLICK DERECHO
	if (IsMouseButtonReleased(1) && ExpandPointSelected != -1 && !ClosedTrack) {
		Vector2 MousePos = GetScreenToWorld2D(GetMousePosition(), Camera);

		if (Grid.Active && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
			MousePos = GetClosestGridPoint(&Grid, MousePos);
		}

		BezierCurve Curve = BezierSpline.back();
		Curve.P[0] = BezierSpline.back().P[1];
		Curve.P[1] = GetScreenToWorld2D(GetMousePosition(), Camera);
		Curve.P[2] = {
			2 * Curve.P[0].x - BezierSpline.back().P[3].x,
			2 * Curve.P[0].y - BezierSpline.back().P[3].y
		};
		Curve.P[3] = Curve.P[1];

		this->BezierSpline.push_back(Curve);

		this->ExpandPointSelected = -1;
	}

	// >> BORRAR PUNTOS << 
	if (IsMouseButtonPressed(2) && !ClosedTrack) {
		if (BezierSpline.size() > 1) {
			int LastCurve = BezierSpline.size() - 1;
			if (MouseInCircV(&Camera, BezierSpline[LastCurve].P[1], PointRadius)) {
				this->BezierSpline.pop_back();
			}
		}

		if (MouseInCircV(&Camera, Ruler.Flag1, PointRadius) ||
			MouseInCircV(&Camera, Ruler.Flag2, PointRadius)) {

			Ruler.Active = false;
		}
	}
}

void BezierEditor::Draw() {
	BeginDrawing();

	ClearBackground(WHITE);

	BeginMode2D(Camera);

	if (Grid.Active) {
		int StartX = ((int)(Camera.target.x - Camera.offset.x / Camera.zoom) / Grid.Spacing) * Grid.Spacing - Grid.Spacing;
		int StartY = ((int)(Camera.target.y - Camera.offset.y / Camera.zoom) / Grid.Spacing) * Grid.Spacing - Grid.Spacing;

		DrawCircle(StartX, StartY, 10 / Camera.zoom, PINK);

		int LinesNY = std::ceil(sy / (Camera.zoom * (float)Grid.Spacing)) + 2;
		int LinesNX = std::ceil(sx / (Camera.zoom * (float)Grid.Spacing)) + 2;

		for (int i = 0; i < LinesNY; i++) {
			Vector2 LeftPoint = {
				StartX,
				StartY + i * Grid.Spacing
			};
			Vector2 RightPoint = {
				StartX + sx / Camera.zoom + Grid.Spacing,
				StartY + i * Grid.Spacing
			};
			DrawLineEx(LeftPoint, RightPoint, 2 / Camera.zoom, GRAY);
		}


		for (int i = 0; i < LinesNX; i++) {
			Vector2 TopPoint = {
				StartX + i * Grid.Spacing,
				StartY
			};
			Vector2 BotPoint = {
				StartX + i * Grid.Spacing,
				StartY + sy / Camera.zoom + Grid.Spacing
			};
			DrawLineEx(TopPoint, BotPoint, 2 / Camera.zoom, GRAY);
		}
	}


	for (int i = 0; i < BezierSpline.size(); i++) {

		for (int j = 0; j < NPoints; j++) {
			Vector2 Point = GetCurvePoint(&BezierSpline[i], j/NPoints);
			float Slope = GetBezierSlope(&BezierSpline[i], j/NPoints);

			Vector2 Extreme1 = {
				Point.x + TrackWidth * 0.5f * CosOfArctan(Slope),
				Point.y + TrackWidth * 0.5f * SinOfArctan(Slope)
			};

			Vector2 Extreme2 = {
				Point.x - TrackWidth * 0.5f * CosOfArctan(Slope),
				Point.y - TrackWidth * 0.5f * SinOfArctan(Slope)
			};

			DrawLineEx(Extreme1, Extreme2, 2,/* j%2 ? RED:*/ DARKGRAY);
		}

		if (!IsKeyDown(KEY_SPACE)) {
			// DIBUJA LAS LINEAS ENTRE CURVATURE P Y TRIAL P
			DrawLineEx(BezierSpline[i].P[0], BezierSpline[i].P[2], 2 / Camera.zoom, BLACK);
			DrawLineEx(BezierSpline[i].P[1], BezierSpline[i].P[3], 2 / Camera.zoom, BLACK);

			// DIBUJA LOS PUNTOS
			DrawCircleV(BezierSpline[i].P[0], PointRadius / Camera.zoom, DARKGREEN);
			DrawCircleV(BezierSpline[i].P[1], PointRadius / Camera.zoom, DARKGREEN);
			DrawCircleV(BezierSpline[i].P[2], PointRadius / Camera.zoom, GREEN);
			DrawCircleV(BezierSpline[i].P[3], PointRadius / Camera.zoom, GREEN);
		}

		// DIBUJA LA LINEA
		//for (int j = 0; j < NPoints; j++) {
		//	Vector2 Point = GetCurvePoint(&BezierSpline[i], j/NPoints);
		//	DrawCircleV(Point, 2 / Camera.zoom, RED);
		//}

	}

	// FADED POINT WHEN EXPANDING THE SPLINE
	if (IsMouseButtonDown(1) && ExpandPointSelected != -1) {
		Vector2 MousePos = GetScreenToWorld2D(GetMousePosition(), Camera);

		if (Grid.Active && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
			MousePos = GetClosestGridPoint(&Grid, MousePos);
		}

		DrawCircleV(MousePos, PointRadius / Camera.zoom, ColorAlpha(DARKGREEN, 0.5f));
	}

	if (Ruler.Active) {
		DrawCircleV(Ruler.Flag1, PointRadius / Camera.zoom, BLUE);
		DrawCircleV(Ruler.Flag2, PointRadius / Camera.zoom, BLUE);

		DrawLineEx(Ruler.Flag1, Ruler.Flag2, 4 / Camera.zoom, DARKBLUE);
	}

	EndMode2D();

	if (Ruler.Active) {
		std::string RulerLengthStr = NumToStr(Ruler.Length, 2) + " m";
		char* RulerLengthChar = StrToChar(RulerLengthStr);

		Vector2 MidPoint = Vector2Lerp(Ruler.Flag1, Ruler.Flag2, 0.5f);
		MidPoint = GetWorldToScreen2D(MidPoint, Camera);

		if (GetSlopeVec2(Ruler.Flag1, Ruler.Flag2) < 0) {
			DrawText(RulerLengthChar,
				MidPoint.x + 10,
				MidPoint.y + 10,
				u*0.7, BLUE);
		} else {
			DrawText(RulerLengthChar,
				MidPoint.x - 10 - MeasureText(RulerLengthChar, u*0.7),
				MidPoint.y + 10,
				u*0.7, BLUE);
		}

		delete[] RulerLengthChar;
	}

	std::string UnitSizeStr = "Unit: " + IntToStr(Grid.Spacing) + " m";
	char* UnitSizeChar = StrToChar(UnitSizeStr);
	DrawRectangle(0, 0, MeasureText(UnitSizeChar, u) + 20, u + 20, DARKGRAY);
	DrawText(UnitSizeChar, 10, 10, u, WHITE);
	delete[] UnitSizeChar;

	std::string TrackWidthStr = "Width: " + IntToStr(TrackWidth) + " m";
	char* TrackWidthChar = StrToChar(TrackWidthStr);
	DrawRectangle(0, u + 20, MeasureText(TrackWidthChar, u) + 20, u + 20, DARKGRAY);
	DrawText(TrackWidthChar, 10, u + 20 + 10, u, WHITE);
	delete[] TrackWidthChar;

	EndDrawing();
}