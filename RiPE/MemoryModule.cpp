/*
 * Major sections revised by Riukuzaki to work with remote DLL loading.
 * Below is the original copyright / author information:
 *
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
 * The Original Code is MemoryModule.c
 *
 * The Initial Developer of the Original Code is Joachim Bauch.
 *
 * Portions created by Joachim Bauch are Copyright (C) 2004-2011
 * Joachim Bauch. All Rights Reserved.
 *
 */
#include "stdafx.h"

#ifndef __GNUC__
// disable warnings about pointer <-> DWORD conversions
#pragma warning( disable : 4311 4312 )
#endif

#ifdef _WIN64
#define POINTER_TYPE ULONGLONG
#else
#define POINTER_TYPE DWORD
#endif

#pragma warning( push )
#pragma warning( disable : 4995 4996 )

#include <Windows.h>
#include <winnt.h>
#ifdef DEBUG_OUTPUT
#include <stdio.h>
#endif
#include <TlHelp32.h>

#ifndef IMAGE_SIZEOF_BASE_RELOCATION
// Vista SDKs no longer define IMAGE_SIZEOF_BASE_RELOCATION!?
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif

#include <wx/msgdlg.h>

#include "lasterror.h"
#include "MemoryModule.h"
#include "memreadwrite.h"

typedef struct {
	PIMAGE_NT_HEADERS headers;
	unsigned char *codeBase;
	HMODULE *modules;
	int numModules;
	int initialized;
} MEMORYMODULE, *PMEMORYMODULE;

typedef BOOL (WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

#define GET_HEADER_DICTIONARY(module, idx)	&(module)->headers->OptionalHeader.DataDirectory[idx]

struct LoadRemoteLibraryThreadArgs
{
    LPCTSTR lpFileName;
    FARPROC functor;
    DWORD  *entryAddress;
};

struct DllEntryThreadArgs
{
    DWORD  *functor;
    HMODULE hModule;
    DWORD   dwReason;
    LPVOID  lpvReserved;
    DWORD  *entryAddress;
};

struct GetRemoteProcAddressThreadArgs
{
    FARPROC functor;
    HMODULE hModule;
    LPCSTR  lpProcName;
    DWORD  *entryAddress;
};

// Functor is defined as the function pointer to LoadLibrary.
// Although this function does not have a return code, you may use
// GetExitCodeThread to get the return value from LoadLibraryA.
HMODULE WINAPI LoadRemoteLibraryCave( LoadRemoteLibraryThreadArgs *args )
{
    HMODULE result;
    LPCTSTR lpFileName = args->lpFileName;
    FARPROC functor    = args->functor;

    __asm
    {
        push lpFileName
        call functor    // by calling the functor in this manner, you avoid a relative call (which would break)
        mov  result,eax
    }
    *(args->entryAddress) = (DWORD)result; // replace the start of the function with the result
    return result;
}
// terminator which comes directly after LoadRemoteLibraryCave.  Used to get size of function
void LoadRemoteLibraryCaveTerminator() {}

BOOL WINAPI DllEntryCave( DllEntryThreadArgs *args )
{
    BOOL    result;
    DWORD  *functor     = args->functor;
    HMODULE hModule     = args->hModule;
    DWORD   dwReason    = args->dwReason;
    LPVOID  lpvReserved = args->lpvReserved;

    __asm
    {
        push lpvReserved
        push dwReason
        push hModule
        call functor
        mov  result,eax
    }
    *(args->entryAddress) = result; // overwrite the start of the function with the result
    return result;
}
void DllEntryCaveTerminator() {};

FARPROC WINAPI
GetRemoteProcAddressCave( GetRemoteProcAddressThreadArgs *args )
{
    FARPROC result;
    FARPROC functor    = args->functor;
    HMODULE hModule    = args->hModule;
    LPCSTR  lpProcName = args->lpProcName;
    __asm
    {
        push lpProcName
        push hModule
        call functor
        mov  result,eax
    }
    *(args->entryAddress) = (DWORD)result; // replace the start of the function with the result
    return result;
}
void GetRemoteProcAddressCaveTerminator() {}

namespace
{
    FARPROC WINAPI
    GetRemoteProcAddressInternal( HANDLE hProcess, HMODULE hModuleRemote, const char *func )
    {
        HANDLE  hThread;
        void   *pArgsRemote, *pCaveRemote, *pFuncNameRemote;   // The address (in the remote process) where 
        // szLibPath will be copied to;
        FARPROC result;

        GetRemoteProcAddressThreadArgs threadArgs;
        HMODULE hKernel32  = ::GetModuleHandle(L"Kernel32");
        threadArgs.functor = ::GetProcAddress( hKernel32,
            "GetProcAddress" );

        // 1. Allocate memory in the remote process for szLibPath
        // 2. Write szLibPath to the allocated memory

        // allocate and write the cave
        pCaveRemote = ::VirtualAllocEx( hProcess,
            NULL,
            0x100,//(SIZE_T)GetRemoteProcAddressCaveTerminator - (SIZE_T)GetRemoteProcAddressCave,
            MEM_COMMIT,
            PAGE_EXECUTE_READWRITE );
        ::WriteProcessMemory( hProcess,
            pCaveRemote,
            GetRemoteProcAddressCave,
            0x100,//(SIZE_T)GetRemoteProcAddressCaveTerminator - (SIZE_T)GetRemoteProcAddressCave,
            NULL );

        // allocate and write the string
        pFuncNameRemote = ::VirtualAllocEx( hProcess,
            NULL,
            strlen(func)+1,
            MEM_COMMIT,
            PAGE_READWRITE );
        ::WriteProcessMemory( hProcess,
            pFuncNameRemote,
            func,
            strlen(func)+1,
            NULL );
        threadArgs.lpProcName   = (LPCSTR)pFuncNameRemote;
        threadArgs.entryAddress = (DWORD *)pCaveRemote;
        threadArgs.hModule      = hModuleRemote;

        // allocate and write the struct
        pArgsRemote = ::VirtualAllocEx( hProcess,
            NULL,
            sizeof(threadArgs),
            MEM_COMMIT,
            PAGE_READWRITE );
        ::WriteProcessMemory( hProcess,
            pArgsRemote,
            &threadArgs,
            sizeof(threadArgs),
            NULL );
        // Load "LibSpy.dll" into the remote process
        // (via CreateRemoteThread & LoadLibrary)
        hThread = ::CreateRemoteThread( hProcess,
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)pCaveRemote,
            pArgsRemote,
            0,
            NULL );
        ::WaitForSingleObject( hThread, INFINITE );

        ReadProcessMemory( hProcess,
            (LPVOID)pCaveRemote,
            &result,
            sizeof(result),
            NULL );

        // Get handle of the loaded module
        //::GetExitCodeThread( hThread, (LPDWORD)&hLibModule );

        // Clean up
        ::CloseHandle( hThread );
        ::VirtualFreeEx( hProcess, // free up file name
            pFuncNameRemote,
            strlen(func)+1,
            MEM_RELEASE );
        ::VirtualFreeEx( hProcess, // free up threadArgs struct
            pArgsRemote,
            sizeof(threadArgs),
            MEM_RELEASE );
        ::VirtualFreeEx( hProcess, // free up the cave
            pCaveRemote,
            0x100,//(SIZE_T)GetRemoteProcAddressCaveTerminator - (SIZE_T)GetRemoteProcAddressCave,
            MEM_RELEASE );

        return result;
    }
}

