#pragma once

int CALLBACK deleteVertexDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//цикл обробки натискання елементів на формі діалогового вікна
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case ID_YES:
			EndDialog(hDlg, true);
			break;
		case IDC_NO:
			EndDialog(hDlg, false);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, false);
		break;
	}
	return FALSE;
}

int CALLBACK helpDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case IDOK:
			EndDialog(hDlg, true);
			break;
		}
		break;
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg, IDC_SPV4), "	Ви можете виділяти вершини, \
натиснувши лівою клавшею миші по вершині, однак це можна зробити і за допомогою \
клавіатури. Коли жодна з вершин не обрана, ви можете виділити будь-яку вершину \
від першої до дев'ятої натиснувши відповідно 1...9. Якщо будь-яка вершина уже виділена, \
ви не можете скорстатись цією функцією, проте ви можете \"переструбнути\" на сусідню вершину, \
натиснувши на клавіатурі стрілки вліво або вправо.");
		break;
	case WM_CLOSE:
		EndDialog(hDlg, true);
		break;
	}

	return FALSE;
}

int CALLBACK addEdgeModeDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool check = false;
	switch (message)
	{
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case IDOK:
			EndDialog(hDlg, true);
			break;
		case IDC_CHECK1: {
			UINT i = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			if (i == BST_CHECKED) {
				check = true;
			}
			else if (i == BST_UNCHECKED) {
				check = false;
			}
		}	break;
		}
		break;
	case WM_INITDIALOG:
		if (check) {
			EndDialog(hDlg, true);
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, true);
		break;
	}

	return FALSE;
}

namespace ShouldBeSaved {
	char result; bool isChanges;
}
int CALLBACK shouldChangesBeSavedDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	using namespace ShouldBeSaved;
	switch (message)
	{
		//цикл обробки натискання елементів на формі діалогового вікна
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case ID_YES:
			result = 1;
			EndDialog(hDlg, false);
			break;
		case ID_NO:
			result = 0;
			EndDialog(hDlg, false);
			break;
		case IDCANCEL:
			result = 2;
			EndDialog(hDlg, false);
			break;
		}
		break;
	case WM_CLOSE:
		result = 2;
		EndDialog(hDlg, false);
		break;
	}
	return FALSE;
}