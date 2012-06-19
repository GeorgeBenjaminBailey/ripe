#include "stdafx.h"

#include "adBrowserDlg.h"

//(*InternalHeaders(adBrowserDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(adBrowserDlg)
//*)

BEGIN_EVENT_TABLE(adBrowserDlg,wxDialog)
	//(*EventTable(adBrowserDlg)
	//*)
END_EVENT_TABLE()

adBrowserDlg::adBrowserDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(adBrowserDlg)
	Create(parent, wxID_ANY, _("Browser"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	//*)
}

adBrowserDlg::~adBrowserDlg()
{
	//(*Destroy(adBrowserDlg)
	//*)
}

