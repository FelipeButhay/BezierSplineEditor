#include "BezierTools.h"

Vector2 GetCurvePoint(BezierCurve* Curve, float t) {
	Vector2 A = Vector2Lerp(Curve->P[0], Curve->P[2], t);
	Vector2 B =	Vector2Lerp(Curve->P[2], Curve->P[3], t);
	Vector2 C =	Vector2Lerp(Curve->P[3], Curve->P[1], t);
	Vector2 D =	Vector2Lerp(A, B, t);
	Vector2 E =	Vector2Lerp(B, C, t);
	Vector2 P =	Vector2Lerp(D, E, t);

	return P;
}

Vector2 GetClosestGridPoint(Grid* Grid, Vector2 Point) {
	Vector2 FinalVector = {
		std::ceil(Point.x / Grid->Spacing - 0.5) * Grid->Spacing,
		std::ceil(Point.y / Grid->Spacing - 0.5) * Grid->Spacing
	};

	return FinalVector;
}

Vector2 GetMirrorPoint(Vector2 Point, Vector2 Pivot) {
	return {
		2 * Pivot.x - Point.x,
		2 * Pivot.y - Point.y,
	};
}

float GetBezierSlope(BezierCurve* Curve, float t1) {
	float t2 = t1 + 1.0f / NPoints;

	Vector2 P1 = GetCurvePoint(Curve, t1);
	Vector2 P2 = GetCurvePoint(Curve, t2);

	return GetTanSlopeVec2(P1, P2);
}

int GetGridSpacing(int sx, float Zoom) {
	int IntendedLinesX = 60;
	int NotRoundedSpacing = sx / (Zoom * IntendedLinesX);

	int e = log10(NotRoundedSpacing);

	if (e < 0) {
		return 1;
	}

	if        ( 2 * pow(10, e) > NotRoundedSpacing) {
		return  2 * pow(10, e);
	} else if ( 5 * pow(10, e) > NotRoundedSpacing) {
		return  5 * pow(10, e);
	} else {
		return 10 * pow(10, e);
	}
}