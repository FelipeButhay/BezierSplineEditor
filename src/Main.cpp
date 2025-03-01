#include "raylib.h"
#include "BezierEditor.h"

int main() {
	BezierEditor Editor;

	while (!WindowShouldClose()) {
		Editor.Loop();
		Editor.Draw();
	}

	CloseWindow();
	Editor.SaveTrack();

	return 0;
}