#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

#include <time.h>

#include "BezierTools.h"
#include "Tools.h"
#include "raylib.h"

class BezierEditor {
	private:
	short sx, sy, u;

	unsigned short resolution;
	short difResolutions[8][2] = { 
		{  900,  500 }, { 1280,  720 }, { 1366,  768 }, { 1536,  864 },
		{ 1600,  900 }, { 1920, 1080 }, { 2560, 1440 }, { 3840, 2160 } 
	};

	int MovingPointSelected;
	int ExpandPointSelected;

	bool ClosedTrack;
	BezierCurve LastCurveSave;

	short TrackWidth;
	short PointRadius;
	Vector2 PointOffset;

	std::vector<BezierCurve> BezierSpline;
	Camera2D Camera;
	Grid Grid;
	Ruler Ruler;

	public:

	BezierEditor();

	void Loop();
	void Draw();
	void SaveTrack();
	bool LoadTrack();
};