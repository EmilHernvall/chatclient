
#pragma once

void ErrorExit(LPTSTR lpszFunction);

INT Explode(LPWSTR** out, LPWSTR in, WCHAR delim);
LPSTR wcToMb(LPWSTR szData);
BOOL IsNumeric(LPWSTR szStr);
