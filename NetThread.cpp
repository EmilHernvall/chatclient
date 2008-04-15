#include "stdafx.h"
#include "ChatClient.h"
#include "Net.h"
#include "IRC.h"
#include "String.h"

VOID ProcessCommand(LPWSTR szRead, int iLen, ChatClient *bwChatClient);

VOID NetworkThread(PVOID pvoid)
{
	ChatClient *bwChatClient;
	Net *net;
	IRC *irc;
	TCHAR buffer[1024];
	LPWSTR szRead;
	int iLen;

	bwChatClient = (ChatClient*)pvoid;

	net = bwChatClient->GetNet();
	irc = bwChatClient->GetIRC();

	_stprintf_s(buffer, sizeof(buffer)/sizeof(TCHAR), TEXT("NICK %s\r\n"), irc->GetNick());
	net->Write(buffer);

	_stprintf_s(buffer, sizeof(buffer)/sizeof(TCHAR), TEXT("USER %s %s %s %s\r\n"), 
		irc->GetUser(), irc->GetUser(), irc->GetUser(), irc->GetUser());
	net->Write(buffer);

	_stprintf_s(buffer, sizeof(buffer)/sizeof(TCHAR), TEXT("JOIN %s\r\n"), irc->GetChannel());
	net->Write(buffer);

	while (TRUE)
	{
		iLen = net->Read(&szRead);
		ProcessCommand(szRead, iLen, bwChatClient);
		//Sleep(10);
	}


}

VOID ProcessCommand(LPWSTR szRead, int iLen, ChatClient *bwChatClient)
{
	Net *net;
	IRC *irc;
	WCHAR szBuffer[1024];
	LPWSTR *split;
	INT arrSize, i, iCommand;
	LPTSTR szMsg, szTmp;

	net = bwChatClient->GetNet();
	irc = bwChatClient->GetIRC();

	arrSize = Explode(&split, szRead, ' ');
	if (arrSize <= 1) {
		bwChatClient->AppendToBuffer(szRead);
		return;
	}

	if (wcsncmp(split[1], L"PRIVMSG", 7) == 0) {
		szMsg = szRead + wcslen(split[0]) + wcslen(split[1]) + wcslen(split[2]) + 4;
		szTmp = wcschr(split[0], '!');
		*szTmp = '\0';
		
		swprintf_s(szBuffer, sizeof(szBuffer)/sizeof(WCHAR), L"<%s> %s", split[0] + 1, szMsg);
		bwChatClient->AppendToBuffer(szBuffer);
	} 
	else if (wcsncmp(split[1], L"NOTICE", 6) == 0) {
		szMsg = szRead + wcslen(split[0]) + wcslen(split[1]) + wcslen(split[2]) + 4;

		szTmp = wcschr(split[0], '!');
		if (szTmp != NULL) {
			*szTmp = '\0';
		}

		swprintf_s(szBuffer, sizeof(szBuffer)/sizeof(WCHAR), L" -%s- %s", split[0] + 1, szMsg);
		bwChatClient->AppendToBuffer(szBuffer);
	}
	else if (wcsncmp(split[0], L"PING", 4) == 0) {
		swprintf_s(szBuffer, sizeof(szBuffer)/sizeof(WCHAR), L"PONG %s\r\n", split[1]);
		net->Write(szBuffer);
	}
	else if (IsNumeric(split[1]))
	{
		iCommand = _wtoi(split[1]);
		switch (iCommand)
		{
		case 1: // welcome
		case 2: // host, version
		case 3: // date
			szMsg = szRead + wcslen(split[0]) + wcslen(split[1]) + wcslen(split[2]) + 4;
			bwChatClient->AppendToBuffer(szMsg);
			break;
		case 4:
		case 5:
			szMsg = szRead + wcslen(split[0]) + wcslen(split[1]) + wcslen(split[2]) + 3;
			bwChatClient->AppendToBuffer(szMsg);
			break;
		case 251: // usage stats
		case 255:
		case 265:
		case 266:
			szMsg = szRead + wcslen(split[0]) + wcslen(split[1]) + wcslen(split[2]) + 4;
			bwChatClient->AppendToBuffer(szMsg);
			break;
		case 372: // motd
		case 375: // motd start
		case 376: // motd end
			szMsg = szRead + wcslen(split[0]) + wcslen(split[1]) + wcslen(split[2]) + 4;
			bwChatClient->AppendToBuffer(szMsg);
			break;
		case 451:
			swprintf_s(szBuffer, sizeof(szBuffer)/sizeof(WCHAR), L"JOIN %s\r\n", irc->GetChannel());
			net->Write(szBuffer);
			break;
		default:
			bwChatClient->AppendToBuffer(szRead);
		}
	}
	else {
		bwChatClient->AppendToBuffer(szRead);
	}

	if (arrSize > 0) {
		for (i = 0; i < arrSize; i++) {
			free(split[i]);
		}
		free(split);
	}
}
