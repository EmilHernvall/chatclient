#include "stdafx.h"
#include "Chatclient.h"
#include "String.h"
#include "Net.h"

struct ConnectionSettings *conn;

int SocketWrite(LPTSTR szData)
{
	int sent;
	LPCSTR szBuffer = wcToMb(szData);

	sent = send(conn->sock, szBuffer, strlen(szBuffer), 0);
	send(conn->sock, "\r\n", 2, 0);

	free((VOID*)szBuffer);

	return sent;
}

int SocketRead(char** ret)
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

        r = recv(conn->sock, &byte, sizeof(char), 0);

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

VOID NetworkThread (PVOID pvoid)
{
	struct sockaddr_in ClientSAddr;
	int ConVal;

	CHAR buffer[1024], host[50], nick[50], user[50], channel[50];
	PCHAR szRead;

	int iLen;

	conn = (struct ConnectionSettings*)pvoid;

	WideCharToMultiByte(CP_ACP, 0, conn->szHost, -1, host, sizeof(host), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, conn->szNick, -1, nick, sizeof(nick), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, conn->szUser, -1, user, sizeof(user), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, conn->szChannel, -1, channel, sizeof(channel), NULL, NULL);

	conn->sock = socket(AF_INET, SOCK_STREAM, 0);

	// Do a dns lookup for the provided address.
	struct addrinfo* info;
	if (getaddrinfo(host, NULL, NULL, &info) != 0) {
		AppendToBuffer(TEXT("Failed to resolve host!"));
		return;
	}

	memset (&ClientSAddr, 0, sizeof(struct sockaddr));
	ClientSAddr.sin_family = AF_INET;
	ClientSAddr.sin_addr.s_addr = ((struct sockaddr_in*)info->ai_addr)->sin_addr.s_addr; //inet_addr(host);
	ClientSAddr.sin_port = htons((u_short)conn->dwPort);

	ConVal = connect(conn->sock, (struct sockaddr*)&ClientSAddr, sizeof(struct sockaddr));

	//send(sock, "",
	sprintf_s(buffer, sizeof(buffer), "NICK %s\r\n", nick);
	send(conn->sock, buffer, strlen(buffer), 0);

	sprintf_s(buffer, sizeof(buffer), "USER %s %s %s %s\r\n", user, user, user, user);
	send(conn->sock, buffer, strlen(buffer), 0);

	sprintf_s(buffer, sizeof(buffer), "JOIN %s\r\n", channel);
	send(conn->sock, buffer, strlen(buffer), 0);

	while (TRUE)
	{
		iLen = SocketRead(&szRead);
		ProcessCommand(szRead, iLen, channel);
	}

    shutdown(conn->sock, SD_BOTH);
	closesocket(conn->sock);

}

struct ConnectionSettings *getConnectionSettings()
{
	return conn;
}

VOID ProcessCommand(LPSTR szRead, int iLen, LPSTR szChannel)
{
	TCHAR szResult[1024];
	CHAR buffer[1024], **split;
	INT arrSize, i, iCommand;
	PCHAR msg, tmp;

	arrSize = explode(&split, szRead, ' ');
	if (arrSize > 1) {

		if (strncmp(split[1], "PRIVMSG", 7) == 0) {
			msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 4;
			tmp = strchr(split[0], '!');
			*tmp = '\0';
			sprintf(buffer, "<%s> %s", split[0] + 1, msg);
			MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
			AppendToBuffer(szResult);
		} 
		else if (strncmp(split[1], "NOTICE", 6) == 0) {
			msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 4;

			tmp = strchr(split[0], '!');
			if (tmp != NULL) {
				*tmp = '\0';
			}

			sprintf(buffer, " -%s- %s", split[0] + 1, msg);
			MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
			AppendToBuffer(szResult);
		}
		else if (strncmp(split[0], "PING", 4) == 0) {
			sprintf_s(buffer, sizeof(buffer), "PONG %s\r\n", split[1]);
			send(conn->sock, buffer, strlen(buffer), 0);
		}
		else if (is_numeric(split[1]))
		{
			iCommand = atoi(split[1]);
			switch (iCommand)
			{
			case 1: // welcome
			case 2: // host, version
			case 3: // date
				msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 4;
				MultiByteToWideChar(CP_ACP, 0, msg, strlen(msg)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
				break;
			case 4:
			case 5:
				msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 3;
				MultiByteToWideChar(CP_ACP, 0, msg, strlen(msg)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
				break;
			case 251: // usage stats
			case 255:
			case 265:
			case 266:
				msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 4;
				MultiByteToWideChar(CP_ACP, 0, msg, strlen(msg)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
				break;
			case 372: // motd
			case 375: // motd start
			case 376: // motd end
				msg = szRead + strlen(split[0]) + strlen(split[1]) + strlen(split[2]) + 4;
				MultiByteToWideChar(CP_ACP, 0, msg, strlen(msg)+1, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
				break;
			case 451:
				sprintf_s(buffer, sizeof(buffer), "JOIN %s\r\n", szChannel);
				send(conn->sock, buffer, strlen(buffer), 0);
				break;
			default:
				MultiByteToWideChar(CP_ACP, 0, szRead, iLen, szResult, sizeof(szResult)/sizeof(TCHAR));
				AppendToBuffer(szResult);
			}
		}
		else {
			MultiByteToWideChar(CP_ACP, 0, szRead, iLen, szResult, sizeof(szResult)/sizeof(TCHAR));
			AppendToBuffer(szResult);
		}

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
}
