#include "stdafx.h"
#include "Chatclient.h"
#include "String.h"
#include "Net.h"

BOOL Net::Initialize()
{
	// Winsock variables
	WORD wVersionRequested;
	WSADATA wsaData;
	INT nErr;

	// Initialize winsock
	wVersionRequested = MAKEWORD(2, 2);
	 
	nErr = WSAStartup(wVersionRequested, &wsaData);
	if (nErr != 0) {
		MessageBox(NULL, TEXT("Your system doesn't support the required Winsock Version!"), TEXT("Winsock Error!"), MB_ICONEXCLAMATION);
		return FALSE;
	}
	 
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		MessageBox(NULL, TEXT("Your system doesn't support the required Winsock Version!"), TEXT("Winsock Error!"), MB_ICONEXCLAMATION);
		return FALSE; 
	}

	return TRUE;
}

VOID Net::CleanUp()
{
	WSACleanup();
}

Net::Net()
{
	m_bConnected = FALSE;
	m_bConnected = FALSE;
}

BOOL Net::Connect(LPTSTR szHost, WORD wPort)
{
	struct sockaddr_in ClientSAddr;
	INT nConVal;
	CHAR szAsciiHost[50];

	m_szHost = szHost;
	m_wPort = wPort;

	WideCharToMultiByte(CP_ACP, 0, szHost, -1, szAsciiHost, sizeof(szAsciiHost)-1, NULL, NULL);

	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Do a dns lookup for the provided address.
	struct addrinfo* info;
	if (getaddrinfo(szAsciiHost, NULL, NULL, &info) != 0) {
		return FALSE;
	}

	memset (&ClientSAddr, 0, sizeof(struct sockaddr));
	ClientSAddr.sin_family = AF_INET;
	ClientSAddr.sin_addr.s_addr = ((struct sockaddr_in*)info->ai_addr)->sin_addr.s_addr; //inet_addr(host);
	ClientSAddr.sin_port = htons((u_short)wPort);

	nConVal = connect(m_sock, (struct sockaddr*)&ClientSAddr, sizeof(struct sockaddr));
	if (nConVal != 0) {
		return false;
	}

	m_bConnected = TRUE;

	return TRUE;
}

VOID Net::Disconnect()
{
    shutdown(m_sock, SD_BOTH);
	closesocket(m_sock);

	m_bConnected = FALSE;
}

INT Net::Write(LPTSTR szData)
{
	int sent;
	LPCSTR szBuffer = wcToMb(szData);

	sent = send(m_sock, szBuffer, strlen(szBuffer), 0);
	send(m_sock, "\r\n", 2, 0);

	free((VOID*)szBuffer);

	return sent;
}

INT Net::Read(LPTSTR* ret)
{
    CHAR byte = 0, lastbyte = 0;
	PCHAR buf;
    INT buf_size = (32 * sizeof(CHAR)), r = 0, totalread = 0, wcLen;
	LPWSTR szOut;

    buf = (PCHAR)malloc(buf_size);

    memset(buf, 0, buf_size);

    while (TRUE) {

        if (totalread + sizeof(CHAR) >= buf_size) {
            buf_size *= 2;
            buf = (PCHAR)realloc(buf, buf_size);
        }

        r = recv(m_sock, &byte, sizeof(CHAR), 0);

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

		//Sleep(10);
    }

	wcLen = MultiByteToWideChar(CP_ACP, 0, buf, strlen(buf)+1, NULL, 0);
	szOut = (LPWSTR)malloc((wcLen+1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, buf, strlen(buf)+1, szOut, wcLen);

    *ret = szOut;
    return wcLen;
}
