#include <stdafx.h>

#include "AddPluginDlg.h"

//(*InternalHeaders(AddPluginDlg)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

#include "StringMangler.h"

//(*IdInit(AddPluginDlg)
const long AddPluginDlg::ID_STATICTEXT3 = wxNewId();
const long AddPluginDlg::ID_FILEPICKERCTRL1 = wxNewId();
const long AddPluginDlg::ID_STATICTEXT1 = wxNewId();
const long AddPluginDlg::ID_TEXTCTRL1 = wxNewId();
const long AddPluginDlg::ID_STATICTEXT2 = wxNewId();
const long AddPluginDlg::ID_TEXTCTRL2 = wxNewId();
const long AddPluginDlg::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(AddPluginDlg,wxDialog)
	//(*EventTable(AddPluginDlg)
	//*)
END_EVENT_TABLE()

AddPluginDlg::AddPluginDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(AddPluginDlg)
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, id, StringMangler::Decode("Nqq-]y#tv{"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, StringMangler::Decode("vq"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, StringMangler::Decode("VQl]N[RY>"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, 
        StringMangler::Decode("`ryrp\"-\"ur-}y#tv{<!p v}\"-'|#-%|#yq") + "\n" + StringMangler::Decode("yvxr-\"|-nqq-\"|-_v]R9-n{q-\"ur{-r{\"r ") + "\n" + 
        StringMangler::Decode("n-tnzr-{nzr-n{q-ynory-s| -\"uri{}y#tv{<!p v}\";"), 
        wxDefaultPosition, wxDefaultSize, 0, StringMangler::Decode("VQl`aNaVPaRea@"));
	FlexGridSizer2->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(Panel1, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer2->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, StringMangler::Decode("Tnzr-[nzrG-"), wxDefaultPosition, wxDefaultSize, 0, StringMangler::Decode("VQl`aNaVPaRea>"));
	FlexGridSizer3->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrlGame = new wxTextCtrl(Panel1, ID_TEXTCTRL1, StringMangler::Decode("Tnzr"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, StringMangler::Decode("VQlaReaPa_Y>"));
	FlexGridSizer3->Add(TextCtrlGame, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, StringMangler::Decode("YnoryG-"), wxDefaultPosition, wxDefaultSize, 0, StringMangler::Decode("VQl`aNaVPaRea?"));
	FlexGridSizer3->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrlPlugin = new wxTextCtrl(Panel1, ID_TEXTCTRL2, StringMangler::Decode("Yn\"r!\"-cr !v|{"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, StringMangler::Decode("VQlaReaPa_Y?"));
	FlexGridSizer3->Add(TextCtrlPlugin, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(Panel1, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(Panel1, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer2->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel1->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel1);
	FlexGridSizer2->SetSizeHints(Panel1);
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&AddPluginDlg::OnClose);
	//*)
    m_addScript = false;

    Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AddPluginDlg::OnOK);
    Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AddPluginDlg::OnCancel);
}

AddPluginDlg::~AddPluginDlg()
{
	//(*Destroy(AddPluginDlg)
	//*)
}

void AddPluginDlg::OnOK(wxCommandEvent& event)
{
    m_addScript = true;
    Hide();
}

void AddPluginDlg::OnCancel(wxCommandEvent& event)
{
    m_addScript = false;
    Hide();
}

void AddPluginDlg::OnClose(wxCloseEvent& event)
{
    m_addScript = false;
    Hide();
}
