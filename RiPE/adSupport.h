#ifndef ADSUPPORT_H
#define ADSUPPORT_H

//(*Headers(adSupport)
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/timer.h>
#include <wx/html/htmlwin.h>
//*)

#include <wx/image.h>
#include <wx/html/htmlproc.h>
#include <wx/fs_inet.h>
#include <wx/dataobj.h>

#include "defs.h"

class adSupport: public wxDialog
{
	public:

		adSupport(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~adSupport();

		//(*Declarations(adSupport)
		wxButton* Button1;
		wxHtmlWindow* HtmlWindow1;
		wxTimer Timer1;
		//*)

	protected:

		//(*Identifiers(adSupport)
		static const long ID_HTMLWINDOW1;
		static const long ID_BUTTON1;
		static const long ID_TIMER1;
		//*)

	private:
		//(*Handlers(adSupport)
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
