#pragma once
#include <string>
#include <cmath>
#include <cstring>

#include "raylib.h"
#include "raymath.h"

typedef struct IntVector2 {
	int x;
	int y;
};

bool MouseInRect(int posx, int posy, int width, int height);

bool MouseInCirc(Camera2D* Camera, int posx, int posy, int radius);

bool MouseInCircV(Camera2D* Camera, Vector2 Pos, int Radius);

char* StrToChar(std::string& str);

std::string NumToStr(float num, int Decimals);

std::string IntToStr(int num);

float GetSlopeVec2(Vector2 P1, Vector2 P2);

float GetTanSlopeVec2(Vector2 P1, Vector2 P2);

float SinOfArctan(float x);

float CosOfArctan(float x);

