#ifndef UPLOADSCRIPTDLG_H
#define UPLOADSCRIPTDLG_H

//(*Headers(UploadScriptDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class UploadScriptDlg: public wxDialog
{
	public:

		UploadScriptDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~UploadScriptDlg();

		//(*Declarations(UploadScriptDlg)
		wxTextCtrl* TextCtrlUsername;
		wxButton* ButtonUpload;
		wxButton* ButtonBrowse;
		wxStaticText* StaticTextUsername;
		wxPanel* Panel1;
		wxStaticText* StaticText3;
		wxTextCtrl* TextCtrlScript;
		wxTextCtrl* TextCtrlFile;
		wxTextCtrl* TextCtrlProcess;
		wxTextCtrl* TextCtrlPassword;
		wxStaticText* StaticTextScript;
		wxRadioButton* RadioButtonPlugin;
		wxStaticText* StaticTextPassword;
		wxStaticText* StaticTextProcess;
		wxRadioButton* RadioButtonScript;
		//*)

	protected:

		//(*Identifiers(UploadScriptDlg)
		static const long ID_STATICTEXT3;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL4;
		static const long ID_TEXTCTRL5;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(UploadScriptDlg)
		void OnButtonBrowseClick(wxCommandEvent& event);
		void OnButtonUploadClick(wxCommandEvent& event);
		//*)
        void UploadScript( const wxString &filename );

		DECLARE_EVENT_TABLE()
};

#endif
