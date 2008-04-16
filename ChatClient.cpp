#include "stdafx.h"
#include "String.h"
#include "Application.h"
#include "ChatClient.h"
#include "AboutDialog.h"
#include "ConnectDialog.h"
#include "Net.h"
#include "IRC.h"

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Strings loaded from resources
	WCHAR szTitle[MAX_LOADSTRING];
	WCHAR szWindowClass[MAX_LOADSTRING];

	// Accelerators
	HACCEL hAccelTable;

	// Initialize strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

	Net::Initialize();

	// Application
	Application appThisApp;
	ChatClient bwThisWindow(hInstance, nCmdShow, szWindowClass, szTitle);

	appThisApp.Run(&bwThisWindow);

	Net::CleanUp();

	return 0;
}

ChatClient::ChatClient(HINSTANCE hInst, INT nCmdShow, LPCWSTR szWindowClass, LPCWSTR szWindowTitle) 
: BasicWindow(hInst, nCmdShow, szWindowClass, szWindowTitle, IDI_CHATCLIENT, IDI_SMALL, IDC_CHATCLIENT)
{
	m_net = new Net();
	m_irc = new IRC(m_net);

	m_nSubWindowCount = 0;
	m_nSubWindowSize = 10;
	m_subWindows = new struct SubWindow[m_nSubWindowSize];
}

LRESULT CALLBACK ChatClient::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT wmId, wmEvent, i, nBufferLen;
	RECT rcClient;
	BOOL r1, r2;

	switch (message)
	{
	case WM_BASICWINDOW_CREATE:

		m_font = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH, TEXT("Fixedsys"));

		m_about = new AboutDialog(m_hInst, IDD_ABOUTBOX, hWnd);
		m_connect = new ConnectDialog(m_hInst, IDC_CONNECT_DIALOG, hWnd, this);

		CreateSubWindow(L"Status");
		m_currentSubWindow = &m_subWindows[0];

		m_inputEdit = new InputEdit(m_hWnd, m_hInst, this);

		SendMessage(m_inputEdit->GetHwnd(), WM_SETFONT, (WPARAM)m_font, (LPARAM)FALSE);

		break;

	case WM_SETFOCUS:
		SetFocus(m_inputEdit->GetHwnd());
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (wmId >= ID_SUBWINDOW && wmId <= ID_SUBWINDOW * 2)
		{
			i = wmId - ID_SUBWINDOW;

			if ((i & 1) > 0) {
				i = (i-1)/2;

				GetClientRect(m_hWnd, &rcClient);
				r1 = MoveWindow(m_subWindows[i].buffer->GetHwnd(), 110, 5, rcClient.right - 115, 
					rcClient.bottom - 35, TRUE);

				r1 = ShowWindow(m_currentSubWindow->buffer->GetHwnd(), SW_HIDE);
				r2 = ShowWindow(m_subWindows[i].buffer->GetHwnd(), SW_SHOW);

				m_currentSubWindow = &m_subWindows[i];

				// Move caret to the end of the buffer
				nBufferLen = Edit_GetTextLength(m_currentSubWindow->buffer->GetHwnd());
				SendMessage(m_currentSubWindow->buffer->GetHwnd(), EM_SETSEL, nBufferLen, nBufferLen);
				SendMessage(m_currentSubWindow->buffer->GetHwnd(), EM_SCROLLCARET, 0, 0);
				SetFocus(m_inputEdit->GetHwnd());
			}
		}

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
		for (i = 0; i < m_nSubWindowCount; i++)
		{
			MoveWindow(m_subWindows[i].hwndButton, 
				5, 5 * (i+1) + 30 * i, 
				100, 30, 
				TRUE);
		}

		MoveWindow(m_currentSubWindow->buffer->GetHwnd(), 110, 5, LOWORD(lParam) - 115, 
			HIWORD(lParam) - 35, TRUE);

        MoveWindow(m_inputEdit->GetHwnd(), 110, HIWORD(lParam) - 25, 
			LOWORD(lParam) - 115, 20, TRUE);
        return 0; 
	case WM_BASICWINDOW_CREATEWINDOW:
		CreateSubWindow((LPWSTR)wParam);
		break;
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

INT ChatClient::CreateSubWindow(LPWSTR szName)
{
	INT nIndex;
	RECT rc;

	if (m_nSubWindowCount == m_nSubWindowSize) {
		m_nSubWindowSize *= 2;
		m_subWindows = (struct SubWindow*)realloc(m_subWindows, sizeof(struct SubWindow) * m_nSubWindowSize);
	}

	nIndex = m_nSubWindowCount;

	m_subWindows[nIndex].id = ID_SUBWINDOW_STATUS + 2 * nIndex;
	m_subWindows[nIndex].buffer = new BufferEdit(m_hWnd, m_hInst, ID_SUBWINDOW_STATUS + 2 * nIndex);
	m_subWindows[nIndex].szName = wcsdup(szName);

	m_subWindows[nIndex].hwndButton = CreateWindow(TEXT ("button"), NULL, 
		WS_CHILD | WS_VISIBLE | BS_LEFT ,
		0, 0, 0, 0, m_hWnd, (HMENU) (ID_SUBWINDOW_STATUS + 2 * nIndex + 1),
		m_hInst, NULL);
	//SendMessage(m_subWindows[nIndex].hwndButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_SMALL)));
	SendMessage(m_subWindows[nIndex].hwndButton, WM_SETTEXT, 0, (LPARAM)m_subWindows[nIndex].szName);

	SendMessage(m_subWindows[nIndex].buffer->GetHwnd(), WM_SETFONT, (WPARAM)m_font, (LPARAM)FALSE);

	MoveWindow(m_subWindows[nIndex].hwndButton, 
		5, 5 * (nIndex+1) + 30 * nIndex, 
		100, 30, 
		TRUE);

	m_nSubWindowCount++;

	return nIndex;
}

