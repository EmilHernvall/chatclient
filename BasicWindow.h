
#pragma once

#include "stdafx.h"

#define WM_BASICWINDOW_CREATE	(WM_USER+0)

class BasicWindow
{
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	BasicWindow(HINSTANCE hInst, INT nCmdShow, LPCWSTR szWindowClass, LPCWSTR szWindowTitle, 
		INT nIcon, INT nSmallIcon, INT nMenu);
	BOOL Create();

	virtual LRESULT CALLBACK HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

protected:
	HINSTANCE m_hInst;
	HWND m_hWnd;
	LPCWSTR m_szWindowClass, m_szWindowTitle;
	INT m_nIcon, m_nSmallIcon, m_nMenu;
	INT m_nCmdShow;

	ATOM RegisterWindow();
};

struct BasicWindowClass : WNDCLASSEX
{
	BasicWindow *bwWindow;
};
