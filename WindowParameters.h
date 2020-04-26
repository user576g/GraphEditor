#pragma once
#include <Windows.h>

class WindowParameters {

	struct Colors {
		HBRUSH bkrg, arc, edge, selectedEdge,
			vertexBkrg, vertexName, edgeWeight;
		COLORREF colBkrg, colArc, colEdge, colSelectedEdge, colVertexBkrg,
			colVertexName, colEdgeWeight;

		COLORREF acrCustClr[16];	// массив пользовательских цветов
		CHOOSECOLOR buf_color;

		HWND hDialog;
		bool isVertexBkrgChanged;

		Colors() {
			bkrg = GetSysColorBrush(COLOR_WINDOW + 1);
			colBkrg = GetSysColor(COLOR_WINDOW + 1);
			
			colArc = RGB(0, 0, 0);
			arc = CreateSolidBrush(colArc);

			colEdge = RGB(0, 0, 0);
			edge = CreateSolidBrush(colEdge);

			colSelectedEdge = RGB(0, 255, 0);
			selectedEdge = CreateSolidBrush(colSelectedEdge);

			colVertexBkrg = GetSysColor(COLOR_WINDOW);
			vertexBkrg = GetSysColorBrush(COLOR_WINDOW);

			colVertexName = RGB(0, 0, 0);
			vertexName = CreateSolidBrush(colVertexName);

			colEdgeWeight = RGB(0, 0, 0);
			edgeWeight = CreateSolidBrush(colEdgeWeight);

			buf_color.lStructSize = sizeof(buf_color);
			buf_color.hwndOwner = NULL;
			buf_color.lpCustColors = (LPDWORD)acrCustClr;
			buf_color.Flags = CC_FULLOPEN | CC_RGBINIT;
		}

		//bkrg --- 0
		//1 --- arc
		//3 --- edge
		//6 --- selectedEdge
		//4 --- vertexName
		//5 --- vertexBkgr
		//2 --- edgeWeight

		void changeColor(int i) {
			COLORREF *chagingColor = nullptr;
			HBRUSH *changingBrush = nullptr;
			switch (i) {
			case 0:
				chagingColor = &colBkrg;
				changingBrush = &bkrg;
				break;
			case 1:
				chagingColor = &colArc;
				changingBrush = &arc;
				break;
			case 2:
				chagingColor = &colEdgeWeight;
				changingBrush = &edgeWeight;
				break;
			case 3:
				chagingColor = &colEdge;
				changingBrush = &edge;
				break;
			case 5:
				chagingColor = &colVertexBkrg;
				changingBrush = &vertexBkrg;
				isVertexBkrgChanged = false;
				break;
			case 4:
				chagingColor = &colVertexName;
				changingBrush = &vertexName;
				break;
			case 6:
				chagingColor = &colSelectedEdge;
				changingBrush = &selectedEdge;
				break;

			}
			buf_color.rgbResult = *chagingColor;
			if (ChooseColor(&buf_color))
			{
				*chagingColor = buf_color.rgbResult;
				
				DeleteObject(*changingBrush);
				*changingBrush = CreateSolidBrush(buf_color.rgbResult);

				InvalidateRect(hDialog, NULL, TRUE);
				InvalidateRect(GetParent(hDialog), NULL, true);
			}
		}
	};

	WindowParameters();
	int width, height;
	int squareLength;
	HWND hWnd;
public:
	const static int initialWidth = 900, initialHeight = 600;
	const float coefForSquare = 0.8;
	const int initialSquareLength = initialHeight * coefForSquare;
	const int initialRadiusForVertex = 50;

	POINT origin;
	RECT square, whereWas;
	double coefForGraph = squareLength / (double)initialSquareLength;
	//vertex second radius depends on the number of vertices
	int radiusForVertex, vertexSecondRadius;
	int bigRadius;
	Colors colors;
	HFONT hFont;
	int letterWidth = 20 * coefForGraph;
	int letterHeight = 25 * coefForGraph;
	
	void updateParameters(int, int, HWND = nullptr);
	static WindowParameters* getInstance();
	
	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}
	
	int getSquareLength() {
		return squareLength;
	}

	~WindowParameters() {
		DeleteObject(hFont);
	}
};