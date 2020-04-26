#pragma once
#include <Windows.h>
#include "resource.h"
#include "TwoArcs.h"
#include <unordered_map>
#include "DlgProcsInEditWind.h"
#include "SingleArc.h" 

extern HINSTANCE hInst;

vector<HWND> vertWins;
int getIndexByValue(HWND val) {
	std::vector<HWND>::iterator it = std::find(vertWins.begin(), vertWins.end(), val);
	if (it == vertWins.end())
		if (vertWins.size() > 0) {
			MessageBox(vertWins[0], "Vertex was not found in vertex vecor ",
				"Critical error", MB_OK);
		}
	return std::distance(vertWins.begin(), it);
}

struct VertexInEdit;
typedef std::unordered_map<HWND, VertexInEdit> T_Map;
T_Map map;

struct EdgeInEdit {
	EdgeElement* edEl;
	HWND static1, static2;
	COLORREF color = RGB(0, 0, 0);
	HWND hVerA, hVerB;

	EdgeInEdit(EdgeElement* e, HWND s1, HWND s2, HWND from, HWND to) {
		edEl = e; static1 = s1; static2 = s2;
		hVerA = from; hVerB = to;
	}
};
typedef list<EdgeInEdit> L_T;
L_T edges;
L_T::iterator findEdgeInEdit(HWND h) {
	L_T::iterator it = edges.begin();
	while (edges.end() != it) {
		if (it->static1 == h) {
			return it;
		}
		it++;
	}
	if (vertWins.size() > 0) {
		MessageBox(vertWins[0], "Edge was not found in edges list",
			"Critical error", MB_OK);
	}
	return edges.end();
}

struct VertexInEdit {
	Vertex ver;
	list<HWND> links;
};

void setTextAndResize(HWND h, string s) {
	SetWindowText(h, s.c_str());
	WindowParameters* param = WindowParameters::getInstance();
	SetWindowPos(h, HWND_TOP, 0, 0, (s.size() + 1) * param->letterWidth,
		param->letterHeight, SWP_NOMOVE);
}

void arrangeVertices(vector<HWND>& v, T_Map& map) {

	int n = v.size();
	WindowParameters* param = WindowParameters::getInstance();
	int diameter = param->square.right - param->square.left;
	vector<POINT> points = Logic::getPositions(n, diameter / 2);
	
	int radius = param->radiusForVertex;
	
	double coef;
	if (n <= 2) {
		coef = 1.8;
	} else if (n <= 7) {
		coef = 1.2;
	} else {
		coef = 1.188'636'6 / 4 
			* Logic::PI * diameter / (n * radius);
	}
	radius *= coef;
	param->vertexSecondRadius = radius;
	int a = 2 * radius;

	int oX = param->origin.x;
	int oY = param->origin.y;

//	int i = 0;
	vector<HWND>::iterator it = v.begin();
	for (POINT p : points) {
		MoveWindow(*it, oX + p.x - radius,
			oY + p.y - radius, a, a, true);
		HRGN hRegion = CreateEllipticRgn(0, 0, a, a);
		SetWindowRgn(*it, hRegion, true);
		VertexInEdit& vin = (map.find(*it))->second;
		vin.ver.setCenter(p);
//		++i;
		++it;
	}
}

WNDPROC standardBtnProc;
HWND selectedVertex;

void selectVertex(HWND choice) {
	HWND temp = selectedVertex;
	selectedVertex = choice;
	if (nullptr != temp) {
		InvalidateRect(temp, NULL, true);
		UpdateWindow(temp);
	}
	if (nullptr != selectedVertex) {
		SetFocus(selectedVertex);
		InvalidateRect(selectedVertex, NULL, true);
	}
}

LRESULT CALLBACK fakeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_KEYUP:
		if (0x30 <= wParam && wParam <= 0x39) {
			if (vertWins.size() == 0) {
				break;
			}
			int x = ((int) wParam) - 0x31;
			if (x < 0) {
				selectVertex(vertWins.front());
			}
			else {
				if (x < vertWins.size()) {
					selectVertex(vertWins[x]);
				}
				else {
					selectVertex(vertWins.back());
				}
			}
			return 0;
		}
		break;
	}
	return standardBtnProc(hDlg, message, wParam, lParam);
}

