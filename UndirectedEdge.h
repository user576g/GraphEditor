#pragma once
#include "EdgeForPair.h"

class UndirectedEdge : public EdgeForPair {
public:
	UndirectedEdge(Vertex* v1, Vertex* v2, double w) : EdgeForPair(v1, v2, w) {}
	UndirectedEdge(Vertex* v1, Vertex* v2, std::string w) : EdgeForPair(v1, v2, w) {}

	virtual void draw(HDC hdc, HWND static1 = nullptr, HWND static2 = nullptr, COLORREF color = NULL) {
		WindowParameters* window = WindowParameters::getInstance();
		if (NULL == color) {
			color = window->colors.colEdge;
		}
		
		HPEN hPen = CreatePen(PS_SOLID, 5, color);
		HGDIOBJ prevPen = SelectObject(hdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(color);
		HGDIOBJ prevBrush = SelectObject(hdc, hBrush);

		POINT a, b;
		doSectionSmaller(a, b);
		MoveToEx(hdc, a.x, a.y, NULL);
		LineTo(hdc, b.x, b.y);

		HGDIOBJ prevFont = SelectObject(hdc, window->hFont);

		drawOnlyOneWeight(hdc, static1);

		SelectObject(hdc, prevFont);
		SelectObject(hdc, prevPen);
		SelectObject(hdc, prevBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}
};