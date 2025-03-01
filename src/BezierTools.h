#pragma once

#include "Tools.h"
#include "raylib.h"
#include "raymath.h"

#define NPoints 1000.0

typedef struct BezierCurve {
	Vector2 P[4];
};

typedef struct Grid {
	int Spacing;
	bool Active;
};

typedef struct Ruler {
	Vector2 Flag1;
	Vector2 Flag2;

	float Length;
	bool Active;
};

Vector2 GetCurvePoint(BezierCurve* Curve, float t);

Vector2 GetClosestGridPoint(Grid* Grid, Vector2 Point);

Vector2 GetMirrorPoint(Vector2 Point, Vector2 Pivot);

float GetBezierSlope(BezierCurve* Curve, float t);

int GetGridSpacing(int sx, float Zoom);