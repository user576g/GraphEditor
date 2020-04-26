
#include "Logic.h"
using Logic::edge;
#include "Windows.h"
#include <vector>
using std::vector;
#include <iostream>
#include "WindowParameters.h"
#include "Vertex.h"
#include "EdgeElement.h"
#include "SingleArc.h"
#include "TwoArcs.h"
#include <assert.h>
#include "UndirectedEdge.h"

WindowParameters* windowParam = WindowParameters::getInstance();

void Logic::drawCircle(HDC hdc, int x, int y) {
	WindowParameters* window = WindowParameters::getInstance();
	int radius = window->radiusForVertex;
	Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
}

vector<POINT> Logic::getPositions(int n, int radius) {
	vector<POINT> points;
	if (n < 1) {
		return points;
	} 
	if (1 == n) {
		points.push_back({ 0, 0 });
		return points;
	}
	if (2 == n) {
		points.push_back({ -radius, 0 });
		points.push_back({ radius, 0 });
		return points;
	}
	points.push_back({ 0, -radius });
	float alfa = 360.0 / n;
	float beta = 90 - alfa;
	for (int i = 1; i < n; ++i) {
		int x, y;
		if (-90 == beta) {
			x = 0;
			y = -radius;
		}
		else {
			double tangenBeta = tan(PI * beta / 180);
			std::cout << tangenBeta;
			int plusMinusX = sqrt(radius * radius 
				/ (1 + tangenBeta * tangenBeta));
			if (beta < -90) {
				x = -1 * plusMinusX;
			}
			else {
				x = plusMinusX;
			}
			y = tangenBeta * x;
		}
		points.push_back({ x, -y });
		beta -= alfa;
	}
	return points;
}

vector<edge> Logic::getEdgesArrayFromCostsMatr(vector<vector<int>> coMa) {
	vector<edge> edges;
	for (int i = 0; i < coMa.size(); i++)
		for (int j = 0; j < coMa[0].size(); j++) {
			if (coMa[i][j] != INT_MAX && i != j) {
				edge e;
				e.srcV = i;
				e.targetV = j;
				e.weight = coMa[i][j];
				edges.push_back(e);
			}
		}
	return edges;
}

void Logic::drawGraph(HDC hdc, std::vector<vector<int>> coMa) {

	WindowParameters* window = WindowParameters::getInstance();
	
	if (0 == coMa.size()) {
		RECT rt = {0, 0, window->getWidth(), window->getHeight()};
		DrawText(hdc, "Порожній граф", -1, &rt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		return;
	}

	SetViewportOrgEx(hdc, window->origin.x, window->origin.y, NULL);
	int diameter = window->square.right - window->square.left;

	int n = coMa.size();
	vector<POINT> points = Logic::getPositions(n, diameter / 2);
	vector<Vertex> vertices;
	vertices.reserve(n);
	int i = 0;
	vector<Vertex*> pointVer;
	pointVer.reserve(n);
	for (POINT p : points) {
		vertices.push_back(Vertex(i, p));
		pointVer.push_back(&vertices[i]);
		vertices[i].draw(hdc);
		++i;
	}

	vector<EdgeElement*> edges = Logic::getEdgeElements(coMa, pointVer);
	for(EdgeElement* e : edges) {
		e->draw(hdc);
		delete e;
	}
	SetViewportOrgEx(hdc, 0, 0, NULL);
}

POINT Logic::divedeSection(POINT a, POINT b, int k1, int k2) {
	int d = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	int d1 = d * k1 / (k1 + k2);
	double alfa = getLineAngle(a, b);
	int dx = cos(alfa) * d1;
	int dy = sin(alfa) * d1;
	return {a.x + dx, a.y - dy};
}

int Logic::distance(POINT a, POINT b) {
	b.x -= a.x;
	b.y -= a.y;
	return sqrt(b.x * b.x + b.y * b.y);
}

vector<EdgeElement*> Logic::getEdgeElements(vector<vector<int>> coMa, 
												vector<Vertex*>& vertices) {
	vector<EdgeElement*> edges;
	
	int num = coMa.size();
	bool** isUsed = new bool*[num];
	for (int i = 0; i < num; ++i) {
		isUsed[i] = new bool[num];
		for (int j = 0; j < num; ++j) {
			isUsed[i][j] = false;
		}
	}

	for (int i = 0; i < coMa.size(); i++)
		for (int j = 0; j < coMa[0].size(); j++) {
			if (coMa[i][j] != INT_MAX && i != j && !isUsed[i][j]) {
				EdgeElement* e;
				if (coMa[j][i] != INT_MAX && !isUsed[j][i]) {
					if (coMa[j][i] == coMa[i][j]) {
						e = new UndirectedEdge(vertices[i], vertices[j],
							coMa[i][j]);
					}
					else {
						e = new TwoArcs(vertices[i], vertices[j],
											coMa[i][j], coMa[j][i]);
					}			
					isUsed[i][j] = true;
					isUsed[j][i] = true;
				}
				else {
					e = new SingleArc(vertices[i], vertices[j], coMa[i][j]);
					isUsed[i][j] = true;
				}
				edges.push_back(e);
			}
		}

	for (int i = 0; i < num; ++i) {
		delete[] isUsed[i];
	}
	delete[] isUsed;

	return edges;
}

void Logic::drawBigLetter(HDC hdc, RECT& square, char number) {
	HDC hMem = CreateCompatibleDC(hdc);
	TEXTMETRIC tm;
	GetTextMetrics(hMem, &tm);
	HBITMAP hBit = CreateCompatibleBitmap(
		hdc,        // дескриптор DC
		tm.tmAveCharWidth * 5 / 2,     // ширина рисунка, в пикселях
		tm.tmHeight     // высота рисунка, в пикселях
	);
	SelectObject(hMem, hBit);
	char letter[5] = "    ";
	letter[1] = number;
	SetBkColor(hMem, windowParam->colors.colVertexBkrg);
	SetTextColor(hMem, windowParam->colors.colVertexName);
	TextOut(hMem, 0, 0, letter, 5);
	StretchBlt(hdc, square.left, square.top,
		square.right - square.left, square.bottom - square.top,
		hMem, 0, 0,
		tm.tmAveCharWidth * 5 / 2, tm.tmHeight, SRCCOPY);
	DeleteDC(hMem);
	DeleteObject(hBit);
}

PBITMAPINFO Logic::CreateBitmapInfoStruct(HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	assert(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp));

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void Logic::CreateBMPFile(LPCSTR pszFile, HBITMAP hBMP)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE *hp;                   // byte pointer  
	DWORD dwTmp;
	PBITMAPINFO pbi;
	HDC hDC;

	hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
	SelectObject(hDC, hBMP);

	pbi = CreateBitmapInfoStruct(hBMP);

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	assert(lpBits);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	assert(GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS));

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	assert(hf != INVALID_HANDLE_VALUE);

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	assert(WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL));

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	assert(WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL)));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	assert(WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL));

	// Close the .BMP file.  
	assert(CloseHandle(hf));

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}