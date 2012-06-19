#ifndef NEWSCRIPTDLG_H
#define NEWSCRIPTDLG_H

#include <list>
#include <vector>

//(*Headers(NewScriptDlg)
#include <wx/sizer.h>
#include <wx/button.h>
#include "asmeditor.h"
#include <wx/dialog.h>
//*)

class NewScriptDlg: public wxDialog
{
	public:

		NewScriptDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~NewScriptDlg();

		//(*Declarations(NewScriptDlg)
		wxButton* Button1;
		AsmEditor* Script;
		//*)
        void ClearCompiledScript();
        bool HasCompiledScript();

	protected:

		//(*Identifiers(NewScriptDlg)
		static const long ID_CUSTOM1;
		static const long ID_BUTTON1;
		//*)

	private:

        //(*Handlers(NewScriptDlg)
		void OnButton1Click(wxCommandEvent& event);
		//*)

		void ParseLine( const wxString &src );

        // returns the depth of the line
        size_t GetLineDepth( const wxString &line );

		DECLARE_EVENT_TABLE()
};

#endif
