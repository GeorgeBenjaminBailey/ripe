#include "stdafx.h"

#include "gameSelectMenu.h"

#include "../defs.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(gameSelectMenu)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(gameSelectMenu)
//*)

#include <wx/artprov.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/textdlg.h>

#include "../processutility.h"

//(*IdInit(gameSelectMenu)
const long gameSelectMenu::ID_STATICTEXT1 = wxNewId();
const long gameSelectMenu::ID_BUTTON3 = wxNewId();
const long gameSelectMenu::ID_BUTTON2 = wxNewId();
const long gameSelectMenu::ID_STATICTEXT2 = wxNewId();
const long gameSelectMenu::ID_STATICTEXT3 = wxNewId();
const long gameSelectMenu::ID_LISTCTRL1 = wxNewId();
const long gameSelectMenu::ID_LISTCTRL2 = wxNewId();
const long gameSelectMenu::ID_BUTTON1 = wxNewId();
const long gameSelectMenu::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(gameSelectMenu,wxDialog)
	//(*EventTable(gameSelectMenu)
	//*)
END_EVENT_TABLE()

gameSelectMenu::gameSelectMenu(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(gameSelectMenu)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Plugin Select Menu"), wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	SetFocus();
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(2);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Please make a selection from the following menu."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	Button3 = new wxButton(Panel1, ID_BUTTON3, _("Search For Plugin"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	Button3->SetMinSize(wxSize(-1,-1));
	Button3->SetToolTip(_("Opens the online plugin search dialog.  You can use this to find new plugins."));
	FlexGridSizer4->Add(Button3, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("Add Plugin"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	Button2->SetToolTip(_("Opens a menu to let you add a plugin to RiPE."));
	FlexGridSizer4->Add(Button2, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableCol(1);
	FlexGridSizer3->AddGrowableRow(1);
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Process"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer3->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Version"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer3->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ListCtrl1 = new wxListCtrl(Panel1, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER, wxDefaultValidator, _T("ID_LISTCTRL1"));
	ListCtrl1->SetMinSize(wxSize(160,160));
	ListCtrl1->InsertColumn( 0, wxT("Title"), 0, -1 );
	Init();
	FlexGridSizer3->Add(ListCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ListCtrl2 = new wxListCtrl(Panel1, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER, wxDefaultValidator, _T("ID_LISTCTRL2"));
	ListCtrl2->SetMinSize(wxSize(160,160));
	ListCtrl2->InsertColumn( 0, wxT("Title"), 0, -1 );
	DisplayPlugins();
	FlexGridSizer3->Add(ListCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("Continue"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON1"));
	Button1->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("RIGHTARROW_XPM")),wxART_BUTTON), wxRIGHT);
	FlexGridSizer2->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel1->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel1);
	FlexGridSizer2->SetSizeHints(Panel1);
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&gameSelectMenu::OnButton3Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&gameSelectMenu::OnButton2Click);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&gameSelectMenu::OnListCtrl1ItemSelect);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&gameSelectMenu::OnButton1Click);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&gameSelectMenu::OnClose);
	//*)

    //curGame.SetVersion( CUSTOM );
    curGame.SetVersion( OTHER_SEND );
    apDlg = new AddPluginDlg(this);
    psdDlg = NULL; // We're using lazy loading for this
}

gameSelectMenu::~gameSelectMenu()
{
	//(*Destroy(gameSelectMenu)
	//*)
    apDlg->Destroy();

    if( psdDlg != NULL )
        psdDlg->Destroy();
}

GameScriptVersion gameSelectMenu::GetScriptVersion(const int pluginIndex, const int scriptIndex)
{
    return gamePlugins[pluginIndex].scripts[scriptIndex].version;
}

const wxString & gameSelectMenu::GetScriptText(const int pluginIndex, const int scriptIndex) const
{
    return gamePlugins[pluginIndex].scripts[scriptIndex].script;
}

void gameSelectMenu::Init()
{
    LoadPlugins();
}

void gameSelectMenu::LoadDefaultPlugins()
{
    GamePlugin gp;

    // Public
    gp.scripts.clear();
    gp.name = "Other";
    gp.scripts.push_back( GameScript("All Send / Recv", OTHER_SENDALL) );
    gp.scripts.push_back( GameScript("Send / Recv", OTHER_SEND) );
    gp.scripts.push_back( GameScript("SendTo / RecvFrom", OTHER_SENDTO) );
    gp.scripts.push_back( GameScript("WSASend / WSARecv", OTHER_WSASEND) );
    gp.scripts.push_back( GameScript("WSASendTo / WSARecvFrom", OTHER_WSASENDTO) );
    gamePlugins.push_back( gp );
}

void gameSelectMenu::LoadCustomPlugins()
{
    wxString directoryName;
    wxString baseDirectory = GetCurrentProcessDirectory() + wxT("plugins\\");
    wxDir dir( baseDirectory );
    GamePlugin gp;

    if( dir.IsOpened() )
    {
        bool keepGoing = dir.GetFirst( &directoryName, wxT("*.*"), wxDIR_DIRS );

        while( keepGoing )
        {
            wxArrayString scriptFiles;
            wxDir gameDir(baseDirectory + directoryName);

            gp.scripts.clear(); // remove anything that shouldn't be there anymore
            gp.name = directoryName;

            if( gameDir.IsOpened() )
            {
                gameDir.GetAllFiles( gameDir.GetName(), &scriptFiles, _("*.*"), wxDIR_HIDDEN | wxDIR_FILES );

                // For each file
                for( wxArrayString::size_type j=0; j < scriptFiles.size(); ++j )
                {
                    wxString scriptName = scriptFiles[j].AfterLast('\\');
                    wxFFile scriptFile(scriptFiles[j]);
                    wxString scriptText;

                    scriptFile.ReadAll(&scriptText);
                    scriptFile.Close();

                    // Add the file and load its script
                    gp.scripts.push_back( GameScript(scriptName, scriptText) );
                }

                // If this game has anything to it, add the game plugin
                if( gp.scripts.size() > 0 )
                    gamePlugins.push_back( gp );
            }
            keepGoing = dir.GetNext( &directoryName );
        }
    }
}

