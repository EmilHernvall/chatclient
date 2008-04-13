// ChatClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ChatClient.h"

struct ConnectionSettings
{
	LPTSTR szHost;
	DWORD dwPort;
	LPTSTR szNick;
	LPTSTR szUser;
	LPTSTR szChannel;
};

#define ID_BUFFER	1
#define ID_INPUT	2

#define MAX_LOADSTRING 100

// Global Variables:

// current instance
HINSTANCE hInst;

// The title bar text
TCHAR szTitle[MAX_LOADSTRING];

// the main window class name
TCHAR szWindowClass[MAX_LOADSTRING];

// window handles for the buffer edit and the input edit
HWND hwndBuffer, hwndInput;

// the default window proc for the input edit, which has been subclassed
// to capture enter strokes.
WNDPROC OldInputProc;

SOCKET sock;
struct ConnectionSettings *conn;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	InputWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Connect(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VOID NetworkThread (PVOID pvoid);

void AppendToBuffer(LPTSTR szData);
int socket_read(SOCKET sock, char** ret);
int socket_write(SOCKET sock, LPTSTR szData);
int explode(char*** out, char* in, unsigned char delim);
LPSTR wcToMb(LPWSTR szData);

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

	sock = NULL;

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
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
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

		SendMessage(hwndBuffer, (UINT)EM_SETREADONLY, TRUE, 0);

		OldInputProc = (WNDPROC)SetWindowLong(hwndInput, GWL_WNDPROC, (LONG)InputWndProc);

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
                   0, 0,                  // starting x- and y-coordinates 
                   LOWORD(lParam),        // width of client area 
                   HIWORD(lParam) - 20,        // height of client area 
                   TRUE);                 // repaint window 

        MoveWindow(hwndInput, 
                   0, 
				   HIWORD(lParam) - 20,        // starting x- and y-coordinates 
                   LOWORD(lParam),        // width of client area 
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

// Window proc for the input edit, subclassed to capture enter strokes.
LRESULT CALLBACK InputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPTSTR szBuffer;
	TCHAR szSendBuffer[2048], szDisplayBuffer[2048];
	int inputLen;

	switch (message)
	{
	case WM_CHAR:
		if (wParam == 0x0D)
		{
			if (sock == NULL) {
				AppendToBuffer(TEXT("Not connected."));
				SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);
				break;
			}

			inputLen = Edit_GetTextLength(hWnd) + 1;
			szBuffer = (LPTSTR)malloc(sizeof(TCHAR) * (inputLen + 1));
			Edit_GetText(hWnd, (LPTSTR)szBuffer, inputLen);

			if (*szBuffer == '/') {
				
			} else {
				_stprintf_s(szSendBuffer, 
					sizeof(szSendBuffer)/sizeof(TCHAR),
					TEXT("PRIVMSG %s :%s"), conn->szChannel, szBuffer);

				_stprintf_s(szDisplayBuffer, 
					sizeof(szDisplayBuffer)/sizeof(TCHAR),
					TEXT("<%s> %s"), conn->szNick, szBuffer);

				AppendToBuffer(szDisplayBuffer);
				socket_write(sock, szSendBuffer);
			}

			free(szBuffer);

			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);

			//MessageBox(NULL, (LPTSTR)szBuffer, TEXT("hwndInput"), 0);
			break;
		}

	default:
		return CallWindowProc (OldInputProc, hWnd, message, wParam, lParam) ;
	}
	return 0;
}

