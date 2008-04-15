#include "stdafx.h"
#include "ConnectDialog.h"
#include "ChatClient.h"
#include "Net.h"
#include "IRC.h"

VOID NetworkThread(PVOID pvoid);

ConnectDialog::ConnectDialog(HINSTANCE hInst, INT nResource, HWND hwndParent, ChatClient *bwChatClient)
: Dialog(hInst, nResource, hwndParent)
{
	m_bwChatClient = bwChatClient;
	m_net = bwChatClient->GetNet();
	m_irc = bwChatClient->GetIRC();
}

// Message handler for connect box.
INT_PTR CALLBACK ConnectDialog::HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	TCHAR szHost[200], szPort[200], szNick[200], szUser[200], szChannel[200], szInfo[1024];
	LPTSTR szConnectHost, szConnectNick, szConnectUser, szConnectChannel;
	WORD wConnectPort;
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
			
			szConnectHost = _tcsdup(szHost);
			wConnectPort = _tstoi(szPort);
			szConnectNick = _tcsdup(szNick);
			szConnectUser = _tcsdup(szUser);
			szConnectChannel = _tcsdup(szChannel);

			m_bwChatClient->AppendToBuffer(szInfo);

			m_net->Connect(szConnectHost, wConnectPort);
			m_irc->SetNick(szConnectNick);
			m_irc->SetUser(szConnectUser);
			m_irc->SetChannel(szConnectChannel);

			_beginthread (NetworkThread, 0, m_bwChatClient);

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