void deleteEdge(L_T::iterator it) {
	map[it->hVerA].links.remove(it->static1);
	VertexInEdit* verBlink = &map[it->hVerB];
	if (nullptr != verBlink) {
		verBlink->links.remove(it->static1);
	}
	delete it->edEl;
	if (it->static1 != nullptr) {
		DestroyWindow(it->static1);
	}
	if (it->static2 != nullptr) {
		DestroyWindow(it->static2);
	}
	edges.erase(it);
}

namespace AddEdgeDlgResults {
	bool isItNumber;
	double number;
	char buffer[101];
}
#define WRONG_MESS MessageBox(hDlg, "Ви ввели не число. Введіть коректне значення.", \
												"Неправильні дані", MB_OK);
int CALLBACK addNewEdgeDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WindowParameters* param;
	//	HWND parent;
	switch (message)
	{
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case IDOK:
			//		parent = GetParent(hDlg);
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT1), AddEdgeDlgResults::buffer, 101);
			using namespace AddEdgeDlgResults;
			if (IsDlgButtonChecked(hDlg, IDC_RADIO3) == BST_CHECKED) {
				int t = atoi(buffer);
				string secStr = std::to_string(t);
				if (0 != strcmp(buffer, secStr.c_str())) {
					WRONG_MESS
						break;
				}
				number = t;
				isItNumber = true;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED) {
				char digits[] = "1234567890";
				bool haveDigit = false;
				for (int i = 0; buffer[i] != '\0'; ++i) {
					if (NULL != strchr(digits, buffer[i])) {
						haveDigit = true;
					}
				}
				if (!haveDigit) {
					WRONG_MESS
						break;
				}
				char chars[] = "1234567890,. ";
				for (int i = 0; buffer[i] != '\0'; ++i) {
					if (NULL == strchr(chars, buffer[i])) {
						WRONG_MESS
							return 1;
					}
				}
				AddEdgeDlgResults::isItNumber = true;
				number = atof(buffer);
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED) {
				AddEdgeDlgResults::isItNumber = false;
			}
			EndDialog(hDlg, true);
			break;
		case IDCANCEL:
			EndDialog(hDlg, false);
			break;
		}
		break;
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_SETCHECK, BST_CHECKED, 0);
		SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		param = WindowParameters::getInstance();
		break;
	case WM_CLOSE:
		EndDialog(hDlg, false);
		break;
	}
	return FALSE;
}

bool isAddNewEdgeModeBegined;
bool ianuemb;
int childNum = 0;

#include <fstream>
#include "UndirectedEdge.h"
HWND hwndEditWindow;

