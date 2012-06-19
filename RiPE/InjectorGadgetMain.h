/***************************************************************
 * Name:      InjectorGadgetMain.h
 * Purpose:   Defines Application Frame
 * Author:    Ryan "Riukuzaki" Cornwall ()
 * Created:   2011-06-06
 * Copyright: Ryan "Riukuzaki" Cornwall ()
 * License:
 **************************************************************/

#ifndef INJECTORGADGETMAIN_H
#define INJECTORGADGETMAIN_H

#include <set>

#include <Windows.h>
#include <TlHelp32.h>

//(*Headers(InjectorGadgetFrame)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include <WinInet.h>

// webview support
#if defined(__WXMSW__)
#include "webview/wxIETest2/wxiepanel.h"
#elif defined (__WXOSX__)
#include "webview/wxOSXWebkitCtrl/webkit.h"
#elif defined (__WXGTK__)
#include "webview/wxGtkWebKitCtrl/wxgtkwebkitctrl.h"
#else
#error "Sorry, your platform is not supported"
#endif
#include "webview/webview.h"

#include "adBrowserDlg.h"
#include "sortinglistview.h"

class InjectorGadgetFrame: public wxDialog
{
    public:
        InjectorGadgetFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~InjectorGadgetFrame();
        int GetPID();

    private:
        //(*Handlers(InjectorGadgetFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnListView1ItemSelect(wxListEvent& event);
        void OnCheckBox3Click(wxCommandEvent& event);
        void OnTimerProcessWatcherTrigger(wxTimerEvent& event);
        void OnListView2KeyDown(wxListEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        void OnButton4Click(wxCommandEvent& event);
        void OnTimerRefreshTrigger(wxTimerEvent& event);
        //*)
        void OnListView1Sort(wxCommandEvent& event);

        //Functions
        void DoDeleteSelected( wxListCtrl *ctrl );
        bool DoInjectIntoProcess( const DWORD pId );
        void ListProcesses( wxSortingListView *list );
        void InsertProcessNameAndID( const PROCESSENTRY32 pe32, wxSortingListView *list );
        void UpdateRiPE();

        //(*Identifiers(InjectorGadgetFrame)
        static const long ID_LISTVIEW1;
        static const long ID_BUTTON1;
        static const long ID_HTMLWINDOW;
        static const long ID_PANEL2;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER2;
        static const long ID_TIMER3;
        //*)

        //(*Declarations(InjectorGadgetFrame)
        wxTimer TimerProcessWatcher;
        wxButton* Button1;
        wxPanel* Panel1;
        wxFileDialog* FileDialog1;
        wxSortingListView* ListView1;
        wxStatusBar* StatusBar1;
        wxTimer TimerRefresh;
        //*)
        std::set<DWORD> processSet;
        unsigned int pId;

        DECLARE_EVENT_TABLE()
};

#endif // INJECTORGADGETMAIN_H