#ifdef DEBUG_OUTPUT
static void
OutputLastError(const char *msg)
{
	LPVOID tmp;
	char *tmpmsg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&tmp, 0, NULL);
	tmpmsg = (char *)LocalAlloc(LPTR, strlen(msg) + strlen(tmp) + 3);
	sprintf(tmpmsg, "%s: %s", msg, tmp);
	OutputDebugString(tmpmsg);
	LocalFree(tmpmsg);
	LocalFree(tmp);
}
#endif

HMODULE WINAPI
LoadRemoteLibraryW( HANDLE hProcess, LPCTSTR lpFileName )
{
    HANDLE  hThread;
    //char    szLibPath[_MAX_PATH];
    void   *pLibRemote, *pCaveRemote, *pFileNameRemote;   // The address (in the remote process) where 
                                                          // szLibPath will be copied to
    HMODULE   hLibModule = NULL;   // Base address of loaded module

    LoadRemoteLibraryThreadArgs threadArgs;
    HMODULE hKernel32  = ::GetModuleHandle(L"Kernel32"); // Kernel32 is always the same as far as address
    threadArgs.functor = ::GetProcAddress( hKernel32, "LoadLibraryW" );

    // 1. Allocate memory in the remote process
    // 2. Write szLibPath to the allocated memory

    // allocate and write the cave
    pCaveRemote = ::VirtualAllocEx( hProcess,
        NULL,
        0x34, // LoadRemoteLibraryCaveTerminator - LoadRemoteLibraryCave
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE );
    ::WriteProcessMemory( hProcess,
        pCaveRemote,
        LoadRemoteLibraryCave,
        0x34, // LoadRemoteLibraryCaveTerminator - LoadRemoteLibraryCave
        NULL );

    // allocate and write the string
    pFileNameRemote = ::VirtualAllocEx( hProcess,
        NULL,
        _tcslen(lpFileName)*2+2,
        MEM_COMMIT,
        PAGE_READWRITE );
    ::WriteProcessMemory( hProcess,
        pFileNameRemote,
        lpFileName,
        _tcslen(lpFileName)*2+2,
        NULL );
    threadArgs.lpFileName = (LPCTSTR)pFileNameRemote;
    threadArgs.entryAddress = (DWORD *)pCaveRemote;

    // allocate and write the struct
    pLibRemote = ::VirtualAllocEx( hProcess,
        NULL,
        sizeof(threadArgs),
        MEM_COMMIT,
        PAGE_READWRITE );
    ::WriteProcessMemory( hProcess,
        pLibRemote,
        &threadArgs,
        sizeof(threadArgs),
        NULL );

    // Load into the remote process
    // (via CreateRemoteThread & LoadLibrary)
    hThread = ::CreateRemoteThread( hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)pCaveRemote,
        pLibRemote,
        0,
        NULL );
    ::WaitForSingleObject( hThread, INFINITE );

    ReadProcessMemory( hProcess,
        (LPVOID)pCaveRemote,
        &hLibModule,
        sizeof(hLibModule),
        NULL );

    // Get handle of the loaded module
    //::GetExitCodeThread( hThread, (LPDWORD)&hLibModule );

    // Clean up
    ::CloseHandle( hThread );
    ::VirtualFreeEx( hProcess, // free up file name
        pFileNameRemote,
        _tcslen(lpFileName)*2+2,
        MEM_RELEASE );
    ::VirtualFreeEx( hProcess, // free up threadArgs struct
        pLibRemote,
        sizeof(threadArgs),
        MEM_RELEASE );
    ::VirtualFreeEx( hProcess, // free up the cave
        pCaveRemote,
        0x34, // LoadRemoteLibraryCaveTerminator - LoadRemoteLibraryCave
        MEM_RELEASE );

    return hLibModule;
}

HMODULE WINAPI
    LoadRemoteLibraryA( HANDLE hProcess, LPCSTR lpFileName )
{
    HMODULE result = NULL;
    wchar_t wlpFileName[MAX_PATH];
    mbstowcs(wlpFileName,lpFileName,MAX_PATH);
    result = LoadRemoteLibraryW( hProcess, wlpFileName );

    return result;
}

HMODULE GetRemoteModuleHandle(HANDLE hProcess, const char *module)
{
    MODULEENTRY32 modEntry;
    HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(hProcess));

    modEntry.dwSize = sizeof(MODULEENTRY32);
    Module32First(tlh, &modEntry);

    do
    {
        char *cpModule = new char[wcstombs(NULL,modEntry.szModule,MAX_PATH)];
        wcstombs(cpModule,modEntry.szModule,MAX_PATH);
        if(!_stricmp(cpModule, module))
            return modEntry.hModule;
        modEntry.dwSize = sizeof(MODULEENTRY32);
    }
    while(Module32Next(tlh, &modEntry));

    return NULL;
}

