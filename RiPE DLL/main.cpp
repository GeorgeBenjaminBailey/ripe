#include "defs.h"

#include <Windows.h>

//#include "cloakdll.h"
#include "ripe_dll.h"

HANDLE threadHandle;
HMODULE hModuleProcess = NULL;

void WINAPI InitDLL()
{
    // Run the HackDLL object
    RiPE::HackDLL::GetInstance()->Run();
    //FreeLibraryAndExitThread(hModuleProcess, 0);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    switch ( dwReason )
    {
    case DLL_PROCESS_ATTACH:
//        CloakDll(hModule);
        hModuleProcess = hModule;
        threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitDLL, 
            NULL, 0, NULL);
        if ( threadHandle == NULL || threadHandle == INVALID_HANDLE_VALUE )
            return FALSE;
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}
