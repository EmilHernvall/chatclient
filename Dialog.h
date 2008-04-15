
#pragma once

class Dialog
{
public:
	static INT_PTR CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	Dialog(HINSTANCE, INT, HWND);
	VOID Display();

	virtual INT_PTR CALLBACK HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) = 0;

private:
	HINSTANCE m_hInst;
	INT m_nResource;
	HWND m_hwndParent;
	HWND m_hwnd;

};
