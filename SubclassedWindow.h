#pragma once

class SubclassedWindow
{
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	SubclassedWindow(LPWSTR szClass, INT nStyle, HWND hwndParent, INT nId, HINSTANCE hInstance);

	HWND GetHwnd() { return m_hwnd; }

	virtual LRESULT CALLBACK HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

protected:
	HWND m_hwnd;
	WNDPROC m_prevWndProc;

};
