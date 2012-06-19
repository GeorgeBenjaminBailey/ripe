#include "stdafx.h"

#include "defs.h"
#include "NewScriptDlg.h"

#include <wx/msgdlg.h>

//(*InternalHeaders(NewScriptDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(NewScriptDlg)
const long NewScriptDlg::ID_CUSTOM1 = wxNewId();
const long NewScriptDlg::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NewScriptDlg,wxDialog)
	//(*EventTable(NewScriptDlg)
	//*)
END_EVENT_TABLE()

NewScriptDlg::NewScriptDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(NewScriptDlg)
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("RiPEST Editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	SetMinSize(wxSize(400,350));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Script = new AsmEditor(this,ID_CUSTOM1,wxDefaultPosition,wxDefaultSize,0,_T("ID_CUSTOM1"));
	FlexGridSizer1->Add(Script, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("Save Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer1->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NewScriptDlg::OnButton1Click);
	//*)
}

NewScriptDlg::~NewScriptDlg()
{
	//(*Destroy(NewScriptDlg)
	//*)
}

void NewScriptDlg::ClearCompiledScript()
{
    Script->SetText("");
}

bool NewScriptDlg::HasCompiledScript() {
    return Script->GetTextLength() > 0;
}

// On button save script
void NewScriptDlg::OnButton1Click(wxCommandEvent& event)
{
    Hide();
}

// returns the depth of the line
size_t NewScriptDlg::GetLineDepth( const wxString &line )
{
    wxString strippedLine;
    size_t pos = -1, commentPos = -1;

    commentPos = line.Find("//");
    if( commentPos != wxNOT_FOUND )
    {
        strippedLine = line.Mid(0, commentPos);
        pos = strippedLine.find_first_not_of(' ');
        if( pos >= strippedLine.size() )
            pos = wxNOT_FOUND;
    }
    else
    {
        pos = line.find_first_not_of(' ');
        if( pos >= line.size() )
            pos = wxNOT_FOUND;
    }

    if( pos != wxNOT_FOUND )
        return pos/4;
    else
        return -1; // empty line
}
