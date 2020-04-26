#include "Vertex.h"
#include "Logic.h"
#include "Windows.h"
#include "WindowParameters.h"

void Vertex::draw(HDC hdc) {
	WindowParameters* window = WindowParameters::getInstance();
	HBRUSH prevBrush = (HBRUSH) SelectObject(hdc, window->colors.vertexBkrg);
	Logic::drawCircle(hdc, center.x, center.y);
	int radius = window->radiusForVertex;
	int halfSmallerSquare = radius / sqrt(2);
	RECT square = {
		center.x - halfSmallerSquare, center.y - halfSmallerSquare,
		center.x + halfSmallerSquare, center.y + halfSmallerSquare
	};
	if (1 == name.size()) {
		Logic::drawBigLetter(hdc, square, name[0]);
	}
	else {
		DrawText(hdc, name.c_str(), -1, &square, DT_SINGLELINE |
	DT_CENTER | DT_VCENTER | DT_MODIFYSTRING | DT_END_ELLIPSIS);
	}
	SelectObject(hdc, prevBrush);
}

Vertex::Vertex(int num, POINT p) {
	if (num < 0) {
		// write to Logger
	}
	number = num;
	center = p;
	char buf[] = "X";
	buf[0] = 65 + number;
	name = std::string(buf);
}

Vertex::Vertex(char* s, POINT p, int n) {
	number = n;
	center = p;
	name = std::string(s);
}

POINT Vertex::getCenter() {
	return center;
}

Vertex::Vertex(){
	number = 0;
	center = {0, 0};
}

void Vertex::setCenter(POINT p) {
	center = p;
}