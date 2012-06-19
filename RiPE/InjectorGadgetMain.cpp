/***************************************************************
 * Name:      InjectorGadgetMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alexandria "Riukuzaki" Cornwall ()
 * Created:   2011-06-06
 * Copyright: Alexandria"Riukuzaki" Cornwall ()
 * License:
 **************************************************************/
#include "stdafx.h"

#include <list>
#include <map>

#include <boost/thread.hpp>

#include <Windows.h>
#include <psapi.h>

#pragma warning( push )
#pragma warning( disable : 4996 )
#include <wx/dnd.h>
#include <wx/msgdlg.h>

//(*InternalHeaders(InjectorGadgetFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "InjectorGadgetMain.h"

#include "inject.hpp"
#include "lasterror.h"
#include "MemoryModule.h"
#include "privilege.h"
#include "artProvider.h"

#pragma comment(lib, "Psapi.lib")

typedef void (__cdecl *SetFilename) ( const char *file );
typedef void (__cdecl *SetProcessID)( const DWORD processId );

namespace
{
    HMODULE dllHelperModule = LoadLibraryA("dll_helper.dll");//LoadLibraryA(p->fileName);
    SetFilename setFilename = (SetFilename)GetProcAddress(dllHelperModule, "SetFilename");
    SetProcessID setProcessID = (SetProcessID)GetProcAddress(dllHelperModule, "SetProcessID");

    // A helper class to store and close a HANDLE when it goes out of scope
    class HandleHelper
    {
    private:
        HANDLE m_hProcess;

    public:
        HandleHelper( HANDLE hProcess = NULL )
        {
            m_hProcess;
        }

        ~HandleHelper()
        {
            if( m_hProcess != NULL )
                CloseHandle( m_hProcess );
        }

        HANDLE GetHandle()
        {
            return m_hProcess;
        }

        void SetHandle( HANDLE hProcess )
        {
            if( m_hProcess != NULL && m_hProcess != hProcess )
                CloseHandle( m_hProcess );
            m_hProcess = hProcess;
        }

        HANDLE& operator= ( const HANDLE &hProcess )
        {
            if( m_hProcess != NULL && m_hProcess != hProcess )
                CloseHandle( m_hProcess );
            m_hProcess = hProcess;
            return m_hProcess;
        }

        HandleHelper& operator= ( const HandleHelper &handleHelper )
        {
            if( m_hProcess != NULL && m_hProcess != handleHelper.m_hProcess )
                CloseHandle( m_hProcess );
            m_hProcess = handleHelper.m_hProcess;
            return *this;
        }
    };

    // The time (in milliseconds) between process watch "pulses."
    // Lower values make it faster but require more CPU usage.
    // Default = 100.
    const int PROCESS_WATCHER_PULSE = 100;

    // stores all open process handles
    //std::map<DWORD, HandleHelper> processHandles;

    struct LoadRemoteLibParam {
        wxString fileName;
        unsigned long pId;
        bool cloak;

        LoadRemoteLibParam()
        {
            fileName = _("");
            pId      = 0;
            cloak    = false;
        }

        LoadRemoteLibParam( const wxString &fileNameStr,
            const unsigned long processId, const bool usingCloak )
        {
            fileName = fileNameStr;
            pId      = processId;
            cloak    = usingCloak;
        }
    };

