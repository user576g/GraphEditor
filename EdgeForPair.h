#pragma once
#include "EdgeElement.h"
#include "Vertex.h"
#include "Windows.h"
#include "Logic.h"

class EdgeForPair : public EdgeElement {
protected : 
	Vertex* B;

	EdgeForPair(Vertex* v1, Vertex* v2, double w) : EdgeElement(v1, w) {
		B = v2;
	}

	EdgeForPair(Vertex* v1, Vertex* v2, std::string w) : EdgeElement(v1, w) {
		B = v2;
	}

	void doSectionSmaller(POINT& a, POINT& b) {
		a = A->getCenter();
		b = B->getCenter();
		double alfa = Logic::getLineAngle(a, b);
		WindowParameters* window = WindowParameters::getInstance();
		int radius = window->vertexSecondRadius;
		int dx = radius * cos(alfa);
		int dy = radius * sin(alfa);
		a.x += dx;
		b.x -= dx;
		a.y -= dy;
		b.y += dy;
	}

	void drawOnlyOneWeight(HDC hdc, HWND static1 = nullptr) {
		POINT a, b;
		doSectionSmaller(a, b);
		POINT forNum;
		int distance = Logic::distance(a, b);
		
		WindowParameters* window = WindowParameters::getInstance();
		int letterWidth = 20 * window->coefForGraph;
		int h = 50 * window->coefForGraph;
		
		if (window->bigRadius < distance) {
			forNum = Logic::divedeSection(a, b, 3, 1);
		}
		else if (a.x < b.x && a.y < b.y) {
			distance >>= 1;
			if (letterWidth + h > distance) {
				forNum = a;
			}
			else {
				forNum = Logic::divedeSection(a, b, 1, 1);
			}
		}
		else {
			forNum = Logic::divedeSection(a, b, 1, 1);
		}
		drawWeight(hdc, forNum.x, forNum.y, static1);
	}

public :
	Vertex* getBver() {
		return B;
	}
	void setBver(Vertex* b) {
		B = b;
	}
};
