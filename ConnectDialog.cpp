#include "stdafx.h"
#include "ChatClient.h"
#include "Net.h"

// Message handler for connect box.
INT_PTR CALLBACK Connect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	TCHAR szHost[200], szPort[200], szNick[200], szUser[200], szChannel[200], szInfo[1024];
	HWND hHost, hPort, hNick, hUser, hChannel;
	struct ConnectionSettings *conn;

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