    // Given a wxString
    void MemoryLoadRemoteLib( LPVOID lpParam )
    {
        if( lpParam != NULL ) {
            LoadRemoteLibParam *p = (LoadRemoteLibParam *)lpParam;

            FILE *fp;
            unsigned char *data = NULL;
            size_t size = 0;
            HMEMORYMODULE module;
            fp = fopen(p->fileName, "rb");
            if( fp == NULL ) {
                PrintLastError( L"Failed to open file with fopen.\n"
                    L"Ensure that the DLL path is valid." );
                return;
            }
            fseek( fp, 0, SEEK_END );
            size = ftell( fp );
            data = new unsigned char[size];
            fseek( fp, 0, SEEK_SET );
            fread( data, 1, size, fp );
            fclose( fp );

            HANDLE hProcess = NULL;
            //if( processHandles.find(p->pId) != processHandles.end() )
            //    hProcess = processHandles[p->pId].GetHandle();
            //else
            {
                hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, p->pId );
                //processHandles[p->pId] = HandleHelper(hProcess);
            }

            module = MemoryLoadRemoteLibrary( hProcess, p->pId, data, p->cloak );
            delete [] data;
            CloseHandle( hProcess );
        }
    }

    void LoadRemoteLibA( LoadRemoteLibParam * lpParam )
    {
        if( lpParam != NULL ) {
            LoadRemoteLibParam *p = (LoadRemoteLibParam *)lpParam;
            bool succeeded = false;

            // open the target process
            HANDLE hProcess = NULL;
            //if( processHandles.find(p->pId) != processHandles.end() )
            //    hProcess = processHandles[p->pId].GetHandle();
            //else
            {
                hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, p->pId );
                //processHandles[p->pId] = HandleHelper(hProcess);
            }
            if( hProcess != NULL )
            {
                SIZE_T numberOfBytesRead;

                if( ReadProcessMemory(hProcess, (LPCVOID)0x00400000, NULL, 0, &numberOfBytesRead) )
                    succeeded = LoadRemoteLibraryA( hProcess, p->fileName ) != NULL;
                CloseHandle( hProcess );
            }

            // it LoadRemoteLibraryA failed
            if( !succeeded )
            {
//                 HANDLE hThread = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, p->pId );
//                 if( hThread != INVALID_HANDLE_VALUE )
//                 {
//                     THREADENTRY32 te;
//                     te.dwSize = sizeof(te);
//                     if( Thread32First(hThread, &te) )
//                         if( te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32ThreadID) + sizeof(te.th32ThreadID) )
//                         {
                            setFilename = (SetFilename)GetProcAddress(dllHelperModule, "SetFilename");
                            setProcessID = (SetProcessID)GetProcAddress(dllHelperModule, "SetProcessID");
                            if( setFilename == 0 || setProcessID == 0 ) {
                                wxMessageBox("I tried to inject your DLL, but it failed.  I then tried looking for dll_helper.dll, but it was either not found or was invalid.  I give up.  :(", "I'm sorry");
                                delete p;
                                return;
                            }
                            setFilename( p->fileName.mbc_str() );
                            setProcessID( p->pId );
                            HHOOK hHook = SetWindowsHookExA( WH_GETMESSAGE, (HOOKPROC)GetProcAddress(dllHelperModule, "_CallWndProc@12"), dllHelperModule, 0 );
                            if( hHook != NULL )
                            {
                                succeeded = true;
                                Sleep(15000);
                                UnhookWindowsHookEx( hHook );
                            }
                       // }
                //}
                if( !succeeded )
                    PrintLastError( L"Failed on both injection routines." );
            }
            delete p;
        }
    }

    class DnDFile : public wxFileDropTarget
    {
    private:
        wxListView *m_pOwner;

    public:
        DnDFile( wxListView *pOwner = NULL )
        {
            m_pOwner = pOwner;
        }

        virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString &filenames )
        {
            if( m_pOwner != NULL ) {
                // For all files
                for( size_t i=0; i < filenames.GetCount(); ++i ) {
                    const wxString fileName = filenames[i].AfterLast('\\');
                    const wxString fullPath = filenames[i];

                    // Check if it already exists
                    int index = m_pOwner->GetNextItem(-1);
                    bool alreadyExists = false;
                    for( ; index != -1; index = m_pOwner->GetNextItem(index) ) {
                        // If it already exists
                        if( *(wxString *)m_pOwner->GetItemData(index) == fullPath ) {
                            alreadyExists = true;
                            break;
                        }
                    }

                    // If it doesn't already exist, add it
                    if( !alreadyExists ) {
                        index = m_pOwner->GetItemCount();

                        index = m_pOwner->InsertItem(index,
                            fullPath.BeforeLast('\\').AfterLast('\\') + _("\\") + fileName);
                        m_pOwner->SetItemPtrData(index, (wxUIntPtr)new wxString(fullPath));
                    }
                }
            }
            return true;
        }
    };
}

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(InjectorGadgetFrame)
const long InjectorGadgetFrame::ID_LISTVIEW1 = wxNewId();
const long InjectorGadgetFrame::ID_BUTTON1 = wxNewId();
const long InjectorGadgetFrame::ID_HTMLWINDOW = wxNewId();
const long InjectorGadgetFrame::ID_PANEL2 = wxNewId();
const long InjectorGadgetFrame::ID_PANEL1 = wxNewId();
const long InjectorGadgetFrame::idMenuQuit = wxNewId();
const long InjectorGadgetFrame::idMenuAbout = wxNewId();
const long InjectorGadgetFrame::ID_STATUSBAR1 = wxNewId();
const long InjectorGadgetFrame::ID_TIMER2 = wxNewId();
const long InjectorGadgetFrame::ID_TIMER3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(InjectorGadgetFrame,wxDialog)
    //(*EventTable(InjectorGadgetFrame)
    //*)
