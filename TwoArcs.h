#pragma once
#include "EdgeForPair.h"
#include <string>

class TwoArcs : public EdgeForPair {
	double secondWeight;
	std::string secWeight;
public: 
	TwoArcs(Vertex* v1, Vertex* v2, double w1, double w2) : EdgeForPair(v1, v2, w1) {
		secondWeight = w2;
		secWeight = std::to_string(w2);
	}
	TwoArcs(Vertex* v1, Vertex* v2, char* w1, char* w2) : EdgeForPair(v1, v2, w1) {
		secWeight = w2;
	}

	virtual void draw(HDC hdc, HWND static1 = nullptr, HWND static2 = nullptr, COLORREF color = NULL) {
		WindowParameters* window = WindowParameters::getInstance();
		if (NULL == color) {
			color = window->colors.colArc;
		}
		HPEN hPen = CreatePen(PS_SOLID, 1, color);
		HGDIOBJ prevPen = SelectObject(hdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(color);
		HGDIOBJ prevBrush = SelectObject(hdc, hBrush);

		POINT a, b;
		doSectionSmaller(a, b);

		HGDIOBJ prevFont = SelectObject(hdc, window->hFont);

		double alfa = Logic::getLineAngle(a, b);
		double beta;
		if (Logic::PI / 2 < alfa) {
			beta = alfa - Logic::PI / 2;
		}
		else {
			beta = alfa + Logic::PI / 2;
		}
		POINT m = Logic::divedeSection(a, b, 1, 1);
				int distance = Logic::distance(a, b);
		int dx = 0.4 * distance * cos(beta);
		int dy = 0.4 * distance * sin(beta);
		m.x += dx;
		m.y += dy;
		POINT points[4] = { a, m, m, b };
		drawWeight(hdc, m.x, m.y, static1);
		PolyBezier(hdc, points, 4);

		int h = 50 * window->coefForGraph;
		int aK = 20 * window->coefForGraph;
		Logic::directionalArrow(hdc, m, b, h, aK);

		m.x -= dx << 1;
		m.y -= dy << 1;
		points[1] = m; points[2] = m;
	//	std::string s;
		if (nullptr == static2) {
	//		s = std::to_string(secondWeight);
			TextOut(hdc, m.x, m.y, secWeight.c_str(), secWeight.size());
		}
		else {
			drawWeight(hdc, m.x, m.y, static2);
		}
		PolyBezier(hdc, points, 4);
		Logic::directionalArrow(hdc, m, a, h, aK);

		SelectObject(hdc, prevFont);
		SelectObject(hdc, prevPen);
		SelectObject(hdc, prevBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	std::string getSecondWeight() {
		return secWeight;
	}
};