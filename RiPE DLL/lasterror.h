#ifndef __LAST_ERROR_H
#define __LAST_ERROR_H

#include <Windows.h>
#include <strsafe.h>

#define PrintLastError(errorMessage) _PrintLastError((errorMessage),__LINE__)

void _PrintLastError(LPTSTR lpszFunction, int line);

#endif //__LAST_ERROR_H