END_EVENT_TABLE()

InjectorGadgetFrame::InjectorGadgetFrame(wxWindow* parent,wxWindowID id)
{
    wxInitAllImageHandlers();
    wxArtProvider::Push(new RiuArtProvider);
    //(*Initialize(InjectorGadgetFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer5;
    wxFlexGridSizer* FlexGridSizer2;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxFlexGridSizer* FlexGridSizer1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Select Process"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetIcon(wxIcon(wxT("aaaa"), wxBITMAP_TYPE_ICO_RESOURCE));
    FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(0);
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizer2->AddGrowableCol(0);
    FlexGridSizer2->AddGrowableRow(0);
    FlexGridSizer5 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer5->AddGrowableCol(0);
    FlexGridSizer5->AddGrowableRow(0);
    ListView1 = new wxSortingListView(Panel1, ID_LISTVIEW1, wxDefaultPosition, wxSize(250,280), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW1"));
    FlexGridSizer5->Add(ListView1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("Inject"), wxDefaultPosition, wxSize(250,-1), wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON1"));
    Button1->SetToolTip(_("Injects the DLL into the specified process."));
    Button1->SetHelpText(_("Injects the DLL into the specified process."));
    Button1->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("INJECT_XPM")),wxART_BUTTON), wxRIGHT);
    FlexGridSizer5->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(FlexGridSizer5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel1->SetSizer(FlexGridSizer2);
    FlexGridSizer2->Fit(Panel1);
    FlexGridSizer2->SetSizeHints(Panel1);
    FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(FlexGridSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
//    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
//    SetStatusBar(StatusBar1);
    TimerProcessWatcher.SetOwner(this, ID_TIMER2);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, _("*.dll"), wxFD_DEFAULT_STYLE|wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    TimerRefresh.SetOwner(this, ID_TIMER3);
    TimerRefresh.Start(2000, false);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_LISTVIEW1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&InjectorGadgetFrame::OnListView1ItemSelect);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&InjectorGadgetFrame::OnButton1Click);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&InjectorGadgetFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&InjectorGadgetFrame::OnAbout);
    Connect(ID_TIMER2,wxEVT_TIMER,(wxObjectEventFunction)&InjectorGadgetFrame::OnTimerProcessWatcherTrigger);
    Connect(ID_TIMER3,wxEVT_TIMER,(wxObjectEventFunction)&InjectorGadgetFrame::OnTimerRefreshTrigger);
    //*)

    UpdateRiPE();

    HANDLE hToken = NULL;
    if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ) {
        PrintLastError( L"OpenProcessToken" );
    }
    if( !SetPrivilege( hToken, SE_DEBUG_NAME, TRUE ) ) {
        PrintLastError( L"SetPrivilege" );
    }
    CloseHandle( hToken );

    pId = 0;
    ListView1->InsertColumn( 0, _("Process"), 0, 170 );
    ListView1->InsertColumn( 1, _("PID"), 0, 60 );
    ListProcesses( ListView1 );
}

