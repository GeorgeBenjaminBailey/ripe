/*
 * Memory DLL loading code
 * Version 0.0.2
 *
 * Copyright (c) 2004-2011 by Joachim Bauch / mail@joachim-bauch.de
 * http://www.joachim-bauch.de
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is MemoryModule.h
 *
 * The Initial Developer of the Original Code is Joachim Bauch.
 *
 * Portions created by Joachim Bauch are Copyright (C) 2004-2011
 * Joachim Bauch. All Rights Reserved.
 *
 */

#ifndef __MEMORY_MODULE_HEADER
#define __MEMORY_MODULE_HEADER

#include <Windows.h>

typedef void *HMEMORYMODULE;

struct LoadRemoteLibraryThreadArgs;

struct DllEntryThreadArgs;

struct GetRemoteProcAddressThreadArgs;

BOOL WINAPI DllEntryCave( DllEntryThreadArgs *args );
void DllEntryCaveTerminator();

// Functor is defined as the function pointer to LoadLibrary.
// Although this function does not have a return code, you may use
// GetExitCodeThread to get the return value from LoadLibraryA.
HMODULE WINAPI LoadRemoteLibraryCave( LoadRemoteLibraryThreadArgs *args );
// terminator which comes directly after LoadRemoteLibraryCave.  Used to get size of function
void LoadRemoteLibraryCaveTerminator();

FARPROC WINAPI
GetRemoteProcAddressCave( GetRemoteProcAddressThreadArgs *args );
void GetRemoteProcAddressCaveTerminator();

#ifdef __cplusplus
extern "C" {
#endif

HMODULE WINAPI LoadRemoteLibraryW( HANDLE hProcess, LPCTSTR lpFileName );

HMODULE WINAPI LoadRemoteLibraryA( HANDLE hProcess, LPCSTR lpFileName );

HMEMORYMODULE MemoryLoadLibrary(const void *);

// given a process handler, process ID and a pointer to the data in memory, loads a library remotely
HMODULE MemoryLoadRemoteLibrary( HANDLE hProcess, unsigned long pId, const void *data, bool cloakDependencies = false );

FARPROC MemoryGetProcAddress(HMEMORYMODULE, const char *);

void MemoryFreeLibrary(HMEMORYMODULE);

#ifdef __cplusplus
}
#endif

#endif  // __MEMORY_MODULE_HEADER