int CALLBACK editWindowProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rt;
	HMENU hMenu;
	static WindowParameters* param;
	static FileManager* manager;
	static HWND selectedEdge = nullptr;
	static HWND hButtDelEd, hButAddVer;;
	static HWND hCheck1, hCheck2;

	switch (message)
	{
	case WM_PAINT: {			//Перемалювати вікно
		hdc = BeginPaint(hDlg, &ps); 	//Почати графічний вивід	
		GetClientRect(hDlg, &rt); 		//Область вікна для малювання
		char buffer[100];
		int n = map.size();
		TextOut(hdc, 10, 10, buffer, wsprintf(buffer, "Кількість вершин: %3d", n));
		if (0 == n) {
			DrawText(hdc, "Порожній граф", -1, &rt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}

		SetViewportOrgEx(hdc, param->origin.x, param->origin.y, NULL);
		for (EdgeInEdit e : edges) {
			e.edEl->draw(hdc, e.static1, e.static2, e.color);
		}
		SetViewportOrgEx(hdc, 0, 0, NULL);
		
		EndPaint(hDlg, &ps); 		//Закінчити графічний вивід	
	}	break;
	case WM_CTLCOLORSTATIC :
		if ((HWND)lParam != hCheck1 && (HWND)lParam != hCheck2) {
			SetTextColor((HDC)wParam, param->colors.colEdgeWeight);
		}
		return (int) GetSysColorBrush(COLOR_WINDOW);
	case WM_COMMAND: {
		UINT is4Checked;
		UINT is5Checked;
		WORD low = LOWORD(wParam);
		if (4 == low || 5 == low) {
			is4Checked = IsDlgButtonChecked(hDlg, 4);
			is5Checked = IsDlgButtonChecked(hDlg, 5);
		}
		switch (low) {
		case IDR_CLOSE_EDIT:
			PostMessage(hDlg, WM_CLOSE, wParam, lParam);
			break;
		case 0: {
			char buf[] = "X";
			int number = map.size();
			buf[0] = 65 + number;
			VertexInEdit v;
			HWND h = CreateWindow(
				"vertexWindow", buf,
				WS_CHILD | WS_VISIBLE | WS_EX_TRANSPARENT,
				10, 100, 50, 50,
				hDlg, (HMENU)childNum, hInst, NULL);
			++childNum;
			v.ver = Vertex(number, { 35, 125 });
			//	vertices.push_back(v);
			map[h] = v;
			vertWins.push_back(h);
			arrangeVertices(vertWins, map);
			InvalidateRect(hDlg, NULL, true);
			ShouldBeSaved::isChanges = true;
		}	break;
		case 1:
			if (nullptr == selectedEdge) {
				MessageBox(hDlg, "Ви не вибрали ребро/дугу.\n\
Виділити можна кліком миші по вазі ребра/дуги (по цифрі).",
"Помилка", MB_OK);
				break;
			}
			deleteEdge(findEdgeInEdit(selectedEdge));
			selectedEdge = nullptr;
			InvalidateRect(hDlg, NULL, true);
			ShouldBeSaved::isChanges = true;
			break;
		case DELETE_VERTEX_BUTTON:
			if (nullptr == selectedVertex) {
				MessageBox(hDlg, "Ви не вибрали вершину.\n\
Виділити можна натиснувши ліву клавішу миші на вершині.",
"Помилка", MB_OK);
				break;
			}
			SendMessage(selectedVertex, WM_KEYUP, VK_DELETE, 0);
			break;
		case 3:
			arrangeVertices(vertWins, map);
			InvalidateRect(hDlg, NULL, true);
			break;
		case 4: {
			if (is4Checked && is5Checked) {
				CheckDlgButton(hDlg, 5, BST_UNCHECKED);
			}
			if (BST_CHECKED == is4Checked && !isAddNewEdgeModeBegined) {
				if (vertWins.size() == 0) {
					MessageBox(hDlg, "Граф не має жодної вершини. \n\
Добавте спочатку вершини.", "Не можна добавити", MB_OK);
					break;
				}
				else if (vertWins.size() == 1) {
					MessageBox(hDlg, "Граф має тільки одну вершину. \n\
Добавте другу вершину і тоді можна буде добавити ребро/дугу.", "Не можна добавити", MB_OK);
					break;
				}
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG5), hDlg, (DLGPROC)addEdgeModeDlg);
				isAddNewEdgeModeBegined = true;
				ianuemb = false;
				selectedVertex = nullptr;
			}
			else if (BST_UNCHECKED == is4Checked && isAddNewEdgeModeBegined) {
				isAddNewEdgeModeBegined = false;
		//		ianuemb = true;
			}
		}	break;
		case 5: {
			if (is4Checked && is5Checked) {
				CheckDlgButton(hDlg, 4, BST_UNCHECKED);
			}
			if (BST_CHECKED == is5Checked && !ianuemb) {
				if (vertWins.size() == 0) {
					MessageBox(hDlg, "Граф не має жодної вершини. \n\
Добавте спочатку вершини.", "Не можна добавити", MB_OK);
					break;
				}
				else if (vertWins.size() == 1) {
					MessageBox(hDlg, "Граф має тільки одну вершину. \n\
Добавте другу вершину і тоді можна буде добавити ребро/дугу.", "Не можна добавити", MB_OK);
					break;
				}
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG5), hDlg, (DLGPROC)addEdgeModeDlg);
				ianuemb = true;
				isAddNewEdgeModeBegined = false;
				selectedVertex = nullptr;
			}
			else if (BST_UNCHECKED == is5Checked && ianuemb) {
				ianuemb = false;
	//			isAddNewEdgeModeBegined = true;
			}
		}	break;

		case ID_MENU_BUT_HELP:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG4), hDlg, (DLGPROC)helpDlg);
			break;
		}

		if (STN_DBLCLK == HIWORD(wParam)) {
			if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG6),
				hDlg, (DLGPROC)addNewEdgeDlg)) {
				setTextAndResize((HWND)lParam,
					AddEdgeDlgResults::buffer);
				ShouldBeSaved::isChanges = true;
			}
		}
		else if (STN_CLICKED == HIWORD(wParam)) {
			if (selectedEdge != nullptr) {
				findEdgeInEdit(selectedEdge)->color = 0;
			}
			HWND h = (HWND)lParam;
			list<EdgeInEdit>::iterator it = edges.begin();
			while (it != edges.end()) {
				if (it->static1 == h || it->static2 == h) {
					it->color = param->colors.colSelectedEdge; //RGB(0, 255, 0);
					selectedEdge = it->static1;
					break;
				}
				it++;
			}
			InvalidateRect(hDlg, NULL, true);
		}
	}	break;
	case WM_KEYDOWN:
