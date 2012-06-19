#include "stdafx.h"
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS 1
#endif
#ifdef __WXDEBUG__
	#undef __WXDEBUG__
#endif

//#include "../Check.h"

//#include <windows.h>
#include "RiPEApp.h"

//(*AppHeaders
#include "../processutility.h"
#include "../RiPE.h"
#include <wx/image.h>

#include "../cloakdll.h"
#include "../StringMangler.h"
//extern void mshscrcBypass(__in BOOL bEnable);
HANDLE         threadHandle;
extern HMODULE hModuleProcess;

static wxAppConsole *wxCreateApp()
{
	wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,StringMangler::Decode("_v]R").c_str());
	return new RiPEApp;
}

void WINAPI initRiPE()
{
	//mshscrcBypass(true);
    //Call the wxWidgets trainer class from here
	//ExtractResource(dllHandle, IDR_BIN_1, L"Filter2.txt");

	wxApp::SetInitializerFunction(wxCreateApp);
	wxEntry(0,NULL);
}

int main()
{
    RiPEApp::SetFilePath( wxGetCwd() ); // must be before CloakDll
//    CloakDll(hModule);
    initRiPE();

    return 0;
}
