#include "WindowParameters.h"
#include "Logic.h"
#include "EdgeElement.h"

void EdgeElement::drawWeight(HDC hdc, int x, int y, HWND hStatic) {
	WindowParameters* window = WindowParameters::getInstance();
	if (nullptr == hStatic) {
		HGDIOBJ prevFont = SelectObject(hdc, window->hFont);
		TextOut(hdc, x, y, label.c_str(), label.size());
		SelectObject(hdc, prevFont);
	}
	else {
		char buffer[MAX_WEIGHT_LENGTH];
		GetWindowText(hStatic, buffer, MAX_WEIGHT_LENGTH);
		MoveWindow(hStatic, window->origin.x + x, window->origin.y + y,
			window->letterWidth * (strlen(buffer) + 1),
			window->letterHeight, true);
	}
}