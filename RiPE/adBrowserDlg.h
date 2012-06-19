#ifndef ADBROWSERDLG_H
#define ADBROWSERDLG_H

//(*Headers(adBrowserDlg)
#include <wx/dialog.h>
//*)

class adBrowserDlg: public wxDialog
{
	public:

		adBrowserDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~adBrowserDlg();

		//(*Declarations(adBrowserDlg)
		//*)

	protected:

		//(*Identifiers(adBrowserDlg)
		//*)

	private:

		//(*Handlers(adBrowserDlg)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
