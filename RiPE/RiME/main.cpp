#include "stdafx.h"

#include "../defs.h"

#ifdef __WXDEBUG__
	#undef __WXDEBUG__
#endif
#include <windows.h>
#include "RiMEApp.h"

//(*AppHeaders
#include <wx/image.h>
#include "../RiME.h"

#include "../cloakdll.h"

HANDLE threadHandle;

static wxAppConsole *wxCreateApp()
{
	wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,"RiME");
	return new RiMEApp;
}

void WINAPI initRiME()
{
	//mshscrcBypass(true);
    //Call the wxWidgets trainer class from here
	//ExtractResource(dllHandle, IDR_BIN_1, L"Filter2.txt");
	wxApp::SetInitializerFunction(wxCreateApp);
	wxEntry(0,NULL);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    switch ( dwReason ) {
        case DLL_PROCESS_ATTACH:
            CloakDll(hModule);
            DisableThreadLibraryCalls(hModule);
            threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)initRiME, NULL, 0, NULL);
            if ( threadHandle == NULL )
                return FALSE;
            break;
            
        case DLL_PROCESS_DETACH:
			ExitProcess(0); //Helps prevent random errors when DLL is closed.
            break;
            
        // These values will never occur because we called DisableThreadLibraryCalls.
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}
