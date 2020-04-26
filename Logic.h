#ifndef LOGIC
#define LOGIC

#define MAX_WEIGHT_LENGTH 101

#include <Windows.h>
#include <vector>
#include <string>
#include "WindowParameters.h"
#include "EdgeElement.h"
#include "Vertex.h"

namespace Logic {

	const double PI = 3.14159265358979;
	void drawCircle(HDC, int x, int y);
	std::vector<POINT> getPositions(int, int);

	template<typename p>
	double getLineAngle(p src, p dst) {
		double alfa = atan((-dst.y + src.y) / (double)(dst.x - src.x));
		if (dst.x < src.x) {
			if (dst.y < src.y) {
				alfa += PI;
			}
			else {
				alfa -= PI;
			}

		}
		return alfa;
	}

	template<typename p>
	void directionalLine(HDC hdc, p src, p dst, int h = 50, int a = 20) {
		MoveToEx(hdc, src.x, src.y, NULL);
		LineTo(hdc, dst.x, dst.y);

		double alfa = getLineAngle(src, dst);
		double beta = atan(a / (double)h);
		
		POINT triangle[3] = {
			{dst.x, dst.y}, 
			{dst.x + a * sin(alfa - PI / 2 + beta), dst.y + a * cos(alfa - PI / 2 + beta) },
			{dst.x + a * sin(alfa - PI / 2 - beta), dst.y + a * cos(alfa - PI / 2 - beta)}
		};
		Polygon(hdc, triangle, 3);
	}

	template<typename p>
	void directionalArrow(HDC hdc, p src, p dst, int h = 50, int a = 20) {
		double alfa = getLineAngle(src, dst);
		double beta = atan(a / (double)h);

		POINT triangle[3] = {
			{dst.x, dst.y},
			{dst.x + a * sin(alfa - PI / 2 + beta), dst.y + a * cos(alfa - PI / 2 + beta) },
			{dst.x + a * sin(alfa - PI / 2 - beta), dst.y + a * cos(alfa - PI / 2 - beta)}
		};
		Polygon(hdc, triangle, 3);
	}

	struct edge
	{
		unsigned int srcV, targetV;
		double weight;
	};
	std::vector<edge> getEdgesArrayFromCostsMatr(std::vector<std::vector<int>>);
	void drawGraph(HDC, std::vector<std::vector<int>>);
	POINT divedeSection(POINT, POINT, int, int);
	int distance(POINT, POINT);
	std::vector<EdgeElement*> getEdgeElements(std::vector<std::vector<int>>, 
									std::vector<Vertex*>&);
	void drawBigLetter(HDC hdc, RECT& square, char);

	PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
	void CreateBMPFile(LPCSTR pszFile, HBITMAP hBMP);
}

#endif // LOGIC