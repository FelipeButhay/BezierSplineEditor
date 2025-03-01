#include "Tools.h"
#include "raylib.h"

bool MouseInRect(int posx, int posy, int width, int height) {
    int mx = GetMouseX(), my = GetMouseY();
    return (mx > posx && mx < posx+width) && (my > posy && my < posy+height);
}

bool MouseInCirc(Camera2D* Camera, int posx, int posy, int Radius) {
    Vector2 Mouse = GetScreenToWorld2D(GetMousePosition(), *Camera);

    Vector2 Pos = { posx, posy };
    float h = Vector2Distance(Pos, Mouse);

    return h < Radius / Camera->zoom;
}

bool MouseInCircV(Camera2D* Camera, Vector2 Pos, int Radius) {
    Vector2 Mouse = GetScreenToWorld2D(GetMousePosition(), *Camera);
    float h = Vector2Distance(Pos, Mouse);
    return h < Radius / Camera->zoom;
}

char* StrToChar(std::string& str) {
    char* cstr = new char[str.length() + 1];

    for (int i = 0; i<(int)str.length(); i++) {
        cstr[i] = str[i];
    }
    cstr[str.length()] = '\0';

    return cstr;
}

// EJEMPLO DE PASAR STD::STR A CHAR*
// 
// std::string textStr = "...";
// char* textChar = strToChar(textStr);
// DrawText(textChar,...);
// delete[] textChar;

std::string NumToStr(float num, int Decimals) {
    std::string str = std::to_string(num);
    int BeforePointNumbers = std::ceil(log10(num) < 0 ? 1 : log10(num));
    
    return str.substr(0, BeforePointNumbers + Decimals + 1);
}

std::string IntToStr(int num) {
    std::string str = std::to_string(num);
    return str;
}

float GetSlopeVec2(Vector2 P1, Vector2 P2) {
    return (P1.x == P2.x) ? INFINITY : (P2.y - P1.y) / (P2.x - P1.x);
}

float GetTanSlopeVec2(Vector2 P1, Vector2 P2) {
    return (P1.y == P2.y) ? INFINITY : (P1.x - P2.x) / (P2.y - P1.y);
}

float SinOfArctan(float x) {
    return x / (sqrt(1 + x*x));
}

float CosOfArctan(float x) {
    return 1.0f / (sqrt(1 + x*x));
}