//		MessageBox(hDlg, "KEYDOWN", "KEYDOWN", MB_OK);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP: {
		selectedVertex = nullptr;
		SetFocus(hButAddVer);
		if (selectedEdge != nullptr) {
			findEdgeInEdit(selectedEdge)->color = 0;
		}
		selectedEdge = nullptr;
		InvalidateRect(hDlg, NULL, TRUE);
	}	break;
	case WM_CTLCOLORDLG:
		return (LONG) param->colors.bkrg;
	case WM_INITDIALOG: { //ініціалізація функціоналу керування діалоговим вікном
		param = WindowParameters::getInstance();
		RECT& par = param->whereWas;
		int width = par.right - par.left;
		int height = par.bottom - par.top;
		MoveWindow(hDlg,
			par.left, 			// положення по Х	
			par.top,			// положення по Y	
			width, 			// розмір по Х
			height, 			// розмір по Y
			true);
		hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU2));
		SetMenu(hDlg, hMenu);
		param->updateParameters(width - 16, height - 60);

		hButAddVer = CreateWindow("button", "Добавити вершину", 
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 30, 150, 40, hDlg, (HMENU)0, hInst, NULL);
		childNum = 1;
		standardBtnProc = (WNDPROC)SetWindowLong(hButAddVer, GWL_WNDPROC, (LONG) &fakeDlgProc);
		
		hButtDelEd = CreateWindow("button", "Видалити ребро/дугу",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			param->getWidth() - 160, 110, 150, 40, hDlg, (HMENU)1, hInst, NULL);
		++childNum;

		CreateWindow("button", "Видалити вершину",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 75, 140, 40, hDlg, (HMENU) DELETE_VERTEX_BUTTON, hInst, NULL);
		++childNum;

		CreateWindow("button", "Рівномірно розмісти вершини",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			param->getWidth() - 220, param->getHeight() - 50, 210, 40, 
			hDlg, (HMENU)3, hInst, NULL);
		++childNum;

		hCheck1 = CreateWindow("button", "Режим вставки дуг",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			param->getWidth() - 160, 60, 150, 40, hDlg, (HMENU)4, hInst, NULL);
		++childNum;

		hCheck2 = CreateWindow("button", "Режим вставки ребер",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			param->getWidth() - 175, 10, 165, 40, hDlg, (HMENU)5, hInst, NULL);
		++childNum;


		manager = FileManager::getInstance();