INT ChatClient::GetWindowNameByID(LPWSTR szName)
{
	INT i;

	for (i = 0; i < m_nSubWindowCount; i++) {
		if (wcsncmp(szName, m_subWindows[i].szName, wcslen(m_subWindows[i].szName)) == 0) {
			return i;
		}
	}

	return -1;
}

// Utility method to append data to the end of the buffer edit.
void ChatClient::AppendToBuffer(INT nIndex, LPTSTR szData)
{
	int bufferLen;

	// Move caret to the end of the buffer
	bufferLen = Edit_GetTextLength(m_subWindows[nIndex].buffer->GetHwnd());
	SendMessage(m_subWindows[nIndex].buffer->GetHwnd(), EM_SETSEL, bufferLen, bufferLen);

	// Insert new data
	SendMessage(m_subWindows[nIndex].buffer->GetHwnd(), EM_REPLACESEL, 0, (LPARAM)szData);

	// Insert line break
	TCHAR* szNewLine = TEXT("\r\n");
	SendMessage(m_subWindows[nIndex].buffer->GetHwnd(), EM_REPLACESEL, 0, (LPARAM)szNewLine);
}

void ChatClient::ParseCommand(LPTSTR szBuffer)
{
	WCHAR szSendBuffer[2048], szDisplayBuffer[2048];
	LPWSTR szCommandEnd;
	INT nId, nLen;

	if (*szBuffer == '/') {
		szCommandEnd = wcschr(szBuffer, ' ');
		nLen = (szCommandEnd - szBuffer)/sizeof(WCHAR);
		if (_wcsnicmp(szBuffer, L"/join", 5) == 0) {
			//MessageBox(NULL, szCommandEnd+1, L"Foo", 0);
			swprintf(szSendBuffer, sizeof(szSendBuffer)/sizeof(WCHAR), L"JOIN %s\r\n", szCommandEnd+1);
			m_net->Write(szSendBuffer);
		}
	} else {
		nId = (m_currentSubWindow->id - ID_SUBWINDOW) / 2;

		if (nId != 0) 
		{
			_stprintf_s(szSendBuffer, 
				sizeof(szSendBuffer)/sizeof(TCHAR),
				TEXT("PRIVMSG %s :%s"), m_currentSubWindow->szName, szBuffer);

			_stprintf_s(szDisplayBuffer, 
				sizeof(szDisplayBuffer)/sizeof(TCHAR),
				TEXT("<%s> %s"), m_irc->GetNick(), szBuffer);

			AppendToBuffer(nId, szDisplayBuffer);
			m_net->Write(szSendBuffer);
		}
		else {
			AppendToBuffer(0, L"Invalid command.");
		}
	}
}

BufferEdit::BufferEdit(HWND hwndParent, HINSTANCE hInstance, INT nId)
: SubclassedWindow(L"edit", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 
				   hwndParent, nId, hInstance)
{

}

LRESULT CALLBACK BufferEdit::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		break;
	default:
		return CallWindowProc (m_prevWndProc, hWnd, message, wParam, lParam) ;
	}
	return 0;
}

InputEdit::InputEdit(HWND hwndParent, HINSTANCE hInstance, ChatClient *bwChatClient)
: SubclassedWindow(L"edit", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 
				   hwndParent, ID_INPUT, hInstance)
{
	m_bwChatClient = bwChatClient;
}

// Window proc for the input edit, subclassed to capture enter strokes.
LRESULT CALLBACK InputEdit::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Net *net;
	LPWSTR szBuffer;
	int inputLen;

	switch (message)
	{
	case WM_CHAR:
		if (wParam == 0x0D)
		{
			net = m_bwChatClient->GetNet();

			if (!net->IsConnected()) {
				m_bwChatClient->AppendToBuffer(0, L"Not connected.");
				SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);
				//AnimateWindow(m_bwChatClient->GetBufferEdit()->GetHwnd(), 200, AW_HIDE | AW_HOR_POSITIVE);
				break;
			} else {
				//AnimateWindow(m_bwChatClient->GetBufferEdit()->GetHwnd(), 200, AW_ACTIVATE | AW_CENTER);
			}

			inputLen = Edit_GetTextLength(hWnd) + 1;
			szBuffer = (LPWSTR)malloc(sizeof(WCHAR) * (inputLen + 1));
			Edit_GetText(hWnd, (LPWSTR)szBuffer, inputLen);

			m_bwChatClient->ParseCommand(szBuffer);

			free(szBuffer);

			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)NULL);

			//MessageBox(NULL, (LPTSTR)szBuffer, TEXT("m_hwndInput"), 0);
			break;
		}
	default:
		return CallWindowProc (m_prevWndProc, hWnd, message, wParam, lParam) ;
	}
	return 0;
}
