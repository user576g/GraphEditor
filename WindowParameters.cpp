#include "WindowParameters.h"

WindowParameters::WindowParameters() {
	updateParameters(initialWidth, initialHeight);
}

WindowParameters* WindowParameters::getInstance() {
	static WindowParameters instance;
	return &instance;
}
void WindowParameters::updateParameters(int newW, int newH, HWND h) {
	width = newW;
	height = newH;

	squareLength = height > width ? width : height;
	squareLength *= coefForSquare;
	square = {
			(width - squareLength) / 2,
			(height - squareLength) / 2,
			(width - squareLength) / 2 + squareLength,
			(height - squareLength) / 2 + squareLength
	};
	origin = {
		(square.right + square.left) / 2,
		(square.bottom + square.top) / 2
	};
	coefForGraph = squareLength / (double)initialSquareLength;
	radiusForVertex = initialRadiusForVertex * coefForGraph;
	vertexSecondRadius = radiusForVertex;

	bigRadius = squareLength >> 1;

	DeleteObject(hFont);
	letterWidth = 20 * coefForGraph;
	letterHeight = 25 * coefForGraph;
	hFont = CreateFont(letterHeight, letterWidth, 0, 0, 700 * coefForGraph,
		3, false, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, ("Arial"));

	hWnd = h;
}