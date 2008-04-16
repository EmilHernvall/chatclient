#include "stdafx.h"
#include "SubclassedWindow.h"

SubclassedWindow::SubclassedWindow(LPWSTR szClass, INT nStyle, 
								   HWND hwndParent, INT nId, HINSTANCE hInstance)
{

	m_hwnd = CreateWindow(szClass, NULL, nStyle, 0, 0, 0, 0, 
		hwndParent, (HMENU) nId, hInstance, NULL);

	m_prevWndProc = (WNDPROC)SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)SubclassedWindow::WndProc);
	SetWindowLong(m_hwnd, GWL_USERDATA, (LONG)this);
}

LRESULT CALLBACK SubclassedWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SubclassedWindow *scWindow = (SubclassedWindow*)GetWindowLong(hWnd, GWL_USERDATA);

	if (scWindow != NULL) {
		return scWindow->HandleMessage(hWnd, message, wParam, lParam);
	} else {
		return (LRESULT)0;
	}
}
