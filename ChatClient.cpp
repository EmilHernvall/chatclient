#include "stdafx.h"
#include "Application.h"
#include "ChatClient.h"
#include "Net.h"
#include "IRC.h"
#include "AboutDialog.h"
#include "ConnectDialog.h"
#include "String.h"

/*struct Channel {
	INT id;
	HWND hwndButton;
	LPTSTR szName;
};

struct Channel channels[] = { { ID_CHANNEL_STATUS, NULL, TEXT("Status") },
                              { ID_CHANNELS+1, NULL, TEXT("#c0la") },
                              { ID_CHANNELS+2, NULL, TEXT("#floodffs!") } };*/

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	InputWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	BufferWndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Strings loaded from resources
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];

	// Accelerators
	HACCEL hAccelTable;

	// Application
	Application *appThisApp;

	// Main window
	ChatClient *bwThisWindow;

	Net::Initialize();

	// Initialize strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

	bwThisWindow = new ChatClient(hInstance, nCmdShow, szWindowClass, szTitle);

	appThisApp = new Application();
	appThisApp->Run(bwThisWindow);

	Net::CleanUp();

	return 0;
}

ChatClient::ChatClient(HINSTANCE hInst, INT nCmdShow, LPCWSTR szWindowClass, LPCWSTR szWindowTitle) 
: BasicWindow(hInst, nCmdShow, szWindowClass, szWindowTitle, IDI_CHATCLIENT, IDI_SMALL, IDC_CHATCLIENT)
{
	m_net = new Net();
	m_irc = new IRC(m_net);
}

LRESULT CALLBACK ChatClient::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent, i;
	WNDPROC inputProc, bufferProc;
	HFONT font;

	switch (message)
	{
	case WM_BASICWINDOW_CREATE:

		m_about = new AboutDialog(m_hInst, IDD_ABOUTBOX, hWnd);
		m_connect = new ConnectDialog(m_hInst, IDC_CONNECT_DIALOG, hWnd, this);

		m_hwndBuffer = CreateWindow(TEXT ("edit"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
			WS_BORDER | ES_LEFT | ES_MULTILINE |
			ES_AUTOVSCROLL,
			0, 0, 0, 0, hWnd, (HMENU) ID_BUFFER,
			m_hInst, NULL) ;

		m_hwndInput = CreateWindow(TEXT ("edit"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			0, 0, 0, 0, hWnd, (HMENU) ID_INPUT,
			m_hInst, NULL) ;
		SetWindowLong(m_hwndInput, GWL_USERDATA, (LONG)this);

		//SendMessage(m_hwndBuffer, (UINT)EM_SETREADONLY, TRUE, 0);

		bufferProc = (WNDPROC)SetWindowLong(m_hwndBuffer, GWL_WNDPROC, (LONG)BufferWndProc);
		SetWindowLong(m_hwndBuffer, GWL_USERDATA, (LONG)bufferProc);

		inputProc = (WNDPROC)SetWindowLong(m_hwndInput, GWL_WNDPROC, (LONG)InputWndProc);
		SetWindowLong(m_hwndInput, GWL_USERDATA, (LONG)inputProc);

		font = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH, TEXT("Fixedsys"));

		SendMessage(m_hwndBuffer, WM_SETFONT, (WPARAM)font, (LPARAM)FALSE);
		SendMessage(m_hwndInput, WM_SETFONT, (WPARAM)font, (LPARAM)FALSE);

		/*for (i = 0; i < sizeof(channels) / sizeof(struct Channel); i++)
		{
			channels[i].hwndButton = CreateWindow(TEXT ("button"), NULL, 
				WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
				0, 0, 0, 0, hWnd, (HMENU) channels[i].id,
				m_hInst, NULL);
			SendMessage(channels[i].hwndButton, WM_SETTEXT, 0, (LPARAM)channels[i].szName);
		}*/

		break;

	case WM_SETFOCUS:
		SetFocus(m_hwndInput);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_CONNECT:
			m_connect->Display();
			break;
		case IDM_ABOUT:
			m_about->Display();
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
    case WM_SIZE: 
		/*for (i = 0; i < sizeof(channels) / sizeof(struct Channel); i++)
		{
			MoveWindow(channels[i].hwndButton, 
				5, 5 * (i+1) + 30 * i, 
				100, 30, 
				TRUE);
		}*/

        MoveWindow(m_hwndBuffer, 110, 5, LOWORD(lParam) - 115, 
			HIWORD(lParam) - 35, TRUE);

        MoveWindow(m_hwndInput, 5, HIWORD(lParam) - 25, 
			LOWORD(lParam) - 10, 20, TRUE);
        return 0; 
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    case WM_CTLCOLOREDIT:
		SetBkColor((HDC)wParam, 0x000000);
        SetTextColor((HDC)wParam, RGB(0, 255, 0));
        return (LRESULT)GetStockObject(BLACK_BRUSH);
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
	ChatClient *bwChatClient;
	Net *net;
	LPWSTR szBuffer;
	int inputLen;
	WNDPROC inputProc;

	switch (message)
	{
	case WM_CHAR:
		if (wParam == 0x0D)
		{
			bwChatClient = (ChatClient*)GetWindowLong(hWnd, GWL_USERDATA);
			net = bwChatClient->GetNet();

			if (!net->IsConnected()) {
				bwChatClient->AppendToBuffer(TEXT("Not connected."));
				SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);
				break;
			}

			inputLen = Edit_GetTextLength(hWnd) + 1;
			szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * (inputLen + 1));
			Edit_GetText(hWnd, (LPWSTR)szBuffer, inputLen);

			bwChatClient->ParseCommand(szBuffer);

			free(szBuffer);

			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);

			//MessageBox(NULL, (LPTSTR)szBuffer, TEXT("m_hwndInput"), 0);
			break;
		}
	default:
		inputProc = (WNDPROC)GetWindowLong(hWnd, GWL_USERDATA);
		return CallWindowProc (inputProc, hWnd, message, wParam, lParam) ;
	}
	return 0;
}

// Utility method to append data to the end of the buffer edit.
void ChatClient::AppendToBuffer(LPTSTR szData)
{
	int bufferLen;

	// Move caret to the end of the buffer
	bufferLen = Edit_GetTextLength(m_hwndBuffer);
	SendMessage(m_hwndBuffer, EM_SETSEL, bufferLen, bufferLen);

	// Insert new data
	SendMessage(m_hwndBuffer, EM_REPLACESEL, 0, (LPARAM)szData);

	// Insert line break
	TCHAR* szNewLine = TEXT("\r\n");
	SendMessage(m_hwndBuffer, EM_REPLACESEL, 0, (LPARAM)szNewLine);
}

void ChatClient::ParseCommand(LPTSTR szBuffer)
{
	TCHAR szSendBuffer[2048], szDisplayBuffer[2048];

	if (*szBuffer == '/') {
		
	} else {
		_stprintf_s(szSendBuffer, 
			sizeof(szSendBuffer)/sizeof(TCHAR),
			TEXT("PRIVMSG %s :%s"), m_irc->GetChannel(), szBuffer);

		_stprintf_s(szDisplayBuffer, 
			sizeof(szDisplayBuffer)/sizeof(TCHAR),
			TEXT("<%s> %s"), m_irc->GetChannel(), szBuffer);

		AppendToBuffer(szDisplayBuffer);
		m_net->Write(szSendBuffer);
	}
}
