#pragma once
#include <Windows.h>
#include "Vertex.h"

class EdgeElement {
protected:
	Vertex* A;
	double weight;
	bool isLabelNumber;
	std::string label;

	EdgeElement(Vertex* v1, double w) {
		A = v1; weight = w;
		int z = weight;
		if (0 != w - z) {
			label = std::to_string(weight);
		}
		else {
			label = std::to_string(z);
		}
		isLabelNumber = true;
	}
	
	EdgeElement(Vertex* v1, std::string s) {
		A = v1; label = s;
		isLabelNumber = false;
	}

	void drawWeight(HDC hdc, int x, int y, HWND hStatic);

public :
	virtual void draw(HDC, HWND = nullptr, HWND = nullptr, COLORREF = NULL) = 0;
	
	std::string getWeight() {
		return label;
	}
	Vertex* getAver() {
		return A;
	}

	void setAver(Vertex* a) {
		A = a;
	}
};