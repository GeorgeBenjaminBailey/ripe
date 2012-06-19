#include "stdafx.h"
#include "adSupport.h"

//(*InternalHeaders(adSupport)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(adSupport)
const long adSupport::ID_HTMLWINDOW1 = wxNewId();
const long adSupport::ID_BUTTON1 = wxNewId();
const long adSupport::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(adSupport,wxDialog)
	//(*EventTable(adSupport)
	//*)
END_EVENT_TABLE()

adSupport::adSupport(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    wxInitAllImageHandlers();
#if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS
    #ifndef wxINTERNET_FS_HANDLER
    #define wxINTERNET_FS_HANDLER
    wxFileSystem::AddHandler(new wxInternetFSHandler);
    #endif
#endif

	//(*Initialize(adSupport)
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Help support RiPE, RiuTrainer, and W8baby by clicking on the ad!"), wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP|wxCAPTION|wxSYSTEM_MENU, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	HtmlWindow1 = new wxHtmlWindow(this, ID_HTMLWINDOW1, wxDefaultPosition, wxSize(468,60), wxHW_SCROLLBAR_NEVER, _T("ID_HTMLWINDOW1"));
	HtmlWindow1->LoadPage(_("http://www.w8baby.com/adinfo.txt"));
	wxString tempStr = HtmlWindow1->ToText();
	wxString tempBuffer = _("");
	for(unsigned int i=0; i < tempStr.size(); i++)
	    if( tempStr[i] == ':' || tempStr[i] == '/' || tempStr[i] == '.' ||
	        (tempStr[i] >= 'a' && tempStr[i] <= 'z') || (tempStr[i] >= 'A' &&
	        tempStr[i] <= 'Z') || (tempStr[i] >= '0' && tempStr[i] <= '9') )
	         tempBuffer += tempStr[i];
	HtmlWindow1->SetBorders( 0 );
	HtmlWindow1->LoadPage( tempBuffer );
	FlexGridSizer1->Add(HtmlWindow1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button1 = new wxButton(this, ID_BUTTON1, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer1->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&adSupport::OnButton1Click);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&adSupport::OnTimer1Trigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&adSupport::OnClose);
	//*)
}

adSupport::~adSupport()
{
	//(*Destroy(adSupport)
	//*)
}

void adSupport::OnTimer1Trigger(wxTimerEvent& event)
{
    this->Show(true); //Display the ad
    return;
}

void adSupport::OnClose(wxCloseEvent& event)
{
    return;
}

void adSupport::OnButton1Click(wxCommandEvent& event)
{
    Hide();
    return;
}