void gameSelectMenu::LoadPlugins()
{
    gamePlugins.clear();

    LoadDefaultPlugins();
    LoadCustomPlugins();

    // Sort the game scripts
    if( !gamePlugins.empty() )
        sort(gamePlugins.begin(), gamePlugins.end(), gamePlugins[0]);

    // Sort the game plugins
    for( unsigned int i=0; i < gamePlugins.size(); ++i )
        if( !gamePlugins[i].scripts.empty() )
            sort(gamePlugins[i].scripts.begin(), gamePlugins[i].scripts.end(), gamePlugins[i].scripts[0]);
}

void gameSelectMenu::DisplayPlugins()
{
    // Clear the old rows
    ListCtrl1->DeleteAllItems();

    // For all plugins, add them
    for( unsigned int i=0; i < gamePlugins.size(); ++i )
    {
        ListCtrl1->InsertItem(i, gamePlugins[i].name);
        ListCtrl1->SetItemData(i, i); // Set the data to the index for gamePlugins
        if( gamePlugins[i].name == "Other" )
        {
            ListCtrl1->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
        }
    }
    ListCtrl1->SetColumnWidth(0, -1); // resize the column so everything fits

    DisplayScripts();
}

void gameSelectMenu::DisplayScripts()
{
    ListCtrl2->DeleteAllItems();

    if( ListCtrl1->GetSelectedItemCount() > 0 )
    {
        long item = ListCtrl1->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
        if( item != -1 ) // If there's a selected item
        {
            int gamePluginIndex = ListCtrl1->GetItemData(item);
            GamePlugin *gp = &gamePlugins[gamePluginIndex];

            // Load the scripts from the game plugin
            for( unsigned int i=0; i < gp->scripts.size(); ++i )
            {
                ListCtrl2->InsertItem(i, gp->scripts[i].name);
                ListCtrl2->SetItemData(i, i);
            }
        }
    }
    ListCtrl2->SetColumnWidth(0, -1); // resize the column so everything fits

    if( ListCtrl2->GetItemCount() > 0 )
        ListCtrl2->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
}

void gameSelectMenu::OnButton1Click(wxCommandEvent& event)
{
    UpdateGameVersion();
}

void gameSelectMenu::OnClose(wxCloseEvent& event)
{
    UpdateGameVersion();
}

// update the curGame and hide dialog on success
void gameSelectMenu::UpdateGameVersion()
{
    long pluginIndex = ListCtrl1->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    long scriptIndex = ListCtrl2->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    if( pluginIndex == -1 )
        wxMessageBox( _("Please select a game before continuing."), _("Error: No Game Selected"), wxICON_ERROR|wxOK );
    else if( scriptIndex == -1 )
        wxMessageBox( _("Please select a version for your\ngame before continuing."), _("Error: No Version Selected"), wxICON_ERROR|wxOK );
    else
    {
        GameScript *gs = &(gamePlugins[pluginIndex].scripts[scriptIndex]);
        curGame.SetVersion( gs->version );
        curGame.SetScriptName( gs->name );
        curGame.SetScriptText( gs->script );
        Hide();
    }
}

void gameSelectMenu::OnListCtrl1ItemSelect(wxListEvent& event)
{
    DisplayScripts();
}

// On add plugin
void gameSelectMenu::OnButton2Click(wxCommandEvent& event)
{
    if( apDlg->IsShown() )
        apDlg->Hide();
    else
    {
        Hide();
        apDlg->ShowModal();
        Show();

        // If plugin needs to be added
        if( apDlg->m_addScript )
        {
            // Get the filenames
            wxString pluginsLocation = GetCurrentProcessDirectory() + wxT("plugins");
            wxString gameName = apDlg->TextCtrlGame->GetValue();
            wxString gameVersion = apDlg->TextCtrlPlugin->GetValue();
            wxString filename = pluginsLocation + wxT("\\") + gameName + wxT("\\") + gameVersion;
            wxString oldFilename = apDlg->FilePickerCtrl1->GetFileName().GetFullPath();

            // Make necessary directories
            if( !wxDirExists(pluginsLocation) )
                wxMkdir( pluginsLocation );
            if( !wxDirExists(filename.BeforeLast('\\')) )
                wxMkdir( filename.BeforeLast('\\') );

            // Copy plugin to the directory
            wxCopyFile( oldFilename, filename, true );

            // Refresh the plugins
            LoadPlugins();
            DisplayPlugins();
        }

        ShowModal();
    }
}

// On plugin search dialog
void gameSelectMenu::OnButton3Click(wxCommandEvent& event)
{
    // Lazy loading
    if( psdDlg == NULL )
        psdDlg = new PluginSearchDlg(this);

    if( psdDlg->IsShown() )
        psdDlg->Hide();
    else
        psdDlg->Show();
}
