
#include <windows.h> // підключення бібліотеки з функціями API
#include <string>
using std::string;
#include "Logic.h"
#include <vector>
using std::vector;
#include <list>
using std::list;
#include "WindowParameters.h"
#include "FileManager.h"
#include "resource.h"
#include "EditWindow.h"
#include "ColorDialog.h"

// Глобальні змінні:
HINSTANCE hInst; 	//Дескриптор програми	
LPCTSTR szTitle = "Відображення графів";

// Попередній опис функцій
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// Основна програма 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 	int nCmdShow)
{
	MSG msg;
	// Реєстрація класу вікна 
	MyRegisterClass(hInstance);
//	registerEditWindowClass(hInstance);
	registerVertexWindowClass(hInstance);
	// Створення вікна програми
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// Цикл обробки повідомлень
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//стиль вікна
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//віконна процедура
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//дескриптор програми
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 	//визначення іконки
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//визначення курсору
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); //установка фону
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); 				//визначення меню
	wcex.lpszClassName = "MainWindow"; 		//ім’я класу
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//реєстрація класу вікна
}

// FUNCTION: InitInstance (HANDLE, int)
// Створює вікно програми і зберігає дескриптор програми в змінній hInst
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int x = (GetSystemMetrics(SM_CXSCREEN) - WindowParameters::initialWidth) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - WindowParameters::initialHeight) / 2;
	HWND hWnd;
	hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
	hWnd = CreateWindow(
		"MainWindow", 	// ім’я класу вікна
		szTitle, 				// назва програми
		WS_OVERLAPPEDWINDOW,			// стиль вікна
		x, 			// положення по Х	
		y,			// положення по Y	
		WindowParameters::initialWidth + 16, 			// розмір по Х
		WindowParameters::initialHeight + 39, 			// розмір по Y
		NULL, 					// дескриптор батьківського вікна	
		NULL, 					// дескриптор меню вікна
		hInstance, 				// дескриптор програми
		NULL); 				// параметри створення.

	if (!hWnd) 	//Якщо вікно не творилось, функція повертає FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//Показати вікно
	UpdateWindow(hWnd); 				//Оновити вікно
	return TRUE;
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомл, що приходять в додаток
LRESULT CALLBACK WndProc(HWND hWnd, 
					UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	static HWND hRst;
	static HWND hCombo, hEdit;
	static WindowParameters* window;
	static FileManager* manager;
	static vector<Vertex> *vertices;
	static vector<EdgeElement*> *edges;
	static bool isEditWindowOpened;

	switch (message)
	{
	case WM_SIZE: {
		if (!isEditWindowOpened) {
			window->updateParameters(LOWORD(lParam), HIWORD(lParam), hWnd);
			manager->relocateVertices();
		}
	}break;
	case WM_PAINT: {			//Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); 	//Почати графічний вивід	
		GetClientRect(hWnd, &rt); 		//Область вікна для малювання
	
		SetBkMode(hdc, TRANSPARENT);

		vector<Vertex> *vertices;
		vector<EdgeElement*> *edges;
		manager->getGraphElements(vertices, edges);

		if (0 == vertices->size()) {
			RECT rt = { 0, 0, window->getWidth(), window->getHeight() };
			DrawText(hdc, "Порожній граф", -1, &rt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			EndPaint(hWnd, &ps);
			break;
		}

		SetTextColor(hdc, window->colors.colVertexName);
		SetViewportOrgEx(hdc, window->origin.x, window->origin.y, NULL);
		for (Vertex v : *vertices) {
			v.draw(hdc);
		}

		SetTextColor(hdc, window->colors.colEdgeWeight);
		for (EdgeElement* e : *edges) {
			e->draw(hdc);
		}
		SetViewportOrgEx(hdc, 0, 0, NULL);

		EndPaint(hWnd, &ps); 		//Закінчити графічний вивід	
	}	break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case CREATE_NEW: {
			if (!DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG9),
				hWnd, (DLGPROC)changeVertexNameDlg)) {
				break;
			}
			string fileName(AddEdgeDlgResults::buffer);
			vector<string> names = manager->getNames();
			if (names.end() !=
				std::find(names.begin(), names.end(), fileName)) {
				MessageBox(hWnd,
		"Новий граф не був створений. Граф з таким ім'ям вже існує.",
					"Помилка", MB_OK);
			}
			else {
				manager->createEmpty(fileName, hCombo);
			}

		}	break;
		case 0:
			if (CBN_SELCHANGE == HIWORD(wParam)) {
				int i = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
				manager->changeFile(i, hCombo);
				GetClientRect(hWnd, &rt);
				InvalidateRect(hWnd, &rt, true);
			}
			break;
		case ID_40003:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)colorDialog);
			break;
		case EDIT_WINDOW:
			GetWindowRect(hWnd, &window->whereWas);
			ShowWindow(hWnd, SW_SHOWMINNOACTIVE);
			isEditWindowOpened = true;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)editWindowProc);
			isEditWindowOpened = false;
			ShowWindow(hWnd, SW_RESTORE);
			break;
		case BTN_SAVE_IN_BMP: {
			hdc = GetDC(hWnd);
			HDC hMem = CreateCompatibleDC(hdc);
			int w = window->getWidth();
			int h = window->getHeight();
			int s;
			s = w > h ? h : w;
			HBITMAP myBMP = CreateCompatibleBitmap(hdc, s, s);
			SelectObject(hMem, myBMP);
			BitBlt(hMem, 0, 0, s, s, hdc, (w - s) / 2, (h - s) / 2, SRCCOPY);
			std::string c = manager->getCurrentFileName();
			int l = c.size() - 3;
			c[l] = 'b'; c[++l] = 'm'; c[++l] = 'p';
			char name[150];
			char path[200];
			GetCurrentDirectory(200, path);
			string strPath(path);
			strPath.append("\\BITMAPS\\");
			CreateDirectory(strPath.c_str(), NULL);
			strPath.append(c);
			Logic::CreateBMPFile(strPath.c_str(), myBMP);
			ReleaseDC(hWnd, hdc);
			wsprintf(name, "Граф був збережений у файл %s У папці BITMAPS.", c.c_str());
			MessageBox(hWnd,  name, "Збережено!", MB_OK);
		}	break;
		}
		break;
	case WM_CREATE: {//Повідомлення приходить при створенні вікна
		CreateDirectory("Matrices", NULL);
		
		hCombo = CreateWindow("combobox", "filesCombo", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
			10, 10, 100, 300, hWnd, (HMENU) 0, hInst, NULL);
		
		manager = FileManager::getInstance();
		vector<string> names =  manager->getNames();
		for (string s : names) {
			SendMessage(hCombo, (UINT)CB_ADDSTRING, 0, (LPARAM) s.c_str());
		}

		window = WindowParameters::getInstance();
		manager = FileManager::getInstance();
		manager->changeFile(0, hCombo);
		manager->getGraphElements(vertices, edges);
		isEditWindowOpened = false;
	}	break;
	case WM_DESTROY: 				//Завершення роботи
		PostQuitMessage(0);
		break;
	default:
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
