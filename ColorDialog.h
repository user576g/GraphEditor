#pragma once

int CALLBACK colorDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rt;
	static WindowParameters* par;

	switch (message)
	{
	case WM_PAINT: {			//Перемалювати вікно
		hdc = BeginPaint(hDlg, &ps); 	//Почати графічний вивід	
		GetClientRect(hDlg, &rt); 		//Область вікна для малювання
		
		EndPaint(hDlg, &ps); 		//Закінчити графічний вивід	
	}	break;
		//цикл обробки натискання елементів на формі діалогового вікна
	case WM_COMMAND:
		switch LOWORD(wParam) {
		case 1002: 
			par->colors.changeColor(0);
			SetClassLong(GetParent(hDlg), GCL_HBRBACKGROUND, (LONG)par->colors.bkrg);
			break;
		case 1003:
		case 1004:
		case 1005:
		case 1006:
		case 1007:
		case 1008: {
			int i = LOWORD(wParam) - 1002;
			par->colors.changeColor(i);
		}	break;
		}
		break;
	case WM_CTLCOLORSTATIC: {
		HWND h = (HWND)lParam;
		int id = GetDlgCtrlID(h);
		switch (id) {
		case IDC_SPV0:
			return (LONG) par->colors.bkrg;
		case IDC_SPV1:
			return (LONG)par->colors.arc;
		case IDC_SPV3:
			return (LONG)par->colors.edge;
		case IDC_SPV4:
			return (LONG)par->colors.vertexName;
		case IDC_SPV5:
			return (LONG)par->colors.vertexBkrg;
		case IDC_SPV6:
			return (LONG)par->colors.selectedEdge;
		case IDC_SPV2:
			return (LONG)par->colors.edgeWeight;
		}
	}	break;
	case WM_INITDIALOG: { //ініціалізація функціоналу керування діалоговим вікном
		par = WindowParameters::getInstance();
		par->colors.hDialog = hDlg;
		par->colors.buf_color.hwndOwner = hDlg;
	}	return TRUE;
	case WM_CLOSE:
		par->colors.hDialog = nullptr;
		par->colors.buf_color.hwndOwner = NULL;
		EndDialog(hDlg, LOWORD(wParam));
		break;
	}
	return FALSE;
}