//		vector<vector<int>> coMa = manager->getMatr();
		vector<Vertex> *vertices;
		vector<EdgeElement*> *edVec;
		manager->getGraphElements(vertices, edVec);

		vertWins.reserve(vertices->size());
		for (Vertex ver : *vertices) {
			VertexInEdit v;
			v.ver = ver;
			HWND h = CreateWindow("vertexWindow", v.ver.name.c_str(),
				WS_CHILD | WS_VISIBLE | WS_EX_TRANSPARENT,
				10, 10, 50, 50,
				hDlg, (HMENU)childNum, hInst, NULL);
			++childNum;
			map[h] = v;
			vertWins.push_back(h);
		}
		arrangeVertices(vertWins, map);

		for (EdgeElement* e : *edVec) {
			HWND h = CreateWindow("static", e->getWeight().c_str(),
				WS_EX_TRANSPARENT | WS_CHILD | WS_VISIBLE | SS_NOTIFY,
				10, 100, 50, 25,
				hDlg, (HMENU)childNum, hInst, NULL);
			++childNum;
			SendMessage(h, WM_SETFONT, WPARAM(param->hFont), TRUE);

			HWND from = vertWins[e->getAver()->getNumber()];
			e->setAver(&map[from].ver);
			HWND to;
			EdgeForPair* efp = dynamic_cast<EdgeForPair*>(e);
			if (efp) {
				to = vertWins[efp->getBver()->getNumber()];
				efp->setBver(&map[to].ver);
			}
			else {
				to = nullptr;
			}
			TwoArcs* ch = dynamic_cast<TwoArcs*>(e);
			if (ch) {	// выполняем проверку ch на нулевой указатель
				HWND h2 = CreateWindow("static", ch->getSecondWeight().c_str(),
					WS_EX_TRANSPARENT | WS_CHILD | WS_VISIBLE | SS_NOTIFY,
					10, 100, 50, 25,
					hDlg, (HMENU)childNum, hInst, NULL);
				++childNum;
				SendMessage(h2, WM_SETFONT, WPARAM(param->hFont), TRUE);

				EdgeInEdit dualEdge = EdgeInEdit(e, h, h2, from, to);
				edges.push_back(dualEdge);
				map[dualEdge.hVerA].links.push_back(h);
				if (nullptr == dualEdge.hVerB) {
					MessageBox(hDlg, "nullptr == dualEdge.hVerB", "Errpr", MB_OK);
				}
				else {
					map[dualEdge.hVerB].links.push_back(h);
				}
			}
			else {
				EdgeInEdit eeee = EdgeInEdit(e, h, nullptr, from, to);
				edges.push_back(eeee);
				map[eeee.hVerA].links.push_back(h);
				if (nullptr == eeee.hVerB) {
					MessageBox(hDlg, "nullptr == dualEdge.hVerB", "Errpr", MB_OK);
				}
				else {
					map[eeee.hVerB].links.push_back(h);
				}
			}
		}
		
		selectedVertex = nullptr;
		if (vertWins.size() > 0) {
			SetFocus(vertWins.front());
		}
		selectedEdge = nullptr;
		hwndEditWindow = hDlg;
		ShouldBeSaved::isChanges = false;
		manager->edges.clear();
	}	return TRUE;
	case WM_CLOSE: {
		using namespace ShouldBeSaved;
		if (isChanges) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG8),
				hwndEditWindow, (DLGPROC)shouldChangesBeSavedDlg);
		}
		if (1 == result && isChanges) {
			//	temporarily
			std::string s = "Matrices\\" + manager->getCurrentFileName();
			int t = s.size() - 3;
			s[t] = 'g'; s[++t] = 'p'; s[++t] = 'h';
			std::ofstream fout(s);

			const int m = MAX_WEIGHT_LENGTH;
			char buffer[m];

			fout << vertWins.size() << "\n\n";
			for (HWND h : vertWins) {
				GetWindowText(h, buffer, m);
				fout << buffer << "\n";
			}

			fout << "\n" << edges.size() << "\n\n";
			for (EdgeInEdit e : edges) {
				fout << getIndexByValue(e.hVerA) << " "
					<< getIndexByValue(e.hVerB) << " ";
				int type = 0;
				SingleArc* s = dynamic_cast<SingleArc*>(e.edEl);
				if (s) {
					type = 1;
				}
				else {
					TwoArcs* t = dynamic_cast<TwoArcs*>(e.edEl);
					if (t) {
						type = 2;
					}
					else {
						typedef UndirectedEdge un;
						un* u = dynamic_cast<un*>(e.edEl);
						if (u) {
							type = 3;
						}
					}
				}
				fout << type << " ";
				GetWindowText(e.static1, buffer, m);
				fout << buffer << "\n";

				if (2 == type) {
					GetWindowText(e.static2, buffer, m);
					fout << buffer << "\n";
				}


				delete e.edEl;
			}
			fout.close();
		}
		if (2 != result) {
			vertWins.clear();
			map.clear();
			edges.clear();
			isAddNewEdgeModeBegined = false;
			manager->update();
			EndDialog(hDlg, LOWORD(wParam));
		}
	}	break;
	}
	return FALSE;
}

