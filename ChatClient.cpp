// ChatClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ChatClient.h"
#include "Net.h"
#include "String.h"

// Global Variables:

// The title bar text
TCHAR szTitle[MAX_LOADSTRING];

// the main window class name
TCHAR szWindowClass[MAX_LOADSTRING];

// window handles for the buffer edit and the input edit
HWND hwndBuffer, hwndInput;

// current instance
HINSTANCE hInst;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	 
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		MessageBox(NULL, TEXT("Your system doesn't support the required Winsock Version!"), TEXT("Winsock Error!"), MB_ICONEXCLAMATION);
		return 1;
	}
	 
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		MessageBox(NULL, TEXT("Your system doesn't support the required Winsock Version!"), TEXT("Winsock Error!"), MB_ICONEXCLAMATION);
		return 1; 
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WSACleanup();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CHATCLIENT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_CREATE
//  WM_COMMAND	- process the application menu
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
	WNDPROC inputProc, bufferProc;
	HFONT font;

	switch (message)
	{
	case WM_CREATE:
		hwndBuffer = CreateWindow(TEXT ("edit"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
			WS_BORDER | ES_LEFT | ES_MULTILINE |
			ES_AUTOVSCROLL,
			0, 0, 0, 0, hWnd, (HMENU) ID_BUFFER,
			((LPCREATESTRUCT)lParam) -> hInstance, NULL) ;

		hwndInput = CreateWindow(TEXT ("edit"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			0, 0, 0, 0, hWnd, (HMENU) ID_INPUT,
			((LPCREATESTRUCT)lParam) -> hInstance, NULL) ;

		//SendMessage(hwndBuffer, (UINT)EM_SETREADONLY, TRUE, 0);

		bufferProc = (WNDPROC)SetWindowLong(hwndBuffer, GWL_WNDPROC, (LONG)BufferWndProc);
		SetWindowLong(hwndBuffer, GWL_USERDATA, (LONG)bufferProc);

		inputProc = (WNDPROC)SetWindowLong(hwndInput, GWL_WNDPROC, (LONG)InputWndProc);
		SetWindowLong(hwndInput, GWL_USERDATA, (LONG)inputProc);

		font = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH, TEXT("Courier New"));

		SendMessage(hwndBuffer, WM_SETFONT, (WPARAM)font, (LPARAM)FALSE);

		break;

	case WM_SETFOCUS:
		SetFocus(hwndInput);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_CONNECT:
			DialogBox(hInst, MAKEINTRESOURCE(IDC_CONNECT_DIALOG), hWnd, Connect);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	/*case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;*/
    case WM_SIZE: 
        // Make the edit control the size of the window's client area. 

        MoveWindow(hwndBuffer, 
                   5, 5,                  // starting x- and y-coordinates 
                   LOWORD(lParam) - 10,        // width of client area 
                   HIWORD(lParam) - 35,        // height of client area 
                   TRUE);                 // repaint window 

        MoveWindow(hwndInput, 
                   5, 
				   HIWORD(lParam) - 25,        // starting x- and y-coordinates 
                   LOWORD(lParam) - 10,        // width of client area 
                   20,        // height of client area 
                   TRUE);                 // repaint window 
        return 0; 
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK BufferWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC bufferProc;

	switch (message)
	{
	case WM_CHAR:
		break;
	default:
		bufferProc = (WNDPROC)GetWindowLong(hWnd, GWL_USERDATA);
		return CallWindowProc (bufferProc, hWnd, message, wParam, lParam) ;
	}
	return 0;
}

// Window proc for the input edit, subclassed to capture enter strokes.
LRESULT CALLBACK InputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPTSTR szBuffer;
	int inputLen;
	WNDPROC inputProc;

	switch (message)
	{
	case WM_CHAR:
		if (wParam == 0x0D)
		{
			if (getConnectionSettings()->sock == NULL) {
				AppendToBuffer(TEXT("Not connected."));
				SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);
				break;
			}

			inputLen = Edit_GetTextLength(hWnd) + 1;
			szBuffer = (LPTSTR)malloc(sizeof(TCHAR) * (inputLen + 1));
			Edit_GetText(hWnd, (LPTSTR)szBuffer, inputLen);

			ParseCommand(szBuffer);

			free(szBuffer);

			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);

			//MessageBox(NULL, (LPTSTR)szBuffer, TEXT("hwndInput"), 0);
			break;
		}

	default:
		inputProc = (WNDPROC)GetWindowLong(hWnd, GWL_USERDATA);
		return CallWindowProc (inputProc, hWnd, message, wParam, lParam) ;
	}
	return 0;
}

// Utility method to append data to the end of the buffer edit.
void AppendToBuffer(LPTSTR szData)
{
	int bufferLen;

	// Move caret to the end of the buffer
	bufferLen = Edit_GetTextLength(hwndBuffer);
	SendMessage(hwndBuffer, EM_SETSEL, bufferLen, bufferLen);

	// Insert new data
	SendMessage(hwndBuffer, EM_REPLACESEL, 0, (LPARAM)szData);

	// Insert line break
	TCHAR* szNewLine = TEXT("\r\n");
	SendMessage(hwndBuffer, EM_REPLACESEL, 0, (LPARAM)szNewLine);
}

void ParseCommand(LPTSTR szBuffer)
{
	struct ConnectionSettings *settings;
	TCHAR szSendBuffer[2048], szDisplayBuffer[2048];

	settings = getConnectionSettings();

	if (*szBuffer == '/') {
		
	} else {
		_stprintf_s(szSendBuffer, 
			sizeof(szSendBuffer)/sizeof(TCHAR),
			TEXT("PRIVMSG %s :%s"), settings->szChannel, szBuffer);

		_stprintf_s(szDisplayBuffer, 
			sizeof(szDisplayBuffer)/sizeof(TCHAR),
			TEXT("<%s> %s"), settings->szNick, szBuffer);

		AppendToBuffer(szDisplayBuffer);
		SocketWrite(szSendBuffer);
	}
}