InjectorGadgetFrame::~InjectorGadgetFrame()
{
    //(*Destroy(InjectorGadgetFrame)
    //*)
}

void InjectorGadgetFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void InjectorGadgetFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = _("Created by Riukuzaki\n\n ");
    wxMessageBox(msg, _("RiPE Launcher v2012-02-06"));
}

// Given a process ID, inject into that process
bool InjectorGadgetFrame::DoInjectIntoProcess( const DWORD pId )
{
    bool result = true;
    LoadRemoteLibParam *p = new LoadRemoteLibParam();
    p->fileName = wxGetCwd() + "\\" + wxT("ripe_dll.dll");
    p->pId      = pId;
    p->cloak    = false;
    
    if( wxFileExists(p->fileName) ) {
        boost::thread thread(boost::bind(LoadRemoteLibA,p));
    } else {
        wxMessageBox("RiPE was unable to find ripe_dll.dll.  Make sure it's in the same directory.", "Unable to inject into process", wxICON_ERROR|wxOK);
        result = false;
    }
    return result;
}

// Inject
void InjectorGadgetFrame::OnButton1Click(wxCommandEvent& event)
{
    if( pId > 0 ) {
        if (DoInjectIntoProcess( pId )) {
            this->Hide(); // Hide ourselves after injection
        }
    }
    else if( pId == 0 )
        wxMessageBox( _("You cannot inject into a PID of 0.  Please select a process to inject into from the list on the left."),
                     _("Unable to inject into process"), wxOK|wxICON_ERROR );
}

// OnRefreshButton
void InjectorGadgetFrame::OnButton2Click(wxCommandEvent& event)
{
    ListProcesses( ListView1 );
}

// Given a control, list the processes
void InjectorGadgetFrame::ListProcesses( wxSortingListView *list )
{
    std::set<DWORD> curProcesses;
    // Get the list of process identifiers.
    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hSnapshot == NULL ) {
        PrintLastError( L"hSnapshot NULL after call to CreateToolhelp32Snapshot.\nUnable to enumerate processes." );
    } else {
        // set the size of the struct before using it
        pe32.dwSize = sizeof( PROCESSENTRY32 );

        // Retrieve information about the first process.
        // Exit if unsuccessful.
        if( !Process32First( hSnapshot, &pe32 ) ) {
            PrintLastError( L"Unable to walk through the processes after call to Process32First.\nUnable to enumerate processes." );
            CloseHandle( hSnapshot );
            return;
        }

        // Add all of the processes to the list
        do {
            InsertProcessNameAndID( pe32, list );
            curProcesses.insert(pe32.th32ProcessID);
        } while( Process32Next( hSnapshot, &pe32 ) );

        // Run through the list and remove processes that don't exist
        long index = list->GetNextItem(-1);
        long prevIndex = -1;
        for( ; index != -1; index = list->GetNextItem(index) ) {
            // If item no longer exists, delete it
            DWORD pId = wxAtoi(list->GetItemText(index,1));
            if( curProcesses.find(pId) == curProcesses.end() ) {
                list->DeleteItem(index);

                // Free up process handles
                //if( processHandles.find(pId) != processHandles.end() )
                //    processHandles.erase( pId );

                index = prevIndex;
            }
            prevIndex = index;
        }

        CloseHandle( hSnapshot );
        list->Sort();
    }
}

