#pragma once
#include <vector>
#include <string>
#include "Vertex.h"
#include "EdgeElement.h"

#define FILES_NOT_FOUND "Файли не знайдені"

class FileManager {
private:

	FileManager();

	bool isLoadedNames = false;
	bool isLoadedVector = false;
	int currentFile = 0;
	std::vector<std::string> names;
	std::vector<std::vector<int>> matr;

	std::vector<Vertex> vertices;

	void fillInNames();
	void fillInMatr();
public: 
	std::vector<EdgeElement*> edges;

	static FileManager* getInstance();

	std::vector<std::string> getNames();
	void changeFile(int, HWND);
	std::vector<std::vector<int>> getMatr();
	std::string getCurrentFileName() {
		return names[currentFile];
	}
	void getGraphElements(std::vector<Vertex>* &v,
					std::vector<EdgeElement*>* &e) {
		v = &vertices; e = &edges;
	}
	void update() {
		edges.clear();
		changeFile(currentFile, nullptr);
	}
	void createEmpty(std::string, HWND);
	void relocateVertices();
};