#ifndef DISASSEMBLYDLG_H
#define DISASSEMBLYDLG_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(disassemblyDlg)
	#include <wx/listctrl.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/button.h>
	#include <wx/dialog.h>
	//*)
#endif
//(*Headers(disassemblyDlg)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

//#define BEA_ENGINE_STATIC
//#define BEA_USE_STDCALL
#include "beaengine/BeaEngine.h"
#include "beaengine/bea-reloc.h"

#include "../hexconverter.hpp"

class disassemblyDlg: public wxDialog
{
	public:

		disassemblyDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~disassemblyDlg();

		//(*Declarations(disassemblyDlg)
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxListView* ListView1;
		wxTextCtrl* TextCtrl1;
		//*)

	protected:

		//(*Identifiers(disassemblyDlg)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_LISTVIEW1;
		//*)

	private:

		//(*Handlers(disassemblyDlg)
		void OnButton1Click(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
