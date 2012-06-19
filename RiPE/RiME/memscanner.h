#ifndef MEMSCANNER_H
#define MEMSCANNER_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(MemScanner)
	#include <wx/listctrl.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/checkbox.h>
	#include <wx/choice.h>
	#include <wx/button.h>
	#include <wx/dialog.h>
	//*)
#endif
//(*Headers(MemScanner)
//*)

#include <list>

#include "../defs.h"

#include "../hexconverter.hpp"
//#include "../usefulFunctions.h"

class MemScanner: public wxDialog
{
	public:

		MemScanner(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MemScanner();

		//(*Declarations(MemScanner)
		wxListCtrl* ListCtrl1;
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxCheckBox* CheckBox1;
		wxStaticText* StaticText5;
		wxTextCtrl* TextCtrl1;
		wxStaticText* StaticText4;
		wxChoice* Choice1;
		wxChoice* Choice2;
		//*)

	protected:

		//(*Identifiers(MemScanner)
		static const long ID_LISTCTRL1;
		static const long ID_STATICTEXT4;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT1;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL1;
		static const long ID_CHOICE1;
		static const long ID_CHOICE2;
		//*)

	private:

		//(*Handlers(MemScanner)
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnChoice2Select(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

enum ValueType
{
    VALUE_TYPE_BINARY = 0,
    VALUE_TYPE_BYTE,
    VALUE_TYPE_WORD,
    VALUE_TYPE_DWORD,
    VALUE_TYPE_QWORD,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_DOUBLE,
    VALUE_TYPE_TEXT,
    VALUE_TYPE_AOB
};

enum ScanType
{
    SCAN_EXACT = 0,
    SCAN_GREATER,
    SCAN_LESS,
};

template< typename T>
bool GreaterThan( T a, T b );

template< typename T>
bool LessThan( T a, T b );

template< typename T>
bool EqualTo( T a, T b );

#endif