// Retrieves the address of an exported function or variable from the specified dynamic-link library (DLL)
// of the remote process.
FARPROC WINAPI
GetRemoteProcAddress( HANDLE hProcess, HMODULE hModuleLocal, HMODULE hModuleRemote, const char *func )
{
    //   Account for potential differences in base address
    //   of modules in different processes.
    unsigned long delta = hModuleRemote - hModuleLocal;

    // if delta is not 0 and func can be read from
    if( delta != 0 && ReadPointer(NULL, func) ) {
        return GetRemoteProcAddressInternal( hProcess, hModuleRemote, func );
    }
    else
        return (FARPROC)((DWORD)GetProcAddress(hModuleLocal, func) );
}

static void
CopySections(const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
	int i, size;
	unsigned char *codeBase = module->codeBase;
	unsigned char *dest;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++) {
		if (section->SizeOfRawData == 0) {
			// section doesn't contain data in the dll itself, but may define
			// uninitialized data
			size = old_headers->OptionalHeader.SectionAlignment;
			if (size > 0) {
				dest = (unsigned char *)VirtualAlloc(codeBase + section->VirtualAddress,
					size,
					MEM_COMMIT,
					PAGE_READWRITE);

				section->Misc.PhysicalAddress = (POINTER_TYPE)dest;
				memset(dest, 0, size);
			}

			// section is empty
			continue;
		}

		// commit memory block and copy data from dll
		dest = (unsigned char *)VirtualAlloc(codeBase + section->VirtualAddress,
							section->SizeOfRawData,
							MEM_COMMIT,
							PAGE_READWRITE);
		memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
		section->Misc.PhysicalAddress = (POINTER_TYPE)dest;
	}
}

static void
CopySectionsEx( HANDLE hProcess, const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
    int size;
    unsigned char *codeBase = module->codeBase;
    unsigned char *dest;
    //PIMAGE_SECTION_HEADER section2 = IMAGE_FIRST_SECTION(module->headers);
    WORD sizeOfOptionalHeader, numberOfSections;

    // IMAGE_FIRST_SECTION work-around for remote process
    ReadProcessMemory( hProcess,
        (LPCVOID)((LONG)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.SizeOfOptionalHeader)),
        &sizeOfOptionalHeader,
        sizeof(sizeOfOptionalHeader),
        NULL );
    // number of sections work-around for remote process
    ReadProcessMemory( hProcess,
        (LPCVOID)((LONG)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.NumberOfSections)),
        &numberOfSections,
        sizeof(numberOfSections),
        NULL );

    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((ULONG_PTR)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader ) + (ULONG_PTR)sizeOfOptionalHeader);

    for( int i=0; i < numberOfSections; ++i, ++section)
    {
        DWORD sectionSizeOfRawData, sectionVirtualAddress;
        // section->SizeOfRawData work-around
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, SizeOfRawData)),
            &sectionSizeOfRawData,
            sizeof(sectionSizeOfRawData),
            NULL );
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, VirtualAddress)),
            &sectionVirtualAddress,
            sizeof(sectionVirtualAddress),
            NULL );
        if ( sectionSizeOfRawData == 0) {
            // section doesn't contain data in the dll itself, but may define
            // uninitialized data
            size = old_headers->OptionalHeader.SectionAlignment;
            if (size > 0)
            {
                // section->SizeOfRawData work-around
                WriteProcessMemory( hProcess,
                    (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, Misc.PhysicalAddress)),
                    &dest,
                    sizeof(dest),
                    NULL );
                //section->Misc.PhysicalAddress = (POINTER_TYPE)dest;

                // since we're dealing with a remote process, we need to use
                // WriteProcessMemory instead of memset
                unsigned char *memsetArr = new unsigned char[size];
                for( int i=0; i < size; ++i )
                    memsetArr[i] = 0; // 0-out the memory
                WriteProcessMemory( hProcess,
                    dest,
                    memsetArr,
                    size,
                    NULL );
                //memset(dest, 0, size);
                delete [] memsetArr;
            }

            // section is empty
            continue;
        }

        DWORD sectionPointerToRawData;
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, VirtualAddress)),
            &sectionVirtualAddress,
            sizeof(sectionVirtualAddress),
            NULL );
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, PointerToRawData)),
            &sectionPointerToRawData,
            sizeof(sectionPointerToRawData),
            NULL );
        // commit memory block and copy data from dll
        dest = (unsigned char *)VirtualAllocEx( hProcess,
            codeBase + sectionVirtualAddress,
            sectionSizeOfRawData,
            MEM_COMMIT,
            PAGE_READWRITE);
        WriteProcessMemory( hProcess,
            dest,
            data + sectionPointerToRawData,
            sectionSizeOfRawData,
            NULL );
        //memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);

        // section->SizeOfRawData work-around
        WriteProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, Misc.PhysicalAddress)),
            &dest,
            sizeof(dest),
            NULL );
        //section->Misc.PhysicalAddress = (POINTER_TYPE)dest;
    }
}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = {
	{
		// not executable
		{PAGE_NOACCESS, PAGE_WRITECOPY},
		{PAGE_READONLY, PAGE_READWRITE},
	}, {
		// executable
		{PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
		{PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
	},
};

static void
FinalizeSections(PMEMORYMODULE module)
{
	int i;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
#ifdef _WIN64
	POINTER_TYPE imageOffset = (module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
#else
	#define imageOffset 0
#endif
	
	// loop through all sections and change access flags
	for (i=0; i<module->headers->FileHeader.NumberOfSections; ++i, ++section) {
		DWORD protect, oldProtect, size;
		int executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		int readable =   (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
		int writeable =  (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;

		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) {
			// section is not needed any more and can safely be freed
			VirtualFree((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), section->SizeOfRawData, MEM_DECOMMIT);
			continue;
		}

		// determine protection flags based on characteristics
		protect = ProtectionFlags[executable][readable][writeable];
		if (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED) {
			protect |= PAGE_NOCACHE;
		}

		// determine size of region
		size = section->SizeOfRawData;
		if (size == 0) {
			if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
				size = module->headers->OptionalHeader.SizeOfInitializedData;
			} else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
				size = module->headers->OptionalHeader.SizeOfUninitializedData;
			}
		}

		if (size > 0) {
			// change memory access flags
			if (VirtualProtect((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), size, protect, &oldProtect) == 0)
            {
#ifdef DEBUG_OUTPUT
				OutputLastError("Error protecting memory page");
#endif
            }
		}
	}
#ifndef _WIN64
#undef imageOffset
#endif
}

static void
FinalizeSectionsEx( HANDLE hProcess, PMEMORYMODULE module )
{
    WORD sizeOfOptionalHeader, numberOfSections;

    // IMAGE_FIRST_SECTION work-around for remote process
    ReadProcessMemory( hProcess,
        (LPCVOID)((LONG)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.SizeOfOptionalHeader)),
        &sizeOfOptionalHeader,
        sizeof(sizeOfOptionalHeader),
        NULL );
    // number of sections work-around for remote process
    ReadProcessMemory( hProcess,
        (LPCVOID)((LONG)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.NumberOfSections)),
        &numberOfSections,
        sizeof(numberOfSections),
        NULL );

    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((ULONG_PTR)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader ) + (ULONG_PTR)sizeOfOptionalHeader);

