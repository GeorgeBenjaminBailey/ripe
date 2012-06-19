#include "stdafx.h"

#include "enumModulesDlg.h"

//(*InternalHeaders(enumModulesDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/msgdlg.h>

#include "../hookhop.h"

//(*IdInit(enumModulesDlg)
const long enumModulesDlg::ID_SEARCHCTRL1 = wxNewId();
const long enumModulesDlg::ID_TREECTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(enumModulesDlg,wxDialog)
	//(*EventTable(enumModulesDlg)
	//*)
END_EVENT_TABLE()

std::set<EnumModuleInfo> enumModulesDlg::moduleSymbols;

enumModulesDlg::enumModulesDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(enumModulesDlg)
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Enumerate Modules and Symbols"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	SetClientSize(wxSize(-1,-1));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	SearchCtrl1 = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	FlexGridSizer2->Add(SearchCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TreeCtrl1 = new wxTreeCtrl(this, ID_TREECTRL1, wxDefaultPosition, wxSize(250,250), wxTR_TWIST_BUTTONS|wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
	FlexGridSizer1->Add(TreeCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_SEARCHCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&enumModulesDlg::OnSearchCtrl1Text);
	//*)
}

enumModulesDlg::~enumModulesDlg()
{
	//(*Destroy(enumModulesDlg)
	//*)
}

// list all process modules inside of TreeCtrl1
int enumModulesDlg::GetAllModules( DWORD processID )
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Get a handle to the process.
    hProcess = HookHop::FunctionHop( OpenProcess, PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID );
    if (NULL == hProcess)
        return 1;

    // Get a list of all the modules in this process.
    if( HookHop::FunctionHop(EnumProcessModules, hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        moduleSymbols.clear();
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i )
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.
            if ( HookHop::FunctionHop(GetModuleFileNameEx, hProcess, hMods[i], szModName,
                sizeof(szModName) / sizeof(TCHAR)))
            {
                EnumModuleInfo modInfo;
                modInfo.moduleName = szModName;
                modInfo.handle     = hMods[i];

                GetModuleSymbols( modInfo.moduleName.mbc_str(), &(modInfo) );

                moduleSymbols.insert( modInfo ); // push back a copy
            }
            else
                wxMessageBox( "Failed to GetModuleFileNameEx", "ERROR");
        }
    }
    else
        wxMessageBox( "Failed to EnumProcessModules", "ERROR");

    // Release the handle to the process.
    HookHop::FunctionHop(CloseHandle, hProcess );

    return 0;
}

// list all symbols of the module passed in
void enumModulesDlg::GetModuleSymbols( PCSTR imageName, EnumModuleInfo *userContext )
{
    HANDLE hProcess = GetCurrentProcess(); // NOT safe to hookhop
    DWORD64 BaseOfDll;
    char *Mask = "*";
    BOOL status;

    status = (BOOL)HookHop::FunctionHop(SymInitialize, hProcess, NULL, FALSE);
    if (status == FALSE)
    {
        return;
    }

    BaseOfDll = SymLoadModuleEx(hProcess, // NOT safe to hookhop
        NULL,
        imageName,
        NULL,
        0,
        0,
        NULL,
        0);

    if (BaseOfDll == 0)
    {
        SymCleanup(hProcess);
        return;
    }

    if (::SymEnumSymbols( // One of the parameters is 64-bit, and so we can't use our hook hop
        hProcess,                // Process handle from SymInitialize.
        BaseOfDll,               // Base address of module.
        Mask,                    // Name of symbols to match.
        EnumSymProc,             // Symbol handler procedure.
        userContext))            // User context.
    {
        // SymEnumSymbols succeeded
    }
    else
    {
        // SymEnumSymbols failed
    }

    SymCleanup( hProcess ); // NOT safe to hookhop
}

// callback function used in ListModuleSymbols.  UserContext should be the pointer to an EnumModuleInfo
BOOL CALLBACK enumModulesDlg::EnumSymProc(
    PSYMBOL_INFO pSymInfo,
    ULONG SymbolSize,
    PVOID UserContext )
{
    EnumModulesSymbolInfo symInfo;
    symInfo.addr = pSymInfo->Address;
    symInfo.size = SymbolSize;
    symInfo.name = pSymInfo->Name;

    // get the correct address
    symInfo.addr = (DWORD)::GetProcAddress( 
        LoadLibraryA((reinterpret_cast<EnumModuleInfo *>(UserContext))->moduleName.mb_str() ), 
        symInfo.name.mb_str() );

    // if our address failed to be found, populate it with expected address
    if( symInfo.addr == 0 )
        symInfo.addr = pSymInfo->ModBase;

    // push back a copy of symInfo
    (reinterpret_cast<EnumModuleInfo *>(UserContext))->symInfoArr.insert(symInfo);
    return TRUE;
}

