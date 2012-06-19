#ifndef ADDPLUGINDLG_H
#define ADDPLUGINDLG_H

//(*Headers(AddPluginDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/panel.h>
#include <wx/dialog.h>
//*)

class AddPluginDlg: public wxDialog
{
	public:

		AddPluginDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~AddPluginDlg();

		//(*Declarations(AddPluginDlg)
		wxTextCtrl* TextCtrlPlugin;
		wxFilePickerCtrl* FilePickerCtrl1;
		wxStaticText* StaticText2;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxTextCtrl* TextCtrlGame;
		//*)
        bool m_addScript;

        void OnOK(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);

	protected:

		//(*Identifiers(AddPluginDlg)
		static const long ID_STATICTEXT3;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(AddPluginDlg)
		void OnClose(wxCloseEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
