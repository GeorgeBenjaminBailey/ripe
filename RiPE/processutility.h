#ifndef __PROCESS_UTILITY_H
#define __PROCESS_UTILITY_H

#include <Windows.h>

#include <wx/string.h>

const wxString GetCurrentProcessDirectory();

// Returns the directory of the file e.g. C:\foo\ 
const wxString GetProcessDirectory(HMODULE hModule);

// Returns the fully qualified filename e.g. C:\foo\bar.txt
const wxString GetProcessPath(HMODULE hModule);

#endif // __PROCESS_UTILITY_H