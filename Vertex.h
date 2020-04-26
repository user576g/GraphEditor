#pragma once
#include "Windows.h"
#include <string>

class Vertex {
	unsigned number;
	POINT center;
public:
	Vertex(int, POINT);
	Vertex(char*, POINT, int);
//	Vertex(const Vertex&);
	void draw(HDC);
	POINT getCenter();
	void setCenter(POINT p);
	Vertex();
	unsigned getNumber() {
		return number;
	}

	std::string name;
};