#ifdef _WIN64
    POINTER_TYPE imageOffset = (module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
#else
#define imageOffset 0
#endif

    // loop through all sections and change access flags
    for (int i=0; i < numberOfSections; ++i, ++section) {
        DWORD protect, oldProtect, size, sectionCharacteristics, sectionMiscPhysicalAddress, sectionSizeOfRawData;
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, Characteristics)),
            &sectionCharacteristics,
            sizeof(sectionCharacteristics),
            NULL );
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, Misc.PhysicalAddress)),
            &sectionMiscPhysicalAddress,
            sizeof(sectionMiscPhysicalAddress),
            NULL );
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)section + FIELD_OFFSET(IMAGE_SECTION_HEADER, SizeOfRawData)),
            &sectionSizeOfRawData,
            sizeof(sectionSizeOfRawData),
            NULL );

        int executable = (sectionCharacteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        int readable =   (sectionCharacteristics & IMAGE_SCN_MEM_READ) != 0;
        int writeable =  (sectionCharacteristics & IMAGE_SCN_MEM_WRITE) != 0;

        if (sectionCharacteristics & IMAGE_SCN_MEM_DISCARDABLE) {
            // section is not needed any more and can safely be freed
            VirtualFreeEx( hProcess,
                (LPVOID)((POINTER_TYPE)sectionMiscPhysicalAddress | imageOffset),
                sectionSizeOfRawData,
                MEM_DECOMMIT);
            continue;
        }

        // determine protection flags based on characteristics
        protect = ProtectionFlags[executable][readable][writeable];
        if (sectionCharacteristics & IMAGE_SCN_MEM_NOT_CACHED) {
            protect |= PAGE_NOCACHE;
        }

        // determine size of region
        size = sectionSizeOfRawData;
        if (size == 0) {
            if (sectionCharacteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
                //size = module->headers->OptionalHeader.SizeOfInitializedData;
                ReadProcessMemory( hProcess,
                    (LPVOID)((LONG)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader.SizeOfInitializedData)),
                    &size,
                    sizeof(size),
                    NULL );
            } else if (sectionCharacteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
                //size = module->headers->OptionalHeader.SizeOfUninitializedData;
                ReadProcessMemory( hProcess,
                    (LPVOID)((LONG)(module->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader.SizeOfUninitializedData)),
                    &size,
                    sizeof(size),
                    NULL );
            }
        }

        if (size > 0) {
            // change memory access flags
            if (VirtualProtectEx( hProcess,
                (LPVOID)((POINTER_TYPE)sectionMiscPhysicalAddress | imageOffset),
                size,
                protect,
                &oldProtect) == 0)
            {
#ifdef DEBUG_OUTPUT
                OutputLastError("Error protecting memory page");
#endif
            }
        }
    }
#ifndef _WIN64
#undef imageOffset
#endif
}

static void
PerformBaseRelocation(PMEMORYMODULE module, SIZE_T delta)
{
	DWORD i;
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	if (directory->Size > 0) {
		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION) (codeBase + directory->VirtualAddress);
		for (; relocation->VirtualAddress > 0; ) {
			unsigned char *dest = codeBase + relocation->VirtualAddress;
			unsigned short *relInfo = (unsigned short *)((unsigned char *)relocation + IMAGE_SIZEOF_BASE_RELOCATION);
			for (i=0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION) / 2); ++i, ++relInfo) {
				DWORD *patchAddrHL;
#ifdef _WIN64
				ULONGLONG *patchAddr64;
#endif
				int type, offset;

				// the upper 4 bits define the type of relocation
				type = *relInfo >> 12;
				// the lower 12 bits define the offset
				offset = *relInfo & 0xfff;
				
				switch (type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					// skip relocation
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					// change complete 32 bit address
					patchAddrHL = (DWORD *) (dest + offset);
					*patchAddrHL += delta;
					break;
				
#ifdef _WIN64
				case IMAGE_REL_BASED_DIR64:
					patchAddr64 = (ULONGLONG *) (dest + offset);
					*patchAddr64 += delta;
					break;
#endif

				default:
					//printf("Unknown relocation: %d\n", type);
					break;
				}
			}

			// advance to next relocation block
			relocation = (PIMAGE_BASE_RELOCATION) (((char *) relocation) + relocation->SizeOfBlock);
		}
	}
}

