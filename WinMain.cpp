
#include <windows.h> // ���������� �������� � ��������� API
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

// �������� ����:
HINSTANCE hInst; 	//���������� ��������	
LPCTSTR szTitle = "³���������� ������";

// ��������� ���� �������
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// ������� �������� 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 	int nCmdShow)
{
	MSG msg;
	// ��������� ����� ���� 
	MyRegisterClass(hInstance);
//	registerEditWindowClass(hInstance);
	registerVertexWindowClass(hInstance);
	// ��������� ���� ��������
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// ���� ������� ����������
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
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//����� ����
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//������ ���������
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//���������� ��������
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 	//���������� ������
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//���������� �������
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); //��������� ����
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); 				//���������� ����
	wcex.lpszClassName = "MainWindow"; 		//��� �����
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//��������� ����� ����
}

// FUNCTION: InitInstance (HANDLE, int)
// ������� ���� �������� � ������ ���������� �������� � ����� hInst
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int x = (GetSystemMetrics(SM_CXSCREEN) - WindowParameters::initialWidth) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - WindowParameters::initialHeight) / 2;
	HWND hWnd;
	hInst = hInstance; //������ ���������� ������� � ����� hInst
	hWnd = CreateWindow(
		"MainWindow", 	// ��� ����� ����
		szTitle, 				// ����� ��������
		WS_OVERLAPPEDWINDOW,			// ����� ����
		x, 			// ��������� �� �	
		y,			// ��������� �� Y	
		WindowParameters::initialWidth + 16, 			// ����� �� �
		WindowParameters::initialHeight + 39, 			// ����� �� Y
		NULL, 					// ���������� ������������ ����	
		NULL, 					// ���������� ���� ����
		hInstance, 				// ���������� ��������
		NULL); 				// ��������� ���������.

	if (!hWnd) 	//���� ���� �� ���������, ������� ������� FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//�������� ����
	UpdateWindow(hWnd); 				//������� ����
	return TRUE;
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// ³����� ���������. ������ � �������� �� �������, �� ��������� � �������
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
	case WM_PAINT: {			//������������ ����
		hdc = BeginPaint(hWnd, &ps); 	//������ ��������� ����	
		GetClientRect(hWnd, &rt); 		//������� ���� ��� ���������
	
		SetBkMode(hdc, TRANSPARENT);

		vector<Vertex> *vertices;
		vector<EdgeElement*> *edges;
		manager->getGraphElements(vertices, edges);

		if (0 == vertices->size()) {
			RECT rt = { 0, 0, window->getWidth(), window->getHeight() };
			DrawText(hdc, "������� ����", -1, &rt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
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

		EndPaint(hWnd, &ps); 		//�������� ��������� ����	
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
		"����� ���� �� ��� ���������. ���� � ����� ��'�� ��� ����.",
					"�������", MB_OK);
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
			wsprintf(name, "���� ��� ���������� � ���� %s � ����� BITMAPS.", c.c_str());
			MessageBox(hWnd,  name, "���������!", MB_OK);
		}	break;
		}
		break;
	case WM_CREATE: {//����������� ��������� ��� �������� ����
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
	case WM_DESTROY: 				//���������� ������
		PostQuitMessage(0);
		break;
	default:
		//������� ����������, �� �� �������� ������������
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
