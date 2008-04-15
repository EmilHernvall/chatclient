#include "stdafx.h"
#include "Dialog.h"

INT_PTR CALLBACK Dialog::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	Dialog *dlgThis;

	if (message == WM_INITDIALOG) 
	{
		dlgThis = (Dialog*)lParam;
		SetWindowLong(hDlg, GWL_USERDATA, (LONG)dlgThis);
	} else {
		dlgThis = (Dialog*)GetWindowLong(hDlg, GWL_USERDATA);
	}

	if (dlgThis != NULL) {
		return (INT_PTR)dlgThis->HandleMessage(hDlg, message, wParam, lParam);
	} else {
		return (INT_PTR)FALSE;
	}
}

Dialog::Dialog(HINSTANCE hInst, INT nResource, HWND hwndParent)
{
	m_hInst = hInst;
	m_nResource = nResource;
	m_hwndParent = hwndParent;
}

VOID Dialog::Display()
{
	DialogBoxParam(m_hInst, MAKEINTRESOURCE(m_nResource), m_hwndParent, Dialog::WndProc, (LPARAM)this);
}

