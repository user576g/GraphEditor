#pragma once
#include "EdgeForPair.h"
#include <Windows.h>
#include "Logic.h"
#include <string>

class SingleArc : public EdgeForPair {
public:

	SingleArc(Vertex* v1, Vertex* v2, double w) : EdgeForPair(v1, v2, w) {}
	SingleArc(Vertex* v1, Vertex* v2, std::string w) : EdgeForPair(v1, v2, w) {}

	virtual void draw(HDC hdc, HWND static1 = nullptr, HWND static2 = nullptr, COLORREF color = NULL) {
		WindowParameters* window = WindowParameters::getInstance();
		if (NULL == color) {
			color = window->colors.colArc;
		}

		POINT a, b;
		doSectionSmaller(a, b);
		int h = 50 * window->coefForGraph;
		int aK = 20 * window->coefForGraph;

		drawOnlyOneWeight(hdc, static1);

		HPEN hPen = CreatePen(PS_SOLID, 1, color);
		HGDIOBJ prevPen = SelectObject(hdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(color);
		HGDIOBJ prevBr = SelectObject(hdc, hBrush);

		Logic::directionalLine(hdc, a, b, h, aK);

		DeleteObject(hBrush);
		DeleteObject(hPen);

	}
};