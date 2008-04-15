#include "stdafx.h"
#include "Net.h"

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
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

BOOL is_numeric(PCHAR szStr)
{
    PCHAR szStr2;
    for (szStr2 = szStr; szStr2 != szStr + strlen(szStr); szStr2++) {
        if (!isdigit(*szStr2)) {
            return FALSE;
        }
    }

    return TRUE;
}

