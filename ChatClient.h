#pragma once

#include "resource.h"

#define ID_BUFFER	1
#define ID_INPUT	2

#define MAX_LOADSTRING 100

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	InputWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	BufferWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Connect(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void AppendToBuffer(LPTSTR szData);
void ParseCommand(LPTSTR szCommand);

