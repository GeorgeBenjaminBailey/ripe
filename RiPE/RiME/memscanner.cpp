#include "stdafx.h"

#include "../wx_pch.h"
#include "memscanner.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(MemScanner)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(MemScanner)
//*)

#include "memscan.hpp"

//(*IdInit(MemScanner)
const long MemScanner::ID_LISTCTRL1 = wxNewId();
const long MemScanner::ID_STATICTEXT4 = wxNewId();
const long MemScanner::ID_BUTTON1 = wxNewId();
const long MemScanner::ID_BUTTON2 = wxNewId();
const long MemScanner::ID_STATICTEXT1 = wxNewId();
const long MemScanner::ID_CHECKBOX1 = wxNewId();
const long MemScanner::ID_STATICTEXT2 = wxNewId();
const long MemScanner::ID_STATICTEXT3 = wxNewId();
const long MemScanner::ID_STATICTEXT5 = wxNewId();
const long MemScanner::ID_TEXTCTRL1 = wxNewId();
const long MemScanner::ID_CHOICE1 = wxNewId();
const long MemScanner::ID_CHOICE2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MemScanner,wxDialog)
	//(*EventTable(MemScanner)
	//*)
END_EVENT_TABLE()

MemScanner::MemScanner(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MemScanner)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer7;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Memory Scanner"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(0);
	ListCtrl1 = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxSize(200,300), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer2->Add(ListCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("This is NOT working as \nintended yet.  Most of\nthe value types are\nbroken and won\'t work."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticText4->SetForegroundColour(wxColour(184,10,10));
	FlexGridSizer3->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	Button1 = new wxButton(this, ID_BUTTON1, _("First Scan"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer4->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("Next Scan"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	Button2->Disable();
	FlexGridSizer4->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer6->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	CheckBox1 = new wxCheckBox(this, ID_CHECKBOX1, _("Hex"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBox1->SetValue(true);
	CheckBox1->Disable();
	FlexGridSizer6->Add(CheckBox1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Scan Type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer6->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Value Type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer6->Add(StaticText3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer5->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Array of Bytes"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer7->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer7->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Choice1 = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	Choice1->SetSelection( Choice1->Append(_("Exact Value")) );
	Choice1->Append(_("Greater than..."));
	Choice1->Append(_("Less than..."));
	FlexGridSizer7->Add(Choice1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Choice2 = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	Choice2->Append(_("Binary"));
	Choice2->Append(_("Byte"));
	Choice2->Append(_("2 Bytes"));
	Choice2->SetSelection( Choice2->Append(_("4 Bytes")) );
	Choice2->Append(_("8 Bytes"));
	Choice2->Append(_("Float"));
	Choice2->Append(_("Double"));
	Choice2->Append(_("Text"));
	Choice2->Append(_("Array of Bytes"));
	FlexGridSizer7->Add(Choice2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer5->Add(FlexGridSizer7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MemScanner::OnButton1Click);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MemScanner::OnTextCtrl1Text);
	Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MemScanner::OnChoice2Select);
	//*)

    ListCtrl1->InsertColumn(0, L"Address", wxLIST_FORMAT_LEFT, 68);
    ListCtrl1->InsertColumn(1, L"Value",   wxLIST_FORMAT_LEFT, 122);
}

MemScanner::~MemScanner()
{
	//(*Destroy(MemScanner)
	//*)
}

void MemScanner::OnTextCtrl1Text(wxCommandEvent& event)
{
    wxString replaceString = event.GetString();
    if( CheckBox1->GetValue() ) //If value is in hex
    {
        wxString strBuffer = _("");
        long     insertionPoint = TextCtrl1->GetInsertionPoint();
        for( unsigned int i=0; i < replaceString.size(); ++i )
        {
            if( (replaceString[i] >= '0' && replaceString[i] <= '9') ||
                (replaceString[i] >= 'A' && replaceString[i] <= 'F') ||
                (replaceString[i] >= 'a' && replaceString[i] <= 'f') ||
                (replaceString[i] == '?' || replaceString[i] == ' ') )
                strBuffer += replaceString[i];
            else if( i < (unsigned int)TextCtrl1->GetInsertionPoint() )
                --insertionPoint;
        }
        TextCtrl1->ChangeValue( strBuffer );
        TextCtrl1->SetInsertionPoint( insertionPoint );
    }
    else if( !CheckBox1->GetValue() ) //If value is in decimal
    {
        wxString strBuffer = _("");
        long     insertionPoint = TextCtrl1->GetInsertionPoint();
        for( unsigned int i=0; i < replaceString.size(); ++i )
        {
            if( (replaceString[i] >= '0' && replaceString[i] <= '9') ||
                (replaceString[i] == '?' || replaceString[i] == ' ') )
                strBuffer += replaceString[i];
            else if( i < (unsigned int)TextCtrl1->GetInsertionPoint() )
                --insertionPoint;
        }
        TextCtrl1->ChangeValue( strBuffer );
        TextCtrl1->SetInsertionPoint( insertionPoint );
    }
    return;
}

void MemScanner::OnChoice2Select(wxCommandEvent& event)
{
    return;
}

template< typename T>
bool GreaterThan( T a, T b )
{
    return a > b;
}

template< typename T>
bool LessThan( T a, T b )
{
    return a < b;
}

template< typename T>
bool EqualTo( T a, T b )
{
    return a == b;
}

void MemScanner::OnButton1Click(wxCommandEvent& event)
{
    std::list<Address> addressList;
    Scanner            scanner;

    ListCtrl1->DeleteAllItems();
    // populate list
    switch( Choice2->GetSelection() )
    {
    case VALUE_TYPE_BINARY:
        break;
    case VALUE_TYPE_BYTE:
        if( Choice1->GetSelection() == SCAN_EXACT )
            scanner.Scan( HexToDecimal<char>( TextCtrl1->GetValue() ), addressList, EqualTo<char> );
        else if( Choice1->GetSelection() == SCAN_GREATER )
            scanner.Scan( HexToDecimal<char>( TextCtrl1->GetValue() ), addressList, GreaterThan<char> );
        else if( Choice1->GetSelection() == SCAN_LESS )
            scanner.Scan( HexToDecimal<char>( TextCtrl1->GetValue() ), addressList, LessThan<char> );
        break;
    case VALUE_TYPE_WORD:
        if( Choice1->GetSelection() == SCAN_EXACT )
            scanner.Scan( HexToDecimal<short>( TextCtrl1->GetValue() ), addressList, EqualTo<short> );
        else if( Choice1->GetSelection() == SCAN_GREATER )
            scanner.Scan( HexToDecimal<short>( TextCtrl1->GetValue() ), addressList, GreaterThan<short> );
        else if( Choice1->GetSelection() == SCAN_LESS )
            scanner.Scan( HexToDecimal<short>( TextCtrl1->GetValue() ), addressList, LessThan<short> );
        break;
    case VALUE_TYPE_DWORD:
        if( Choice1->GetSelection() == SCAN_EXACT )
            scanner.Scan( HexToDecimal<int>( TextCtrl1->GetValue() ), addressList, EqualTo<int> );
        else if( Choice1->GetSelection() == SCAN_GREATER )
            scanner.Scan( HexToDecimal<int>( TextCtrl1->GetValue() ), addressList, GreaterThan<int> );
        else if( Choice1->GetSelection() == SCAN_LESS )
            scanner.Scan( HexToDecimal<int>( TextCtrl1->GetValue() ), addressList, LessThan<int> );
        break;
    case VALUE_TYPE_QWORD:
        if( Choice1->GetSelection() == SCAN_EXACT )
            scanner.Scan( HexToDecimal<long long>( TextCtrl1->GetValue() ), addressList, EqualTo<long long> );
        else if( Choice1->GetSelection() == SCAN_GREATER )
            scanner.Scan( HexToDecimal<long long>( TextCtrl1->GetValue() ), addressList, GreaterThan<long long> );
        else if( Choice1->GetSelection() == SCAN_LESS )
            scanner.Scan( HexToDecimal<long long>( TextCtrl1->GetValue() ), addressList, LessThan<long long> );
        break;
    case VALUE_TYPE_FLOAT:
        //scanner.Scan( HexToDecimal<float>( TextCtrl1->GetValue() ), addressList );
        break;
    case VALUE_TYPE_DOUBLE:
        //scanner.Scan( HexToDecimal<double>( TextCtrl1->GetValue() ), addressList );
        break;
    }

    std::list<Address>::iterator iter = addressList.begin();
    while( iter != addressList.end() )
    {
        long index = ListCtrl1->GetItemCount();
        index = ListCtrl1->InsertItem(index, DecimalToHex(iter->GetValue()) );        //Address
        ListCtrl1->SetItem( index, 1, wxString::Format( _("%i"), (int)(*iter) ) ); //Value
        ++iter;
    }
    wxMessageBox( wxString::Format( _("%i"), HexToDecimal<int>( TextCtrl1->GetValue() ) ), _("Addresses Found:") );
    return;
}