// Message handler for connect box.
INT_PTR CALLBACK Connect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	TCHAR szHost[200], szPort[200], szNick[200], szUser[200], szChannel[200], szInfo[1024];
	HWND hHost, hPort, hNick, hUser, hChannel;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		hHost = GetDlgItem(hDlg, IDC_HOST_EDIT);
		hPort = GetDlgItem(hDlg, IDC_PORT_EDIT);
		hNick = GetDlgItem(hDlg, IDC_NICK_EDIT);
		hUser = GetDlgItem(hDlg, IDC_USER_EDIT);
		hChannel = GetDlgItem(hDlg, IDC_CHANNEL_EDIT);

		SendMessage(hHost, EM_SETLIMITTEXT, 30, 0);
		SendMessage(hPort, EM_SETLIMITTEXT, 5, 0);
		SendMessage(hNick, EM_SETLIMITTEXT, 30, 0);
		SendMessage(hUser, EM_SETLIMITTEXT, 30, 0);
		SendMessage(hChannel, EM_SETLIMITTEXT, 30, 0);

		Edit_SetText(hHost, TEXT("beppe.c0la.se"));
		Edit_SetText(hPort, TEXT("6667"));
		Edit_SetText(hNick, TEXT("Aderyn2"));
		Edit_SetText(hUser, TEXT("Emil"));
		Edit_SetText(hChannel, TEXT("#floodffs!"));

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmEvent = LOWORD(wParam);
		switch (wmEvent)
		{
		case IDOK:
			hHost = GetDlgItem(hDlg, IDC_HOST_EDIT);
			hPort = GetDlgItem(hDlg, IDC_PORT_EDIT);
			hNick = GetDlgItem(hDlg, IDC_NICK_EDIT);
			hUser = GetDlgItem(hDlg, IDC_USER_EDIT);
			hChannel = GetDlgItem(hDlg, IDC_CHANNEL_EDIT);

			Edit_GetText(hHost, (LPTSTR)szHost, sizeof(szHost)/sizeof(TCHAR));
			Edit_GetText(hPort, (LPTSTR)szPort, sizeof(szPort)/sizeof(TCHAR));
			Edit_GetText(hNick, (LPTSTR)szNick, sizeof(szNick)/sizeof(TCHAR));
			Edit_GetText(hUser, (LPTSTR)szUser, sizeof(szUser)/sizeof(TCHAR));
			Edit_GetText(hChannel, (LPTSTR)szChannel, sizeof(szChannel)/sizeof(TCHAR));

			if (_tcslen(szHost) == 0) {
				MessageBox(hDlg, TEXT("You have to specify a host!"), TEXT("Error!"), MB_ICONEXCLAMATION);
				SetFocus(hHost);
				return (INT_PTR)FALSE;
			}

			if (_tcslen(szPort) == 0) {
				MessageBox(hDlg, TEXT("You have to specify a port!"), TEXT("Error!"), MB_ICONEXCLAMATION);
				SetFocus(hPort);
				return (INT_PTR)FALSE;
			}

			if (_tcslen(szNick) == 0) {
				MessageBox(hDlg, TEXT("You have to specify a nick!"), TEXT("Error!"), MB_ICONEXCLAMATION);
				SetFocus(hNick);
				return (INT_PTR)FALSE;
			}

			if (_tcslen(szUser) == 0) {
				MessageBox(hDlg, TEXT("You have to specify a user!"), TEXT("Error!"), MB_ICONEXCLAMATION);
				SetFocus(hUser);
				return (INT_PTR)FALSE;
			}

			if (_tcslen(szChannel) == 0) {
				MessageBox(hDlg, TEXT("You have to specify a channel!"), TEXT("Error!"), MB_ICONEXCLAMATION);
				SetFocus(hChannel);
				return (INT_PTR)FALSE;
			}

			_stprintf_s(szInfo, sizeof(szInfo)/sizeof(TCHAR), 
				TEXT("Connecting to %s:%s with nick \"%s\" and username \"%s\"."), 
				szHost, szPort, szNick, szUser);
			
			conn = (struct ConnectionSettings*)malloc(sizeof(struct ConnectionSettings));
			conn->szHost = _tcsdup(szHost);
			conn->dwPort = _tstoi(szPort);
			conn->szNick = _tcsdup(szNick);
			conn->szUser = _tcsdup(szUser);
			conn->szChannel = _tcsdup(szChannel);

			//MessageBox(hDlg, szInfo, TEXT("Host"), 0);
			AppendToBuffer(szInfo);

			_beginthread (NetworkThread, 0, conn) ;

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID NetworkThread (PVOID pvoid)
{
	struct ConnectionSettings *settings = (struct ConnectionSettings*)pvoid;

	struct sockaddr_in ClientSAddr;
	int ConVal;

	char **split;
	int arrSize, i;

	CHAR buffer[1024], host[50], nick[50], user[50], channel[50];
	PCHAR szRead;
	TCHAR szResult[1024];
	int iLen;

	WideCharToMultiByte(CP_ACP, 0, settings->szHost, -1, host, sizeof(host), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, settings->szNick, -1, nick, sizeof(nick), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, settings->szUser, -1, user, sizeof(user), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, settings->szChannel, -1, channel, sizeof(channel), NULL, NULL);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	// Do a dns lookup for the provided address.
	struct addrinfo* info;
	if (getaddrinfo(host, NULL, NULL, &info) != 0) {
		AppendToBuffer(TEXT("Failed to resolve host!"));
		return;
	}

	memset (&ClientSAddr, 0, sizeof(struct sockaddr));
	ClientSAddr.sin_family = AF_INET;
	ClientSAddr.sin_addr.s_addr = ((struct sockaddr_in*)info->ai_addr)->sin_addr.s_addr; //inet_addr(host);
	ClientSAddr.sin_port = htons((u_short)settings->dwPort);

	ConVal = connect(sock, (struct sockaddr*)&ClientSAddr, sizeof(struct sockaddr));

	//send(sock, "",
	sprintf_s(buffer, sizeof(buffer), "NICK %s\r\n", nick);
	send(sock, buffer, strlen(buffer), 0);

	sprintf_s(buffer, sizeof(buffer), "USER %s %s %s %s\r\n", user, user, user, user);
	send(sock, buffer, strlen(buffer), 0);

	sprintf_s(buffer, sizeof(buffer), "JOIN %s\r\n", channel);
	send(sock, buffer, strlen(buffer), 0);

	while (TRUE)
	{
		iLen = socket_read(sock, &szRead);

		arrSize = explode(&split, szRead, ' ');
		if (arrSize > 1) {

			if (strncmp(split[1], "PRIVMSG", 7) == 0) {
				char *msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 4;
				char *tmp = strchr(split[0], '!');
				*tmp = '\0';
				sprintf(buffer, "<%s> %s", split[0] + 1, msg);
				MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
			} 
			else if (strncmp(split[0], "PING", 4) == 0) {
				sprintf_s(buffer, sizeof(buffer), "PONG %s\r\n", split[1]);
				send(sock, buffer, strlen(buffer), 0);
			}
			else if (strncmp(split[1], "451", 3) == 0 && strncmp(split[2], "JOIN", 0) == 0) {
				sprintf_s(buffer, sizeof(buffer), "JOIN %s\r\n", channel);
				send(sock, buffer, strlen(buffer), 0);
			}
			else {
				MultiByteToWideChar(CP_ACP, 0, szRead, iLen, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
			}
			//MultiByteToWideChar(CP_ACP, 0, split[1], strlen(split[1])+1, szResult, sizeof(szResult)/sizeof(TCHAR));
			//AppendToBuffer(szResult);
			//MessageBox(NULL, szResult, TEXT("foo"), 0);
		}
		else {
			MultiByteToWideChar(CP_ACP, 0, szRead, iLen, szResult, sizeof(szResult)/sizeof(TCHAR));
			AppendToBuffer(szResult);
		}

		if (arrSize > 0) {
			for (i = 0; i < arrSize; i++) {
				free(split[i]);
			}
			free(split);
		}
		//Sleep(100);
	}

    shutdown(sock, SD_BOTH);
	closesocket(sock);

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

int socket_write(SOCKET sock, LPTSTR szData)
{
	int sent;
	LPCSTR szBuffer = wcToMb(szData);

	sent = send(sock, szBuffer, strlen(szBuffer), 0);
	send(sock, "\r\n", 2, 0);

	free((VOID*)szBuffer);

	return sent;
}

int socket_read(SOCKET sock, char** ret)
{
    char byte = 0, lastbyte = 0;
    int buf_size = (32 * sizeof(char)), r = 0, totalread = 0;

    char* buf = (char*)malloc(buf_size);

    memset(buf,0,buf_size);

    while (1) {

        if (totalread + sizeof(char) >= buf_size) {
            buf_size *= 2;
            buf = (char*)realloc(buf, buf_size);
        }

        r = recv(sock, &byte, sizeof(char), 0);

        if (r == 0 || r == -1) {
            break;
        }

        if (byte == 10 && lastbyte == 13) {
            buf[totalread-1] = 0;
            break;
        } else {
            buf[totalread] = byte;
        }

        totalread += r;
        lastbyte = byte;

    }

    *ret = buf;
    return totalread;
}

int explode(char*** out, char* in, unsigned char delim)
{
	int i, j, len, subLen, arraySize, last;
	char **arr, *subStr;

	len = strlen(in);
	if (len < 0) {
		return -1;
	}

	arraySize = 0;
	for (i = 0; i < len; i++) {
		if (in[i] == delim) {
			arraySize++;
		}
	}

	if (arraySize == 0) {
		return -1;
	}

	arr = (char**)malloc(sizeof(char*) * (arraySize + 1));
	last = 0;
	j = 0;
	for (i = 0; i <= len; i++) {
		if (in[i] == delim || in[i] == '\0') {
			subLen = i - last;
			subStr = (char*)malloc(sizeof(char) * (subLen + 1));
			strncpy(subStr, in + last, subLen);
			subStr[subLen] = '\0';
			arr[j] = subStr;
			j++;
			last = i + 1;
		}
	}

	*out = arr;

	return j;
}

LPSTR wcToMb(LPWSTR szData)
{
	int wLen, mbLen;
	LPSTR szBuffer;

	wLen = _tcslen(szData);
	mbLen = WideCharToMultiByte(CP_ACP, 0, szData, -1, NULL, 0, NULL, NULL);

	szBuffer = (PCHAR)malloc(mbLen+1);
	WideCharToMultiByte(CP_ACP, 0, szData, wLen, szBuffer, mbLen, NULL, NULL);
	szBuffer[mbLen-1] = '\0';

	return szBuffer;
}