#include "stdafx.h"

#include "PluginSearchDlg.h"

//(*InternalHeaders(PluginSearchDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/ffile.h>

#include "RiPE/RiPEApp.h"
#include "RiPE.h"
#include "processutility.h" // used to get the process directory
#include "StringMangler.h"
#include "webutility.h" // used for QueryWebPage and other web-based access

//(*IdInit(PluginSearchDlg)
const long PluginSearchDlg::ID_BUTTON3 = wxNewId();
const long PluginSearchDlg::ID_SEARCHCTRL1 = wxNewId();
const long PluginSearchDlg::ID_RADIOBUTTON1 = wxNewId();
const long PluginSearchDlg::ID_RADIOBUTTON2 = wxNewId();
const long PluginSearchDlg::ID_TREECTRL1 = wxNewId();
const long PluginSearchDlg::ID_BUTTON2 = wxNewId();
const long PluginSearchDlg::ID_BUTTON1 = wxNewId();
const long PluginSearchDlg::ID_STATICTEXT1 = wxNewId();
const long PluginSearchDlg::ID_STATICTEXT2 = wxNewId();
const long PluginSearchDlg::ID_STATICLINE1 = wxNewId();
const long PluginSearchDlg::ID_STATICTEXT3 = wxNewId();
const long PluginSearchDlg::ID_STATICTEXT4 = wxNewId();
const long PluginSearchDlg::ID_CUSTOM1 = wxNewId();
const long PluginSearchDlg::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PluginSearchDlg,wxDialog)
	//(*EventTable(PluginSearchDlg)
	//*)
END_EVENT_TABLE()