void enumModulesDlg::ListAllSymbols()
{
    wxTreeItemId rootId;

    TreeCtrl1->DeleteAllItems();

    rootId = TreeCtrl1->AddRoot(_("Root"));

    std::set<EnumModuleInfo>::iterator i = moduleSymbols.begin();
    for( ; i != moduleSymbols.end(); ++i )
    {
        wxString     childName = wxString::Format("%08X", i->handle) + _(" - ") + i->moduleName;
        wxTreeItemId childId   = TreeCtrl1->AppendItem(rootId, childName, -1, -1, NULL);

        std::set<EnumModulesSymbolInfo>::iterator j = i->symInfoArr.begin();
        for(; j != i->symInfoArr.end(); ++j )
        {
            wxString itemName = wxString::Format("%08X", j->addr) + _(" - ") +
                                j->name + _(" (") +
                                wxString::Format("%i", j->size) + _(")");
            TreeCtrl1->AppendItem(childId, itemName, -1, -1, NULL);
        }
    }
}

//TODO: Merge with GetSymbolsContaining a bit better
void enumModulesDlg::ListSymbolsContaining( wxString substr )
{
    const int MAX_RESULTS = 200; // number of results won't exceed this value
    wxTreeItemId rootId;

    TreeCtrl1->DeleteAllItems();

    rootId = TreeCtrl1->AddRoot(_("Root"));

    std::set<EnumModuleInfo>::iterator i = moduleSymbols.begin();
    for( int total=0; i != moduleSymbols.end() && total < MAX_RESULTS; ++i )
    {
        bool         childIsAppended = false; // keeps track of whether or not child has been appended to tree
        wxString     childName       = wxString::Format("%08X", i->handle) + _(" - ") + i->moduleName;
        wxTreeItemId childId;

        // append only the symbols/modules which contain a substring
        std::set<EnumModulesSymbolInfo>::iterator j = i->symInfoArr.begin();
        for( ; j != i->symInfoArr.end() && total < MAX_RESULTS; ++j )
        {
            wxString tempSearchStr = j->name;
            tempSearchStr.MakeUpper();
            wxString tempInputStr = (SearchCtrl1->GetValue());
            tempInputStr.MakeUpper();
            if( tempInputStr.size() > 0 && tempSearchStr.Find(tempInputStr) != -1) // search for substring
            {
                if( !childIsAppended ) // don't append child more than once!
                {
                    childId = TreeCtrl1->AppendItem(rootId, childName, -1, -1, NULL);
                    childIsAppended = true;
                }
                wxString itemName = wxString::Format("%08X", j->addr) + _(" - ") +
                    j->name + _(" (") +
                    wxString::Format("%i", j->size) + _(")");
                TreeCtrl1->AppendItem(childId, itemName, -1, -1, NULL);
                ++total;
            }
        }
    }
}

// returns a list of EnumModuleSymbolInfo which contain the substr
std::list<EnumModulesSymbolInfo> enumModulesDlg::GetSymbolsContaining( wxString substr, wxString moduleSubstr )
{
    GetAllModules( ::GetCurrentProcessId() ); // you must get all modules before getting symbols

    std::list<EnumModulesSymbolInfo> result;
    std::set<EnumModuleInfo>::iterator i = moduleSymbols.begin();
    for( ; i != moduleSymbols.end(); ++i )
    {
        if( i->moduleName.Upper().Find(moduleSubstr.Upper()) != wxNOT_FOUND )
        {
            wxString     childName = wxString::Format("%08X", i->handle) + _(" - ") + i->moduleName;
            wxTreeItemId childId;

            // append only the symbols/modules which contain a substring
            std::set<EnumModulesSymbolInfo>::iterator j = i->symInfoArr.begin();
            for( ; j != i->symInfoArr.end(); ++j )
            {
                wxString tempSearchStr = j->name;
                tempSearchStr.MakeUpper();
                wxString tempInputStr = substr;
                tempInputStr.MakeUpper();
                if( tempInputStr.size() > 0 && tempSearchStr.Find(tempInputStr) != -1) // search for substring
                {
                    result.push_back(*j); // if substring is found, push it to the back
                }
            }
        }
    }

    return result;
}

void enumModulesDlg::OnSearchCtrl1Text(wxCommandEvent& event)
{
    TreeCtrl1->DeleteAllItems();

    if( SearchCtrl1->GetValue().size() > 0 )
        ListSymbolsContaining( SearchCtrl1->GetValue() );
    else
        ListAllSymbols();
}