static void
PerformBaseRelocationEx( HANDLE hProcess, PMEMORYMODULE module, SIZE_T delta)
{
    unsigned char *codeBase = module->codeBase;
    DWORD directorySize, directoryVirtualAddress;

    //PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    PIMAGE_DATA_DIRECTORY directory = (PIMAGE_DATA_DIRECTORY)(((LONG)(module->headers) + 
        FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader.DataDirectory)) + 
        sizeof(IMAGE_DATA_DIRECTORY)*(LONG)IMAGE_DIRECTORY_ENTRY_BASERELOC);

    ReadProcessMemory( hProcess,
        (LPVOID)((LONG)directory + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, Size)),
        &directorySize,
        sizeof(directorySize),
        NULL );
    ReadProcessMemory( hProcess,
        (LPVOID)((LONG)directory + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, VirtualAddress)),
        &directoryVirtualAddress,
        sizeof(directoryVirtualAddress),
        NULL );

    if( directorySize > 0 ) {
        DWORD relocationVirtualAddress;
        PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION) (codeBase + directoryVirtualAddress);

        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)relocation + FIELD_OFFSET(IMAGE_BASE_RELOCATION, VirtualAddress)),
            &relocationVirtualAddress,
            sizeof(relocationVirtualAddress),
            NULL );

        for( ; relocationVirtualAddress > 0; ) {
            DWORD relocationSizeOfBlock;
            unsigned char *dest = codeBase + relocationVirtualAddress;
            unsigned short *relInfo = (unsigned short *)((unsigned char *)relocation + IMAGE_SIZEOF_BASE_RELOCATION);

            ReadProcessMemory( hProcess,
                (LPVOID)((LONG)relocation + FIELD_OFFSET(IMAGE_BASE_RELOCATION, SizeOfBlock)),
                &relocationSizeOfBlock,
                sizeof(relocationSizeOfBlock),
                NULL );

            for( unsigned int i=0; i < ((relocationSizeOfBlock - IMAGE_SIZEOF_BASE_RELOCATION) / 2); ++i, ++relInfo) {
                DWORD *patchAddrHL;
#ifdef _WIN64
                ULONGLONG *patchAddr64;
#endif
                SIZE_T patchAddrHLDeref;
                int type, offset;
                unsigned short relInfoDeref;
                ReadProcessMemory( hProcess,
                    (LPVOID)relInfo,
                    &relInfoDeref,
                    sizeof(relInfoDeref),
                    NULL );

                // the upper 4 bits define the type of relocation
                type = relInfoDeref >> 12;
                // the lower 12 bits define the offset
                offset = relInfoDeref & 0xfff;

                switch (type)
                {
                case IMAGE_REL_BASED_ABSOLUTE:
                    // skip relocation
                    break;

                case IMAGE_REL_BASED_HIGHLOW:
                    // change complete 32 bit address
                    patchAddrHL = (DWORD *) (dest + offset);
                    ReadProcessMemory( hProcess,
                        patchAddrHL,
                        &patchAddrHLDeref,
                        sizeof(patchAddrHLDeref),
                        NULL );
                    patchAddrHLDeref += delta;
                    WriteProcessMemory( hProcess,
                        patchAddrHL,
                        &patchAddrHLDeref,
                        sizeof(patchAddrHLDeref),
                        NULL );
                    //*patchAddrHL += delta;
                    break;

#ifdef _WIN64
                case IMAGE_REL_BASED_DIR64:
                    patchAddr64 = (ULONGLONG *) (dest + offset);
                    *patchAddr64 += delta;
                    break;
#endif

                default:
                    //printf("Unknown relocation: %d\n", type);
                    break;
                }
            }

            // advance to next relocation block
            relocation = (PIMAGE_BASE_RELOCATION) (((char *) relocation) + relocationSizeOfBlock);
            ReadProcessMemory( hProcess,
                (LPVOID)((LONG)relocation + FIELD_OFFSET(IMAGE_BASE_RELOCATION, SizeOfBlock)),
                &relocationSizeOfBlock,
                sizeof(relocationSizeOfBlock),
                NULL );
            ReadProcessMemory( hProcess,
                (LPVOID)((LONG)relocation + FIELD_OFFSET(IMAGE_BASE_RELOCATION, VirtualAddress)),
                &relocationVirtualAddress,
                sizeof(relocationVirtualAddress),
                NULL );
        }
    }
}

static int
BuildImportTable(PMEMORYMODULE module)
{
	int result=1;
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (directory->Size > 0) {
		PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR) (codeBase + directory->VirtualAddress);
		for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) {
			POINTER_TYPE *thunkRef;
			FARPROC *funcRef;
			HMODULE handle = LoadLibraryA((LPCSTR) (codeBase + importDesc->Name));
			if (handle == INVALID_HANDLE_VALUE) {
#if DEBUG_OUTPUT
				OutputLastError("Can't load library");
#endif
				result = 0;
				break;
			}

			module->modules = (HMODULE *)realloc(module->modules, (module->numModules+1)*(sizeof(HMODULE)));
			if (module->modules == NULL) {
				result = 0;
				break;
			}

			module->modules[module->numModules++] = handle;
			if (importDesc->OriginalFirstThunk) {
				thunkRef = (POINTER_TYPE *) (codeBase + importDesc->OriginalFirstThunk);
				funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
			} else {
				// no hint table
				thunkRef = (POINTER_TYPE *) (codeBase + importDesc->FirstThunk);
				funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
			}
			for (; *thunkRef; ++thunkRef, ++funcRef) {
				if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
					*funcRef = (FARPROC)GetProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef));
				} else {
					PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (*thunkRef));
					*funcRef = (FARPROC)GetProcAddress(handle, (LPCSTR)&thunkData->Name);
				}
				if (*funcRef == 0) {
					result = 0;
					break;
				}
			}

			if (!result) {
				break;
			}
		}
	}

	return result;
}

