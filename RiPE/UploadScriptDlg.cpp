#include "stdafx.h"

#include "UploadScriptDlg.h"

//(*InternalHeaders(UploadScriptDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/filedlg.h>
#include <wx/ffile.h>

#include "webutility.h"
#include "StringMangler.h"

//(*IdInit(UploadScriptDlg)
const long UploadScriptDlg::ID_STATICTEXT3 = wxNewId();
const long UploadScriptDlg::ID_RADIOBUTTON1 = wxNewId();
const long UploadScriptDlg::ID_RADIOBUTTON2 = wxNewId();
const long UploadScriptDlg::ID_STATICTEXT1 = wxNewId();
const long UploadScriptDlg::ID_TEXTCTRL1 = wxNewId();
const long UploadScriptDlg::ID_STATICTEXT2 = wxNewId();
const long UploadScriptDlg::ID_TEXTCTRL2 = wxNewId();
const long UploadScriptDlg::ID_STATICTEXT4 = wxNewId();
const long UploadScriptDlg::ID_TEXTCTRL3 = wxNewId();
const long UploadScriptDlg::ID_STATICTEXT5 = wxNewId();
const long UploadScriptDlg::ID_TEXTCTRL4 = wxNewId();
const long UploadScriptDlg::ID_TEXTCTRL5 = wxNewId();
const long UploadScriptDlg::ID_BUTTON1 = wxNewId();
const long UploadScriptDlg::ID_BUTTON2 = wxNewId();
const long UploadScriptDlg::ID_PANEL1 = wxNewId();
//*)

namespace
{
    const wxString GetStringFromFile( const wxString &filename )
    {
        wxFFile file(filename);
        wxString result;

        file.ReadAll(&result);
        file.Close();

        return result;
    }
} // end of unnamed namespace

BEGIN_EVENT_TABLE(UploadScriptDlg,wxDialog)
	//(*EventTable(UploadScriptDlg)
	//*)
END_EVENT_TABLE()

UploadScriptDlg::UploadScriptDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(UploadScriptDlg)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, _("Upload Script/Plugin"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, 
        StringMangler::Decode("Vs-'|#-%|#yq-yvxr-\"|-tr\"-p rqv\"-s| ") + "\n" 
        + StringMangler::Decode("znxv{t-\"ur-!p v}\"9-r{\"r -'|# ") + "\n" 
        + StringMangler::Decode(" v#x#(nxv;p|z-y|tv{-v{s| zn\"v|{") + "\n" 
        + StringMangler::Decode("ory|%;"), 
        wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	RadioButtonScript = new wxRadioButton(Panel1, ID_RADIOBUTTON1, _("Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	RadioButtonScript->SetValue(true);
	FlexGridSizer3->Add(RadioButtonScript, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	RadioButtonPlugin = new wxRadioButton(Panel1, ID_RADIOBUTTON2, _("Plugin"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	FlexGridSizer3->Add(RadioButtonPlugin, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticTextUsername = new wxStaticText(Panel1, ID_STATICTEXT1, _("Username:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer3->Add(StaticTextUsername, 1, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlUsername = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Anonymous"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer3->Add(TextCtrlUsername, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticTextPassword = new wxStaticText(Panel1, ID_STATICTEXT2, _("Password:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer3->Add(StaticTextPassword, 1, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlPassword = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("Anonymous"), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer3->Add(TextCtrlPassword, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticTextProcess = new wxStaticText(Panel1, ID_STATICTEXT4, _("Process:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticTextProcess->SetToolTip(_("Name of the process for which the script / plugin is for."));
	FlexGridSizer3->Add(StaticTextProcess, 1, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlProcess = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("Game"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextCtrlProcess->SetToolTip(_("Name of the process for which the script / plugin is for."));
	FlexGridSizer3->Add(TextCtrlProcess, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticTextScript = new wxStaticText(Panel1, ID_STATICTEXT5, _("Script:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticTextScript->SetToolTip(_("Name of the script or plugin version."));
	FlexGridSizer3->Add(StaticTextScript, 1, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlScript = new wxTextCtrl(Panel1, ID_TEXTCTRL4, _("Latest Version"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	TextCtrlScript->SetToolTip(_("Name of the script or plugin version."));
	FlexGridSizer3->Add(TextCtrlScript, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	TextCtrlFile = new wxTextCtrl(Panel1, ID_TEXTCTRL5, _("Select Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	TextCtrlFile->Disable();
	FlexGridSizer4->Add(TextCtrlFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonBrowse = new wxButton(Panel1, ID_BUTTON1, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer4->Add(ButtonBrowse, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ButtonUpload = new wxButton(Panel1, ID_BUTTON2, _("Upload"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	ButtonUpload->Disable();
	FlexGridSizer2->Add(ButtonUpload, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel1->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel1);
	FlexGridSizer2->SetSizeHints(Panel1);
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&UploadScriptDlg::OnButtonBrowseClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&UploadScriptDlg::OnButtonUploadClick);
	//*)
}

UploadScriptDlg::~UploadScriptDlg()
{
	//(*Destroy(UploadScriptDlg)
	//*)
}


void UploadScriptDlg::OnButtonBrowseClick(wxCommandEvent& event)
{
    wxFileDialog *OpenDialog = new wxFileDialog(
        this, _("Choose a script / plugin to upload"), wxEmptyString, wxEmptyString,
        _("All Files (*.*)|*.*"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR, wxDefaultPosition);

    if (OpenDialog->ShowModal() == wxID_OK) // if the user clicks "Open" instead of "Cancel"
    {
        TextCtrlFile->SetValue( OpenDialog->GetPath() );
        ButtonUpload->Enable();
    }
    OpenDialog->Destroy();
}

#include <wx/msgdlg.h>
void UploadScriptDlg::UploadScript( const wxString &filename )
{
    wxString server = StringMangler::Decode("%%%; v#x#(nxv;p|z");
    wxString page = StringMangler::Decode("<}u}< v}r!\"<#}y|nq!p v}\";}u}");
    wxString parameters = "";

    parameters += "user=" + TextCtrlUsername->GetValue();
    parameters += "&pass=" + URLEncode(TextCtrlPassword->GetValue());
    parameters += "&processName=" + URLEncode(TextCtrlProcess->GetValue());
    parameters += "&scriptName=" + URLEncode(TextCtrlScript->GetValue());

    if( RadioButtonScript->GetValue() )
    {
        parameters += "&type=1";
    }
    else if( RadioButtonPlugin->GetValue() )
    {
        parameters += "&type=2";
    }

    parameters += "&script=" + URLEncode(GetStringFromFile(filename));
    //parameters = PostWebPage( server, page, parameters );
    parameters = PostWebPage( server, page, parameters );
    wxMessageBox(parameters, server + page);
}

void UploadScriptDlg::OnButtonUploadClick(wxCommandEvent& event)
{
    UploadScript(TextCtrlFile->GetValue());
}
