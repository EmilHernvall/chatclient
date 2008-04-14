
#pragma once

struct ConnectionSettings
{
	SOCKET sock;
	LPTSTR szHost;
	DWORD dwPort;
	LPTSTR szNick;
	LPTSTR szUser;
	LPTSTR szChannel;
};

VOID NetworkThread (PVOID pvoid);

int SocketRead(CHAR** ret);
int SocketWrite(LPTSTR szData);

struct ConnectionSettings *getConnectionSettings();

VOID ProcessCommand(LPSTR szData, INT iLen, LPSTR szChannel);