static int
    BuildImportTableEx( HANDLE hProcess, PMEMORYMODULE module, unsigned long pId = 0, bool cloakDependencies = false )
{
    int result=1;
    unsigned char *codeBase = module->codeBase;
    DWORD directorySize;
    
    //PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
    PIMAGE_DATA_DIRECTORY directory = (PIMAGE_DATA_DIRECTORY)(((LONG)(module->headers) + 
        FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader.DataDirectory)) + 
        sizeof(IMAGE_DATA_DIRECTORY)*(LONG)IMAGE_DIRECTORY_ENTRY_IMPORT);

    ReadProcessMemory( hProcess,
        (LPVOID)((LONG)directory + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, Size)),
        &directorySize,
        sizeof(directorySize),
        NULL );
    if (directorySize > 0) {
        DWORD directoryVirtualAddress, importDescName;
        //directory->VirtualAddress
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)directory + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, VirtualAddress)),
            &directoryVirtualAddress,
            sizeof(directoryVirtualAddress),
            NULL );

        PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR) (codeBase + directoryVirtualAddress);
        //importDesc->Name
        ReadProcessMemory( hProcess,
            (LPVOID)((LONG)importDesc + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, Name)),
            &importDescName,
            sizeof(importDescName),
            NULL );
        for (; /*!IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) &&*/ importDescName; ) {
            DWORD importDescOriginalFirstThunk, importDescFirstThunk;
            POINTER_TYPE *thunkRef;
            FARPROC *funcRef;
            char lpFileName[MAX_PATH];
            ReadProcessMemory( hProcess,
                (LPCVOID)((DWORD)codeBase + importDescName),
                &lpFileName,
                sizeof(lpFileName),
                NULL );
            
            HMODULE handleRemote;

            if( cloakDependencies ) {
                FILE          *fp;
                unsigned char *data = NULL;
                size_t         size = 0;
                fp = fopen(lpFileName, "rb");
                if( fp == NULL ) {
                    result = 0;
                    break;
                }

                fseek( fp, 0, SEEK_END );
                size = ftell( fp );
                data = new unsigned char[size];
                fseek( fp, 0, SEEK_SET );
                fread( data, 1, size, fp );
                fclose( fp );

                HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pId );

                handleRemote = MemoryLoadRemoteLibrary( hProcess, pId, data, cloakDependencies );
                //handleRemote = LoadRemoteLibraryA( hProcess, lpFileName );
            }
            else
                handleRemote = LoadRemoteLibraryA( hProcess, lpFileName );
            // This is where I break
            HMODULE handleLocal = LoadLibraryA( lpFileName );
            if (handleRemote == INVALID_HANDLE_VALUE) {
#if DEBUG_OUTPUT
                OutputLastError("Can't load library");
#endif
                result = 0;
                break;
            }
            
            // I think it's safe to leave this as-is
            module->modules = (HMODULE *)realloc(module->modules, (module->numModules+1)*(sizeof(HMODULE)));
            if (module->modules == NULL) {
                result = 0;
                break;
            }
            
            module->modules[module->numModules++] = handleRemote;
            ReadProcessMemory( hProcess,
                (LPVOID)((LONG)importDesc + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, OriginalFirstThunk)),
                &importDescOriginalFirstThunk,
                sizeof(importDescOriginalFirstThunk),
                NULL );
            ReadProcessMemory( hProcess,
                (LPVOID)((LONG)importDesc + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, FirstThunk)),
                &importDescFirstThunk,
                sizeof(importDescFirstThunk),
                NULL );
            if (importDescOriginalFirstThunk) {
                thunkRef = (POINTER_TYPE *) (codeBase + importDescOriginalFirstThunk);
                funcRef = (FARPROC *) (codeBase + importDescFirstThunk);
            } else {
                // no hint table
                thunkRef = (POINTER_TYPE *) (codeBase + importDescFirstThunk);
                funcRef = (FARPROC *) (codeBase + importDescFirstThunk);
            }
            DWORD thunkRefDeref;
            ReadProcessMemory( hProcess,
                thunkRef,
                &thunkRefDeref,
                sizeof(thunkRefDeref),
                NULL );
            for (; thunkRefDeref; ++funcRef) {
                if (IMAGE_SNAP_BY_ORDINAL(thunkRefDeref)) {
                    //*funcRef = (FARPROC)GetProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(thunkRefDeref));
                    FARPROC funcRefAddr = (FARPROC)GetRemoteProcAddress( hProcess, handleLocal, handleRemote, (LPCSTR)IMAGE_ORDINAL(thunkRefDeref));
                    WriteProcessMemory( hProcess,
                        funcRef,
                        &funcRefAddr,
                        sizeof(funcRefAddr),
                        NULL );
                } else {
                    char funcStr[MAX_PATH];
                    PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (thunkRefDeref));
                    //*funcRef = (FARPROC)GetProcAddress(handle, (LPCSTR)&thunkData->Name);
                    ReadProcessMemory( hProcess,
                        &thunkData->Name,
                        &funcStr,
                        sizeof(funcStr),
                        NULL );
                    FARPROC funcRefAddr = (FARPROC)GetRemoteProcAddress( hProcess, handleLocal, handleRemote, funcStr);
                    WriteProcessMemory( hProcess,
                        funcRef,
                        &funcRefAddr,
                        sizeof(funcRefAddr),
                        NULL );
                }
                FARPROC funcRefDeref;
                ReadProcessMemory( hProcess,
                    funcRef,
                    &funcRefDeref,
                    sizeof(funcRefDeref),
                    NULL );
                if (funcRefDeref == 0) {
                    result = 0;
                    break;
                }
                ++thunkRef;
                // update thunkRefDeref
                ReadProcessMemory( hProcess,
                    thunkRef,
                    &thunkRefDeref,
                    sizeof(thunkRefDeref),
                    NULL );
            }

            if (!result) {
                break;
            }
            // update importDesc
            ++importDesc;
            // fix importDescName
            ReadProcessMemory( hProcess,
                (LPVOID)((LONG)importDesc + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, Name)),
                &importDescName,
                sizeof(importDescName),
                NULL );
        }
    }

    return result;
}