PluginSearchDlg::PluginSearchDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(PluginSearchDlg)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, _("Online Plugin Search"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(-1,-1));
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Panel1->SetMinSize(wxSize(-1,-1));
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableCol(1);
	FlexGridSizer2->AddGrowableRow(0);
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableRow(3);
	Button3 = new wxButton(Panel1, ID_BUTTON3, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	Button3->SetToolTip(_("Refreshes available scripts / plugins."));
	FlexGridSizer3->Add(Button3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SearchCtrlScripts = new wxSearchCtrl(Panel1, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	FlexGridSizer3->Add(SearchCtrlScripts, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer6 = new wxFlexGridSizer(0, 3, 0, 0);
	RadioButtonScripts = new wxRadioButton(Panel1, ID_RADIOBUTTON1, _("Scripts"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	RadioButtonScripts->SetValue(true);
	FlexGridSizer6->Add(RadioButtonScripts, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	RadioButtonPlugins = new wxRadioButton(Panel1, ID_RADIOBUTTON2, _("Plugins"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	FlexGridSizer6->Add(RadioButtonPlugins, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TreeCtrl1 = new wxTreeCtrl(Panel1, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
	FlexGridSizer3->Add(TreeCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("Upload Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	Button2->SetToolTip(_("Opens a dialog for uploading a script that you\'ve written."));
	FlexGridSizer3->Add(Button2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer4->AddGrowableRow(1);
	FlexGridSizer5 = new wxFlexGridSizer(0, 6, 0, 0);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("Add Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Button1->Disable();
	Button1->SetToolTip(_("Adds the script to your RiPE."));
	FlexGridSizer5->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Author:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer5->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticTextAuthor = new wxStaticText(Panel1, ID_STATICTEXT2, _("Anon"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer5->Add(StaticTextAuthor, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine1 = new wxStaticLine(Panel1, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_VERTICAL, _T("ID_STATICLINE1"));
	FlexGridSizer5->Add(StaticLine1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Downloads:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer5->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticTextDownloads = new wxStaticText(Panel1, ID_STATICTEXT4, _("0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer5->Add(StaticTextDownloads, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4->Add(FlexGridSizer5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Script = new AsmEditor(Panel1,ID_CUSTOM1,wxDefaultPosition,wxDefaultSize,0,_T("ID_CUSTOM1"));
	FlexGridSizer4->Add(Script, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer4, 3, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel1->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel1);
	FlexGridSizer2->SetSizeHints(Panel1);
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PluginSearchDlg::OnButton3Click);
	Connect(ID_SEARCHCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&PluginSearchDlg::OnSearchCtrlScriptsText);
	Connect(ID_SEARCHCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&PluginSearchDlg::OnSearchCtrlScriptsText);
	Connect(ID_SEARCHCTRL1,wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN,(wxObjectEventFunction)&PluginSearchDlg::OnSearchCtrlScriptsText);
	Connect(ID_RADIOBUTTON1,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&PluginSearchDlg::OnRadioButtonScriptsSelect);
	Connect(ID_RADIOBUTTON2,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&PluginSearchDlg::OnRadioButtonPluginsSelect);
	Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&PluginSearchDlg::OnTreeCtrl1SelectionChanged);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PluginSearchDlg::OnButton2Click);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PluginSearchDlg::OnButton1Click);
	//*)

    LoadAllScripts();
    LoadAllPlugins();
    LoadFromMap(scripts);

    usDlg = NULL;
}

PluginSearchDlg::~PluginSearchDlg()
{
	//(*Destroy(PluginSearchDlg)
	//*)
    if( usDlg != NULL )
        usDlg->Destroy();
}

/************************************************************************/
/* Loads all scripts from the server.                                   */
/************************************************************************/
void PluginSearchDlg::LoadAllScripts()
{
    // Query server
    wxString data = QueryWebPage( StringMangler::Decode("u\"\"}G<<%%%; v#x#(nxv;p|z<}u}< v}r!\"<tr\"!p v}\"!;}u}") );
    wxString line = "";

    scripts.clear();
    while( (line = data.BeforeFirst('\n')).length() > 0 )
    {
        wxString processName = line.BeforeFirst('|');
        wxString scriptName;
        line = line.AfterFirst('|');

        // Add the process
        scripts[processName];

        // For all scripts in a process, add them
        while( (scriptName = line.BeforeFirst('|')).length() > 0 )
        {
            // Add the script
            scripts[processName][scriptName];

            line = line.AfterFirst('|');
        }

        data = data.AfterFirst('\n');
    }
}

/************************************************************************/
/* Loads all plugins from the server.                                   */
/************************************************************************/
void PluginSearchDlg::LoadAllPlugins()
{
    // Query server
    wxString data = QueryWebPage( StringMangler::Decode("u\"\"}G<<%%%; v#x#(nxv;p|z<}u}< v}r!\"<tr\"}y#tv{!;}u}") );
    wxString line = "";

    plugins.clear();
    // For each line, add it to the tree
    while( (line = data.BeforeFirst('\n')).length() > 0 )
    {
        wxString processName = line.BeforeFirst('|');
        wxString scriptName;
        line = line.AfterFirst('|');

        // Add the process
        plugins[processName];

        // For all scripts in a process, add them
        while( (scriptName = line.BeforeFirst('|')).length() > 0 )
        {
            // Add the script
            plugins[processName][scriptName];

            line = line.AfterFirst('|');
        }

        data = data.AfterFirst('\n');
    }
}

/************************************************************************/
/* Loads the selected plugin or script.  This loads the actual script   */
/* data.                                                                */
/************************************************************************/
/*void PluginSearchDlg::LoadSelection()
{

}
*/
void PluginSearchDlg::OnRadioButtonScriptsSelect(wxCommandEvent& event)
{
    LoadFromMap( scripts );
}

void PluginSearchDlg::OnRadioButtonPluginsSelect(wxCommandEvent& event)
{
    LoadFromMap( plugins );
}

void PluginSearchDlg::LoadFromMap( std::map<wxString, std::map<wxString,
        ScriptNode> > &scriptMap )
{
    wxTreeItemId rootId;
    wxTreeItemId processId;
    std::map<wxString, std::map<wxString, ScriptNode> >::iterator iter;

    // Reset the current script
    Script->ClearAll();
    StaticTextAuthor->SetLabel("Anon");
    StaticTextDownloads->SetLabel("0");

    // Reset the tree
    TreeCtrl1->DeleteAllItems();
    rootId = TreeCtrl1->AddRoot(wxT("Root"));

    iter = scriptMap.begin();

    // Add all processes
    for( ; iter != scriptMap.end(); ++iter )
    {
        std::map<wxString, ScriptNode>::iterator childIter = iter->second.begin();

        processId = TreeCtrl1->AppendItem(rootId, iter->first);

        // Add all children of the process (e.g. scripts / versions)
        for( ; childIter != iter->second.end(); ++childIter )
        {
            TreeCtrl1->AppendItem(processId, childIter->first);
        }
    }
}

void PluginSearchDlg::OnSearchCtrlScriptsText(wxCommandEvent& event)
{
    wxTreeItemId rootId;
    wxTreeItemId processId;
    std::map<wxString, std::map<wxString, ScriptNode> >::iterator iter;
    std::map<wxString, std::map<wxString, ScriptNode> > *scriptMap;

    // Reset the tree
    TreeCtrl1->DeleteAllItems();
    rootId = TreeCtrl1->AddRoot(wxT("Root"));

    // If scripts are being searched
    if( RadioButtonScripts->GetValue() )
    {
        scriptMap = &scripts;
    }
    // If plugins are being searched
    else //if( RadioButtonPlugins->GetValue() )
    {
        scriptMap = &plugins;
    }

    iter = scriptMap->begin();

    // Add all processes
    for( ; iter != scriptMap->end(); ++iter )
    {
        // If the process matches the search string
        wxString processLower = iter->first.Lower();
        wxString searchLower  = SearchCtrlScripts->GetValue().Lower();
        if( processLower.StartsWith(searchLower) )
        {
            std::map<wxString, ScriptNode>::iterator childIter = iter->second.begin();

            processId = TreeCtrl1->AppendItem(rootId, iter->first);

            // Add all children of the process (e.g. scripts / versions)
            for( ; childIter != iter->second.end(); ++childIter )
            {
                TreeCtrl1->AppendItem(processId, childIter->first);
            }
        }
    }
}

// Queries a script from the server, returning useful information in the node
void PluginSearchDlg::QueryScript( const wxString &processName,
        const wxString &scriptName, ScriptNode &node )
{
    wxString response = QueryWebPage(
            StringMangler::Decode("u\"\"}G<<%%%; v#x#(nxv;p|z<}u}< v}r!\"<~#r '!p v}\";}u}L} |pr!![nzrJ")
            + processName + StringMangler::Decode("3!p v}\"[nzrJ") + scriptName );

    node.exists = true;
    node.author = response.BeforeFirst('\n');
    response = response.AfterFirst('\n');
    node.downloads = response.BeforeFirst('\n');
    node.script = response.AfterFirst('\n');
}

// Displays detailed information about a node
void PluginSearchDlg::DisplayNode( ScriptNode &node )
{
    Script->ClearAll();
    Script->SetValue(node.script);
    StaticTextAuthor->SetLabel(node.author);
    StaticTextDownloads->SetLabel(node.downloads);
}

// When a script / version is selected, display it
void PluginSearchDlg::OnTreeCtrl1SelectionChanged(wxTreeEvent& event)
{
    std::map<wxString, std::map<wxString, ScriptNode> > *scriptMap;

    // If scripts are being searched
    if( RadioButtonScripts->GetValue() )
    {
        scriptMap = &scripts;
    }
    // If plugins are being searched
    else //if( RadioButtonPlugins->GetValue() )
    {
        scriptMap = &plugins;
    }

    wxTreeItemId id = TreeCtrl1->GetSelection();
    wxTreeItemId parentId = TreeCtrl1->GetItemParent( id );

    // If we've selected a script / version
    if( parentId && TreeCtrl1->GetItemParent(parentId) )
    {
        wxString scriptName = TreeCtrl1->GetItemText(id);
        wxString processName = TreeCtrl1->GetItemText(parentId);
        ScriptNode *node = &(*scriptMap)[processName][scriptName];

        // If this script / version has not been queried
        if( !node->exists )
        {
            QueryScript( processName, scriptName, *node );
        }

        DisplayNode(*node);
        curScriptName = scriptName;
        curProcessName = processName;
        Button1->Enable(); // Enable the Add Script button
    }
}

// On "Add" script
void PluginSearchDlg::OnButton1Click(wxCommandEvent& event)
{
    if (Script->GetTextLength() > 0) {
        // If a script
        if( RadioButtonScripts->GetValue() )
        {
            AddScript();
        }
        // If a plugin
        else //if( RadioButtonPlugins->GetValue() )
        {
            AddPlugin();
        }
    } else {
        wxMessageBox("Cannot add an empty script or plugin.", "Error", wxICON_ERROR|wxOK);
    }
}

void PluginSearchDlg::AddScript()
{
    // No script has been selected yet!
    if( curProcessName == wxEmptyString || curScriptName == wxEmptyString )
        return;
    wxString curDir = GetCurrentProcessDirectory();
    wxString scriptName = curProcessName + "\\" + curScriptName;
    RiPEApp::GetRiPE()->communicationLayer.DeleteScript( scriptName );
    RiPEApp::GetRiPE()->communicationLayer.RemoveScript( scriptName );

    // Tell the server you're adding the plugin/script
    QueryWebPage( StringMangler::Decode("u\"\"}G<<%%%; v#x#(nxv;p|z<}u}< v}r!\"<q|%{y|nq!p v}\";}u}L")
        + _("gameName=") + URLEncode(curProcessName) + wxT("&scriptName=")
        + URLEncode(curScriptName) );

    if( !wxDirExists(curDir + "scripts") )
    {
        wxMkdir( curDir + "scripts" );
    }

    // Create the script in the scripts directory
    if( !wxDirExists(curDir + "scripts\\" + curProcessName) )
    {
        wxMkdir( curDir + "scripts\\" + curProcessName );
    }

    // Add the script to the script manager
    RiPEApp::GetRiPE()->communicationLayer.AddScript( scriptName,
        Script->GetText() );

    RiPEApp::GetRiPE()->communicationLayer.SaveScripts();

    // Add the script to the check list box
    bool alreadyExists = false;
    wxArrayString scriptNames = RiPEApp::GetRiPE()->CheckListBox1->GetStrings();
    for( size_t i=0; i < scriptNames.GetCount() && !alreadyExists; ++i )
    {
        if( scriptNames[i] == scriptName )
        {
            alreadyExists = true;
        }
    }

    if( !alreadyExists )
    {
        RiPEApp::GetRiPE()->CheckListBox1->Insert( 1, &scriptName,
            RiPEApp::GetRiPE()->CheckListBox1->GetCount() );
    }
    wxMessageBox("The script has been added to your RiPEST tab.");
}

void PluginSearchDlg::AddPlugin()
{
    // No script has been selected yet!
    if( curProcessName == wxEmptyString || curScriptName == wxEmptyString )
        return;
    wxString curDir = GetCurrentProcessDirectory();
    wxString scriptName = curProcessName + "\\" + curScriptName;
    RiPEApp::GetRiPE()->communicationLayer.DeleteScript( scriptName );
    RiPEApp::GetRiPE()->communicationLayer.RemoveScript( scriptName );

    // Tell the server you're adding the plugin/script
    QueryWebPage( StringMangler::Decode("u\"\"}G<<%%%; v#x#(nxv;p|z<}u}< v}r!\"<q|%{y|nq!p v}\";}u}L")
        + _("gameName=") + URLEncode(curProcessName) + wxT("&scriptName=")
        + URLEncode(curScriptName) );

    if( !wxDirExists(curDir + "plugins") )
    {
        wxMkdir( curDir + "plugins" );
    }

    // Create the script in the scripts directory
    if( !wxDirExists( curDir + "plugins\\" + curProcessName) )
    {
        wxMkdir( curDir + "plugins\\" + curProcessName );
    }

    // Make file
    wxFFile file;
    file.Open(curDir + wxT("plugins/") + curProcessName + "/" + curScriptName, "w");
    file.Write(Script->GetText());
    file.Close();

    // Add plugin to game select menu
    if( RiPEApp::GetRiPE()->gsmDlg != NULL )
    {
        RiPEApp::GetRiPE()->gsmDlg->LoadPlugins();
        RiPEApp::GetRiPE()->gsmDlg->DisplayPlugins();
    }
    wxMessageBox("The plugin has been added to your Game Select dialog.");
}

void PluginSearchDlg::OnButton2Click(wxCommandEvent& event)
{
    if( usDlg == NULL )
        usDlg = new UploadScriptDlg(this);
    if( usDlg->IsShown() )
        usDlg->Hide();
    else
        usDlg->Show();
}

void PluginSearchDlg::OnButton3Click(wxCommandEvent& event)
{
    LoadAllScripts();
    LoadAllPlugins();
}
