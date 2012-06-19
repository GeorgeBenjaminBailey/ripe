#include "stdafx.h"

#include "processutility.h"

const wxString GetCurrentProcessDirectory()
{
    return GetProcessDirectory(NULL);
}

const wxString GetProcessDirectory(HMODULE hModule)
{
    return GetProcessPath(hModule).BeforeLast('\\') + '\\';
}

const wxString GetProcessPath(HMODULE hModule)
{
    char buf[MAX_PATH] = { 0 };
    GetModuleFileNameA(hModule, buf, sizeof(buf) );

    return wxString(buf);
}
