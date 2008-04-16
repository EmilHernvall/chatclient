#include "stdafx.h"
#include "String.h"
#include "BasicWindow.h"

LRESULT BasicWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BasicWindow *bw;

	bw = (BasicWindow*)GetWindowLong(hWnd, 0);
	if (bw == NULL) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	} else {
		return bw->HandleMessage(hWnd, message, wParam, lParam);
	}
}

BasicWindow::BasicWindow(HINSTANCE hInst, INT nCmdShow, LPCWSTR szWindowClass, LPCWSTR szWindowTitle, 
		INT nIcon, INT nSmallIcon, INT nMenu)
{
	m_hInst = hInst;
	m_nCmdShow = nCmdShow;
	m_szWindowClass = szWindowClass;
	m_szWindowTitle = szWindowTitle;
	m_nIcon = nIcon;
	m_nSmallIcon = nSmallIcon;
	m_nMenu = nMenu;

	RegisterWindow();
}

ATOM BasicWindow::RegisterWindow()
{
	BasicWindowClass wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= BasicWindow::WndProc;
	wcex.cbClsExtra		= sizeof (BasicWindow*);
	wcex.cbWndExtra		= sizeof (BasicWindow*);
	wcex.hInstance		= m_hInst;
	wcex.hIcon			= LoadIcon(m_hInst, MAKEINTRESOURCE(m_nIcon));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(m_nMenu);
	wcex.lpszClassName	= m_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(m_nSmallIcon));

	wcex.bwWindow = this;

	ATOM res = RegisterClassEx(&wcex);
	if (res == 0) {
		ErrorExit(TEXT("RegisterClassEx"));
	}

	return res;
}

BOOL BasicWindow::Create()
{
	m_hWnd = CreateWindow(m_szWindowClass, m_szWindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, m_hInst, NULL);

	if (!m_hWnd) {
		ErrorExit(TEXT("CreateWindow"));
		return FALSE;
	}

	SetWindowLong(m_hWnd, 0, (LONG)this);

	SendMessage(m_hWnd, WM_BASICWINDOW_CREATE, 0, 0);

	ShowWindow(m_hWnd, m_nCmdShow);
	UpdateWindow(m_hWnd);

	return TRUE;
}
