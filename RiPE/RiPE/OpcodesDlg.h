#ifndef OPCODESDLG_H
#define OPCODESDLG_H

#include <list>
#include <map>
#include <utility>

#ifndef WX_PRECOMP
	//(*HeadersPCH(OpcodesDlg)
	#include <wx/listctrl.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/radiobox.h>
	#include <wx/textctrl.h>
	#include <wx/panel.h>
	#include <wx/button.h>
	#include <wx/dialog.h>
	//*)
#endif
//(*Headers(OpcodesDlg)
#include <wx/spinbutt.h>
//*)

#include "hexspinctrl.h" // A custom hexadecimal-only spin ctrl

class OpcodesDlg: public wxDialog
{
	public:

        enum {
            OPCODES_SEND = 0,
            OPCODES_RECV = 1
        };

		OpcodesDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~OpcodesDlg();

		//(*Declarations(OpcodesDlg)
		wxButton* Button4;
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxButton* Button2;
		wxSpinButton* SpinButton1;
		wxButton* Button3;
		wxListView* ListView1;
		wxTextCtrl* TextCtrl2;
		wxTextCtrl* TextCtrl1;
		wxRadioBox* RadioBox1;
		//*)
        std::map<unsigned short, wxString> GetOpcodes( const int type ) const;
        void SetOpcode( const unsigned short op, const wxString &label, const int type );

        void ClearOpcodes( const int type );
        void RefreshOpcodes();

	protected:

		//(*Identifiers(OpcodesDlg)
		static const long ID_RADIOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_SPINBUTTON1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_LISTVIEW1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_PANEL1;
		//*)
        std::map<unsigned short, wxString> m_savedOpcodes[2];
        std::list<std::pair<unsigned short, wxString> > m_unsavedOpcodes[2];

	private:

        enum {
            OPCODES_SAVE_NO_ERROR,
            OPCODES_SAVE_SEND_DUPLICATE,
            OPCODES_SAVE_RECV_DUPLICATE
        };

		//(*Handlers(OpcodesDlg)
		void OnSpinCtrl1Change(wxSpinEvent& event);
		void OnSpinButton1ChangeUp(wxSpinEvent& event);
		void OnSpinButton1Change(wxSpinEvent& event);
		void OnTextCtrl2Text(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton2Click1(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnListView1EndLabelEdit(wxListEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnRadioBox1Select(wxCommandEvent& event);
		//*)
        void OnCancelButtonClicked(wxCommandEvent& event);
        void OnSaveButtonClicked(wxCommandEvent& event);

        bool IsSendValid() const;
        bool IsRecvValid() const;

        void ColorOpcodes();
        int  SaveOpcodes();
        void UpdateUnsavedOpcodes( const int index );

		DECLARE_EVENT_TABLE()
};

#endif
