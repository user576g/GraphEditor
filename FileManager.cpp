#include "FileManager.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <Windows.h>
#include <fstream>
using std::ifstream;
#include "WindowParameters.h"

FileManager* FileManager::getInstance() {
	static FileManager instance;
	return &instance;
}

vector<string> FileManager::getNames() {
	if (!isLoadedNames) {
		fillInNames();
		isLoadedNames = true;
	}
	vector<string>  r;
	for (string s : names) {
		r.push_back(s.substr(0, s.size() - 4));
	}
	return r;
}

void FileManager::fillInNames() {
	char buffer[100];
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile("Matrices\\*.gph", &findData);
	isLoadedNames = true;
	if (INVALID_HANDLE_VALUE == hFind) {
		names.push_back(FILES_NOT_FOUND);
		return;
	}
	names.push_back(findData.cFileName);
	while (FindNextFile(hFind, &findData)) {
		names.push_back(findData.cFileName);
	}
}

#include "EdgeClasses.h"
#include "Logic.h"

void FileManager::changeFile(int j, HWND hCombo) {
	if (1 == names.size() && FILES_NOT_FOUND == names[0]) {
		return;
	}
	currentFile = j;
	SendMessage(hCombo, (UINT)CB_SETCURSEL, j, 0);

	WindowParameters* window = WindowParameters::getInstance();
	int diameter = window->square.right - window->square.left;
	vector<POINT> points;

	for (EdgeElement* e : edges) {
		delete e;
	}
	edges.clear();
	vertices.clear();
	
	ifstream fin(("Matrices\\" + names[currentFile]));
	int numVer;		fin >> numVer;
	points = Logic::getPositions(numVer, diameter / 2);
	vertices.reserve(numVer);
	fin.ignore(2);
	const int  m = MAX_WEIGHT_LENGTH;
	char buff[m];
	for (int i = 0; i < numVer; ++i) {
		fin.getline(buff, m);
		vertices.push_back(Vertex(buff, points[i], i));
	}
	fin.ignore(1);

	int numEdges;	fin >> numEdges;
	edges.reserve(numEdges);
	for (int i = 0; i < numEdges; ++i) {
		unsigned from, to, type;
		fin >> from >> to >> type;
		fin.ignore(1);
		fin.getline(buff, m);
		EdgeElement* e;
		if (1 == type) {
			e = new SingleArc(&vertices[from], &vertices[to], buff);
		} else if (2 == type) {
			char second[m];
			fin.getline(second, m);
			e = new TwoArcs(&vertices[from], &vertices[to], buff, second);
		}
		else if (3 == type) {
			e = new UndirectedEdge(&vertices[from], &vertices[to], buff);
		}
		edges.push_back(e);
	}

	fin.close();

	InvalidateRect(GetParent(hCombo), NULL, true);
}

void FileManager::fillInMatr() {
	if (!isLoadedNames) {
		fillInNames();
		isLoadedNames = true;
	}

	for (int i = 0; i < matr.size(); ++i) {
		matr[i].clear();
	}
	matr.clear();

	if (1 == names.size() && FILES_NOT_FOUND == names[0]) {
		return;
	}

	ifstream inputFromFile("Matrices\\" + names[currentFile]);
	int numVer, numDirectedEdges;
	inputFromFile >> numVer;

	matr.reserve(numVer);
	char tempForN[10];
	for (int i = 0; i < numVer; i++) {
		vector<int> line;
		line.reserve(numVer);
		for (int j = 0; j < numVer; j++) {
			inputFromFile >> tempForN;
			if (tempForN[0] == 'n')
				line.push_back(2147483647);
			else
				line.push_back(atoi(tempForN));
		}
		matr.push_back(line);
	}
	inputFromFile.close();
}

vector<vector<int>> FileManager::getMatr() {
	if (!isLoadedVector) {
		fillInMatr();
		isLoadedVector = true;
	}
	return matr;
}

FileManager::FileManager() {
	fillInNames();
}

void FileManager::createEmpty(string s, HWND hCombo) {
	char temp[300];
	GetCurrentDirectory(300, temp);
	string path(temp);
	path.append("\\Matrices\\");
	path.append(s);
	path.append(".gph");

	std::ofstream fout(path);
	fout << '0' << "\n\n\n" << '0';
	fout.close();

	names.push_back(s + ".gph");		
	SendMessage(hCombo, (UINT)CB_ADDSTRING, 0, (LPARAM)s.c_str());
	changeFile(names.size() - 1, hCombo);
}

void FileManager::relocateVertices() {
	WindowParameters* param = WindowParameters::getInstance();
	int diameter = param->square.right - param->square.left;
	vector<POINT> points 
		= Logic::getPositions(vertices.size(), diameter / 2);

	vector<Vertex>::iterator it = vertices.begin();
	for (POINT p : points) {
		it->setCenter(p);
		++it;
	}
}