void InjectorGadgetFrame::InsertProcessNameAndID( const PROCESSENTRY32 pe32, wxSortingListView *list )
{
    bool hasIcon = false;

    // don't enumerate the system process, and don't insert the same process twice
    if( pe32.th32ProcessID != 0
        && list->FindItem(wxString::Format("%i", pe32.th32ProcessID), 1) == -1 ) {
        //long index = list->GetItemCount();
        long index = list->GetIndexFromOrder(pe32.th32ProcessID, pe32.szExeFile);
        index = list->InsertItem(index, wxString(pe32.szExeFile) );
        list->SetItem( index, 1, wxString::Format("%i", pe32.th32ProcessID) );

        // If 32-bit process, get the first process module so we can get the
        // full file path.
        HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
        MODULEENTRY32 me32;

        // Take a snapshot of all modules in the specified process.
        hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pe32.th32ProcessID );

        // If a valid module handle, then let's continue trying to add the icon
        if( hModuleSnap != INVALID_HANDLE_VALUE ) {
            // Set the size of the structure before using it.
            me32.dwSize = sizeof( MODULEENTRY32 );

            // Retrieve information from the first module.
            if( Module32First( hModuleSnap, &me32 ) ) {
                HICON hIcon = ExtractIcon( GetModuleHandle(0), me32.szExePath, 0 );
                if( hIcon != NULL ) {
                    wxIcon icon;
                    icon.SetHICON( hIcon );
                    list->SetItemIcon( index, icon, me32.szExePath );
                    hasIcon = true;

                    // Clean-up
                    DestroyIcon( hIcon );
                }
            }

            // Clean-up the handle
            CloseHandle( hModuleSnap );
        }

        // If it doesn't have an icon, set it to the default
        if( !hasIcon )
            list->SetItemIconDefault( index );
        list->SetItemData(index, pe32.th32ProcessID);
        HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, TRUE, pe32.th32ProcessID );
//         if( hProcess != NULL ) {
//             processHandles[pe32.th32ProcessID].SetHandle(hProcess);
//         }
    }
}

// Updates RiPE
void InjectorGadgetFrame::UpdateRiPE()
{

}

// Process / PID view
void InjectorGadgetFrame::OnListView1ItemSelect(wxListEvent& event)
{
    pId = wxAtoi(ListView1->GetItemText(event.GetIndex(), 1));
}

// When it's time to update the process list and see if we need to inject
void InjectorGadgetFrame::OnTimerProcessWatcherTrigger(wxTimerEvent& event)
{
    // Only update if this dialog is being shown
    if (this->IsShown()) {
        std::list<PROCESSENTRY32> pe32List;
        std::list<PROCESSENTRY32>::const_iterator iter;
        bool didInjection = false;

        // Create a snapshot
        PROCESSENTRY32 pe32;
        HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
        if( hSnapshot == NULL )
        {
            PrintLastError( L"hSnapshot NULL after call to CreateToolhelp32Snapshot.\nUnable to enumerate processes." );
            return;
        }

        // set the size of the struct before using it
        pe32.dwSize = sizeof( PROCESSENTRY32 );

        // Retrieve information about the first process.
        // Exit if unsuccessful.
        if( !Process32First( hSnapshot, &pe32 ) )
        {
            PrintLastError( L"Unable to walk through the processes after call to Process32First.\nUnable to enumerate processes." );
            CloseHandle( hSnapshot );
            return;
        }

        // Populate the current process set with the current process IDs
        do
        {
            pe32List.push_back( pe32 );
        } while( Process32Next( hSnapshot, &pe32 ) );

        // Clean-up
        CloseHandle( hSnapshot );
    }
}

void InjectorGadgetFrame::OnTimerRefreshTrigger(wxTimerEvent& event)
{
    ListProcesses( ListView1 );
}

/**
 * Returns the process ID of the last process that was injected into
 */
int InjectorGadgetFrame::GetPID() {
    return pId;
}

#pragma warning( pop )