HMEMORYMODULE MemoryLoadLibrary(const void *data)
{
	PMEMORYMODULE result;
	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS old_header;
	unsigned char *code, *headers;
	SIZE_T locationDelta;
	DllEntryProc DllEntry;
	BOOL successfull;

	dos_header = (PIMAGE_DOS_HEADER)data;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
#if DEBUG_OUTPUT
		OutputDebugString("Not a valid executable file.\n");
#endif
		return NULL;
	}

	old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(data))[dos_header->e_lfanew];
	if (old_header->Signature != IMAGE_NT_SIGNATURE) {
#if DEBUG_OUTPUT
		OutputDebugString("No PE header found.\n");
#endif
		return NULL;
	}

	// reserve memory for image of library
	code = (unsigned char *)VirtualAlloc((LPVOID)(old_header->OptionalHeader.ImageBase),
		old_header->OptionalHeader.SizeOfImage,
		MEM_RESERVE,
		PAGE_READWRITE);

    if (code == NULL) {
        // try to allocate memory at arbitrary position
        code = (unsigned char *)VirtualAlloc(NULL,
            old_header->OptionalHeader.SizeOfImage,
            MEM_RESERVE,
            PAGE_READWRITE);
		if (code == NULL) {
#if DEBUG_OUTPUT
			OutputLastError("Can't reserve memory");
#endif
			return NULL;
		}
	}
    
	result = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), 0, sizeof(MEMORYMODULE));
	result->codeBase = code;
	result->numModules = 0;
	result->modules = NULL;
	result->initialized = 0;

	// XXX: is it correct to commit the complete memory region at once?
    //      calling DllEntry raises an exception if we don't...
	VirtualAlloc(code,
		old_header->OptionalHeader.SizeOfImage,
		MEM_COMMIT,
		PAGE_READWRITE);

	// commit memory for headers
	headers = (unsigned char *)VirtualAlloc(code,
		old_header->OptionalHeader.SizeOfHeaders,
		MEM_COMMIT,
		PAGE_READWRITE);
	
	// copy PE header to code
	memcpy(headers, dos_header, dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders);
	result->headers = (PIMAGE_NT_HEADERS)&((const unsigned char *)(headers))[dos_header->e_lfanew];

	// update position
	result->headers->OptionalHeader.ImageBase = (POINTER_TYPE)code;

	// copy sections from DLL file block to new memory location
	CopySections( (const unsigned char *)data, old_header, result);

	// adjust base address of imported data
	locationDelta = (SIZE_T)(code - old_header->OptionalHeader.ImageBase);
	if (locationDelta != 0) {
		PerformBaseRelocation(result, locationDelta);
	}

	// load required dlls and adjust function table of imports
	if (!BuildImportTable(result)) {
		goto error;
	}

	// mark memory pages depending on section headers and release
	// sections that are marked as "discardable"
	FinalizeSections(result);

	// get entry point of loaded library
	if (result->headers->OptionalHeader.AddressOfEntryPoint != 0) {
		DllEntry = (DllEntryProc) (code + result->headers->OptionalHeader.AddressOfEntryPoint);
		if (DllEntry == 0) {
#if DEBUG_OUTPUT
			OutputDebugString("Library has no entry point.\n");
#endif
			goto error;
		}

		// notify library about attaching to process
		successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
		if (!successfull) {
#if DEBUG_OUTPUT
			OutputDebugString("Can't attach library.\n");
#endif
			goto error;
		}
		result->initialized = 1;
	}

	return (HMEMORYMODULE)result;

error:
	// cleanup
	MemoryFreeLibrary(result);
	return NULL;
}