LRESULT CALLBACK vertexWndProc(HWND, UINT, WPARAM , LPARAM );
ATOM registerVertexWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//стиль вікна
	wcex.lpfnWndProc = (WNDPROC) vertexWndProc; 		//віконна процедура
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//дескриптор програми
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 	//визначення іконки
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//визначення курсору
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); //установка фону
	wcex.lpszMenuName = NULL; 				//визначення меню
	wcex.lpszClassName = "vertexWindow"; 		//ім’я класу
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//реєстрація класу вікна
}

int CALLBACK changeVertexNameDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case IDOK:
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT1), AddEdgeDlgResults::buffer, 101);
			EndDialog(hDlg, true);
			break;
		case IDCANCEL:
			EndDialog(hDlg, false);
			break;
		}
		break;
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, false);
		break;
	}
	return FALSE;
}

LRESULT CALLBACK vertexWndProc(HWND hWnd,
	UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	HPEN hPen;
	static HWND mouseInside = nullptr;
	static WindowParameters* param;

	switch (message)
	{
	case WM_PAINT: { 				//Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); 	//Почати графічний вивід	
		GetClientRect(hWnd, &rt); 		//Область вікна для малювання
		
		SetTextColor(hdc, param->colors.colVertexName);
		SetBkMode(hdc, TRANSPARENT);
				
		const int n = 150;
		char buffer[n];
		GetWindowText(hWnd, buffer, n);
		if ('\0' == buffer[1]) {
			int radius = (rt.right - rt.left) / 2;
			POINT center = { radius, radius };
			int halfSmallerSquare = radius / sqrt(2);
			RECT square = {
				center.x - halfSmallerSquare, center.y - halfSmallerSquare,
				center.x + halfSmallerSquare, center.y + halfSmallerSquare
			};

			Logic::drawBigLetter(hdc, square, buffer[0]);
		}
		else {
			DrawText(hdc, buffer, -1, &rt, DT_SINGLELINE | DT_CENTER
				| DT_VCENTER | DT_MODIFYSTRING | DT_END_ELLIPSIS);
		}
		
		if (hWnd == mouseInside || (hWnd == selectedVertex)) {
			hPen = CreatePen(PS_SOLID, 6, RGB(0, 255, 0));
		}
		else if (isAddNewEdgeModeBegined || ianuemb) {
			hPen = CreatePen(PS_SOLID, 6, RGB(255, 0, 0));
		} else {
			hPen = CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
		}
		SelectObject(hdc, hPen);
		Arc(hdc, 0, 0, rt.right, rt.right, 0,0,0,0);
		DeleteObject(hPen);
		EndPaint(hWnd, &ps); 		//Закінчити графічний вивід	
	}	break;
	case WM_MOUSEMOVE: 
		if (mouseInside != hWnd)
		{	
			mouseInside = hWnd;
			
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE;

			TrackMouseEvent(&tme);

			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	case WM_MOUSELEAVE:
		mouseInside = nullptr;
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_LBUTTONDOWN: {
		if ((isAddNewEdgeModeBegined || ianuemb) && nullptr != selectedVertex) {
			HWND &from = selectedVertex;
			HWND &to = hWnd;
			if (from == to) {
				selectVertex(nullptr);
				break;
			}
			VertexInEdit* fromE = &map[from];
			for (HWND i : fromE->links) {
				L_T::iterator ed = findEdgeInEdit(i);
				if (ed->hVerA == from && ed->hVerB == to) {
					MessageBox(hWnd, "Для цих двох обраних вершин \
вже існує ребро у такому самому напрямку. \n\
Якщо ви бажаєте замінити дугу, то спочатку видаліть попередню.", 
										"Спочатку видаліть", MB_OK);
					selectVertex(nullptr);
					return 1;
				}
			}
			if (!DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG6),
							hWnd, (DLGPROC)addNewEdgeDlg) ) {
				break;
			}
			VertexInEdit* fromV = &map[from];
			VertexInEdit* toV = &map[to];
			EdgeElement* e = nullptr;
			if (isAddNewEdgeModeBegined) {
				e = new SingleArc(&fromV->ver, &toV->ver, AddEdgeDlgResults::buffer);
			}
			else if (ianuemb) {
				e = new UndirectedEdge(&fromV->ver, &toV->ver, AddEdgeDlgResults::buffer);
			}
			HWND st = CreateWindow("static", AddEdgeDlgResults::buffer,
				WS_EX_TRANSPARENT | WS_CHILD | WS_VISIBLE | SS_NOTIFY,
				10, 100, 50, 25,
				hwndEditWindow, (HMENU)childNum, hInst, NULL);
			++childNum;
			SendMessage(st, WM_SETFONT, WPARAM(param->hFont), TRUE);
			edges.push_back(EdgeInEdit(e, st, nullptr, from, to));
			fromV->links.push_back(st);
			toV->links.push_back(st);
			selectVertex(nullptr);
			InvalidateRect(hwndEditWindow, NULL, true);
			ShouldBeSaved::isChanges = true;
		}
		else {
			selectVertex(hWnd);
		}
	}	break;
	case WM_KEYUP:
		switch (wParam) {
		case VK_RIGHT:
			if (vertWins.back() == selectedVertex) {
				selectVertex(vertWins.front());
			}
			else {
				vector<HWND>::iterator it = std::find(vertWins.begin(), 
										vertWins.end(), selectedVertex);
				it++;
				selectVertex(*it);
			}
			break;
		case VK_LEFT:
			if (vertWins.front() == selectedVertex) {
				selectVertex(vertWins.back());
			}
			else {
				vector<HWND>::iterator it = std::find(vertWins.begin(),
					vertWins.end(), selectedVertex);
				it--;
				selectVertex(*it);
			}
			break;
		case VK_DELETE:
			if (hWnd == selectedVertex &&
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3),
								hWnd, (DLGPROC)deleteVertexDlg)) {
				list<HWND> l = map[hWnd].links;
				for (HWND h : l) {
					deleteEdge(findEdgeInEdit(h));
				}
				map.erase(hWnd);
				vector<HWND>::iterator it
		= std::find(vertWins.begin(), vertWins.end(), hWnd);
				if (vertWins.size() == 1) {
					selectedVertex = nullptr;
				} else if (vertWins.begin() == it) {
					selectVertex(vertWins.back());
				}
				else {
					selectVertex(*(it - 1));
				}
				vertWins.erase(it);
				DestroyWindow(hWnd);
				InvalidateRect(GetParent(hWnd), NULL, true);
				ShouldBeSaved::isChanges = true;
			}
			break;
		}
		break;
	case WM_RBUTTONUP:
		if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG7), hWnd, (DLGPROC)changeVertexNameDlg)) {
			SetWindowText(hWnd, AddEdgeDlgResults::buffer);
			InvalidateRect(hWnd, NULL, true);
			ShouldBeSaved::isChanges = true;
		}
		break;
	case WM_CREATE: {
		param = WindowParameters::getInstance();
		if (!param->colors.isVertexBkrgChanged) {
			SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)param->colors.vertexBkrg);
		}
	}
	default:
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}