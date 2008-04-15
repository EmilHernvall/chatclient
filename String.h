
#pragma once

void ErrorExit(LPTSTR lpszFunction);
int explode(char*** out, char* in, unsigned char delim);
LPSTR wcToMb(LPWSTR szData);
BOOL is_numeric(PCHAR szStr);