// given a process ID and a pointer to the data in memory, loads a library remotely
HMODULE MemoryLoadRemoteLibrary( HANDLE hProcess, unsigned long pId, const void *data, bool cloakDependencies )
{
    HMODULE result;
    PMEMORYMODULE temp;
    PIMAGE_DOS_HEADER dos_header;
    PIMAGE_NT_HEADERS old_header;
    unsigned char *code, *headers;
    SIZE_T locationDelta;
    DllEntryProc DllEntry;
    BOOL successfull;

    // open the target process
    if( hProcess == NULL ) {
        PrintLastError( L"OpenProcess" );

        return NULL;
    }

    dos_header = (PIMAGE_DOS_HEADER)data;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        wxMessageBox( _("Not a valid executable file.\n"),
            _("An error has occurred"),
            wxICON_ERROR|wxOK );
        return NULL;
    }

    old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(data))[dos_header->e_lfanew];
    if (old_header->Signature != IMAGE_NT_SIGNATURE) {
        wxMessageBox( _("No PE header found.\n"),
            _("An error has occurred"),
            wxICON_ERROR|wxOK );
        return NULL;
    }

    // reserve memory for image of library
    code = (unsigned char *)VirtualAllocEx( hProcess,
        (LPVOID)(old_header->OptionalHeader.ImageBase),
        old_header->OptionalHeader.SizeOfImage,
        MEM_RESERVE,
        PAGE_READWRITE);

    if (code == NULL) {
        // try to allocate memory at arbitrary position
        code = (unsigned char *)VirtualAllocEx( hProcess,
            NULL,
            old_header->OptionalHeader.SizeOfImage,
            MEM_RESERVE,
            PAGE_READWRITE);
        if (code == NULL) {
            PrintLastError( L"VirtualAllocEx" );
            return NULL;
        }
    }

    temp = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), 0, sizeof(MEMORYMODULE));
    if( temp == NULL ) {
        PrintLastError( L"HeapAlloc" );

        //TODO:  Add cleanup code for VirtualAllocEx
        return NULL;
    }
    temp->codeBase = code;
    temp->numModules = 0;
    temp->modules = NULL;
    temp->initialized = 0;

    // XXX: is it correct to commit the complete memory region at once?
    //      calling DllEntry raises an exception if we don't...
    result = (HMODULE)VirtualAllocEx( hProcess,
        code,
        old_header->OptionalHeader.SizeOfImage,
        MEM_COMMIT,
        PAGE_READWRITE);
    if( result == NULL ) {
        PrintLastError( L"VirtualAllocEx" );
        return NULL;
    }

    // commit memory for headers
    headers = (unsigned char *)VirtualAllocEx( hProcess,
        code,
        old_header->OptionalHeader.SizeOfHeaders,
        MEM_COMMIT,
        PAGE_READWRITE);
    if( headers == NULL ) {
        PrintLastError( L"VirtualAllocEx" );
        return NULL;
    }

    // copy PE header to code
    SIZE_T lpNumberOfBytesWritten;
    if( !WriteProcessMemory( hProcess,
        headers,
        dos_header,
        dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders,
        &lpNumberOfBytesWritten ) ) {
        PrintLastError( L"WriteProcessMemory" );

        return NULL;
    }
    temp->headers = (PIMAGE_NT_HEADERS)(headers+dos_header->e_lfanew);

    // update position
    PIMAGE_NT_HEADERS nullHeaderPtr = NULL;
    WriteProcessMemory( hProcess,
        &(temp->headers->OptionalHeader.ImageBase),
        &code,
        sizeof(temp->headers->OptionalHeader.ImageBase),
        &lpNumberOfBytesWritten );

    // copy sections from DLL file block to new memory location
    CopySectionsEx( hProcess, (const unsigned char *)data, old_header, temp);

    // adjust base address of imported data
    locationDelta = (SIZE_T)(code - old_header->OptionalHeader.ImageBase);
    if (locationDelta != 0) {
        PerformBaseRelocationEx(hProcess, temp, locationDelta);
    }
    
    // load required dlls and adjust function table of imports
    if( !BuildImportTableEx(hProcess, temp, pId, cloakDependencies) ) {
        goto error;
    }

    // mark memory pages depending on section headers and release
    // sections that are marked as "discardable"
    FinalizeSectionsEx(hProcess, temp);
    
    // get entry point of loaded library
    DWORD optionalHeaderAddressOfEntryPoint;
    ReadProcessMemory( hProcess,
        (LPCVOID)((LONG)(temp->headers) + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader.AddressOfEntryPoint)),
        &optionalHeaderAddressOfEntryPoint,
        sizeof(optionalHeaderAddressOfEntryPoint),
        NULL );
    if (optionalHeaderAddressOfEntryPoint != 0) {
        DllEntry = (DllEntryProc) (code + optionalHeaderAddressOfEntryPoint);
        if (DllEntry == 0) {
#if DEBUG_OUTPUT
            OutputDebugString("Library has no entry point.\n");
#endif
            goto error;
        }

        // notify library about attaching to process
        void *pCaveRemote, *pLibRemote, *hThread;
        DllEntryThreadArgs threadArgs;
        // allocate and write the cave
        pCaveRemote = ::VirtualAllocEx( hProcess,
            NULL,
            0x100,//(SIZE_T)DllEntryCaveTerminator - (SIZE_T)DllEntryCave,
            MEM_COMMIT,
            PAGE_EXECUTE_READWRITE );
        ::WriteProcessMemory( hProcess,
            pCaveRemote,
            DllEntryCave,
            0x100,//(SIZE_T)DllEntryCaveTerminator - (SIZE_T)DllEntryCave,
            NULL );
        threadArgs.entryAddress = (DWORD *)pCaveRemote;
        threadArgs.functor     = (DWORD *)DllEntry;
        threadArgs.hModule     = (HMODULE)code;
        threadArgs.dwReason    = DLL_PROCESS_ATTACH;
        threadArgs.lpvReserved = 0;
        // allocate and write the struct
        pLibRemote = ::VirtualAllocEx( hProcess,
            NULL,
            sizeof(threadArgs),
            MEM_COMMIT,
            PAGE_READWRITE );
        ::WriteProcessMemory( hProcess,
            pLibRemote,
            &threadArgs,
            sizeof(threadArgs),
            NULL );

        // Execute remote DLL's entry code
        // (via CreateRemoteThread & a code cave)
        hThread = ::CreateRemoteThread( hProcess,
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)pCaveRemote,
            pLibRemote,
            0,
            NULL );
        ::WaitForSingleObject( hThread, 10000 );
        ReadProcessMemory( hProcess,
            pCaveRemote,
            &successfull,
            sizeof(successfull),
            NULL );
        //successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
        if (!successfull) {
#if DEBUG_OUTPUT
            OutputDebugString("Can't attach library.\n");
#endif
            goto error;
        }
        temp->initialized = 1;
    }
    return result;
    
error:
    // cleanup
    MemoryFreeLibrary(temp);
    
    return NULL;
}

FARPROC MemoryGetProcAddress(HMEMORYMODULE module, const char *name)
{
	unsigned char *codeBase = ((PMEMORYMODULE)module)->codeBase;
	int idx=-1;
	DWORD i, *nameRef;
	WORD *ordinal;
	PIMAGE_EXPORT_DIRECTORY exports;
	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE)module, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if (directory->Size == 0) {
		// no export table found
		return NULL;
	}

	exports = (PIMAGE_EXPORT_DIRECTORY) (codeBase + directory->VirtualAddress);
	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0) {
		// DLL doesn't export anything
		return NULL;
	}

	// search function name in list of exported names
	nameRef = (DWORD *) (codeBase + exports->AddressOfNames);
	ordinal = (WORD *) (codeBase + exports->AddressOfNameOrdinals);
	for (i=0; i<exports->NumberOfNames; i++, nameRef++, ordinal++) {
		if (_stricmp(name, (const char *) (codeBase + (*nameRef))) == 0) {
			idx = *ordinal;
			break;
		}
	}

	if (idx == -1) {
		// exported symbol not found
		return NULL;
	}

	if ((DWORD)idx > exports->NumberOfFunctions) {
		// name <-> ordinal number don't match
		return NULL;
	}

	// AddressOfFunctions contains the RVAs to the "real" functions
	return (FARPROC) (codeBase + (*(DWORD *) (codeBase + exports->AddressOfFunctions + (idx*4))));
}

void MemoryFreeLibrary(HMEMORYMODULE mod)
{
	int i;
	PMEMORYMODULE module = (PMEMORYMODULE)mod;

	if (module != NULL) {
		if (module->initialized != 0) {
			// notify library about detaching from process
			DllEntryProc DllEntry = (DllEntryProc) (module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
			(*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
			module->initialized = 0;
		}

		if (module->modules != NULL) {
			// free previously opened libraries
			for (i=0; i<module->numModules; i++) {
				if (module->modules[i] != INVALID_HANDLE_VALUE) {
					FreeLibrary(module->modules[i]);
				}
			}

			free(module->modules);
		}

		if (module->codeBase != NULL) {
			// release memory of library
			VirtualFree(module->codeBase, 0, MEM_RELEASE);
		}

		HeapFree(GetProcessHeap(), 0, module);
	}
}

#pragma warning( pop )
