#include "stdafx.h"

#include <wx/msgdlg.h>

#include "disassemblyDlg.h"
#include "../memreadwrite.h" // used in testing if memory is readable

//(*InternalHeaders(disassemblyDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(disassemblyDlg)
const long disassemblyDlg::ID_STATICTEXT1 = wxNewId();
const long disassemblyDlg::ID_TEXTCTRL1 = wxNewId();
const long disassemblyDlg::ID_BUTTON1 = wxNewId();
const long disassemblyDlg::ID_LISTVIEW1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(disassemblyDlg,wxDialog)
	//(*EventTable(disassemblyDlg)
	//*)
END_EVENT_TABLE()

disassemblyDlg::disassemblyDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(disassemblyDlg)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Disassembler"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(0);
	FlexGridSizer3 = new wxFlexGridSizer(0, 5, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Start Address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer3->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("00401000"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer3->Add(TextCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button1 = new wxButton(this, ID_BUTTON1, _("Disassemble"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer4->AddGrowableRow(0);
	ListView1 = new wxListView(this, ID_LISTVIEW1, wxDefaultPosition, wxSize(0,300), wxLC_REPORT|wxLC_ICON, wxDefaultValidator, _T("ID_LISTVIEW1"));
	FlexGridSizer4->Add(ListView1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&disassemblyDlg::OnButton1Click);
	//*)

    ListView1->InsertColumn(0, L"Address",     wxLIST_FORMAT_LEFT, 68);
	ListView1->InsertColumn(1, L"Bytes",       wxLIST_FORMAT_LEFT, 100);
    ListView1->InsertColumn(2, L"Instruction", wxLIST_FORMAT_LEFT, 300);
}

disassemblyDlg::~disassemblyDlg()
{
	//(*Destroy(disassemblyDlg)
	//*)
}

// TODO:  Increase accuracy of the memory is readable check.
//        As of now, it is only accurate to 12 bytes and could,
//        in extremely rare circumstances, crash.
void disassemblyDlg::OnButton1Click(wxCommandEvent& event)
{
    DISASM dis = {0};
    int instructionSize;
    dis.EIP         = (UIntPtr)HexToDecimal<int>(TextCtrl1->GetValue());

    ListView1->DeleteAllItems(); // clear list before we populate it

    bool memIsGood = true; // keeps track of if reading mem will crash you


    for( int i=0; i < 1000; ++i )
    {
        dis.VirtualAddr = dis.EIP; // fixes offsets

        bool  memIsGood  = true; // keeps track of if reading memory will crash you
        for( int i=0; i < 16; i+=4 )
        {
            DWORD trashValue; // a trash value used by ReadPointer to store data
            if( !ReadPointer( &trashValue, (LPCVOID)(dis.EIP+i) ) )
                memIsGood = false;
        }

        // we don't want to continue if memory was unreadable
        if( !memIsGood )
        {
            long index = ListView1->GetItemCount();
            index = ListView1->InsertItem(index, wxString::Format("%08x", dis.EIP) ); //Address
            ListView1->SetItem( index, 1, _("??") );                                  //Bytes
            ListView1->SetItem( index, 2, _("Memory could not be read") );            //Instruction
            ++(dis.EIP);
        }
        else if( (instructionSize = _Disasm(&dis) ) > 0 ) // -1 is an unknown instruction
        {
            long index = ListView1->GetItemCount();
            wxString byteArray = _("");
            for(int j=0; j < instructionSize; ++j )
            {
                byteArray << wxString::Format("%02x", reinterpret_cast<unsigned char *>(dis.EIP)[j] ) << " ";
            }

            index = ListView1->InsertItem(index, wxString::Format("%08x", dis.EIP) ); //Address
            ListView1->SetItem( index, 1, byteArray );                                //Bytes
            ListView1->SetItem( index, 2, dis.CompleteInstr );                        //Instruction
            dis.EIP += instructionSize;
        }
        else // unknown instruction, so we'll say that this is a "db" instruction
        {
            wxString byteArray = _("");
            byteArray << wxString::Format("%02x", reinterpret_cast<unsigned char *>(dis.EIP)[0] );

            long index = ListView1->GetItemCount();
            index = ListView1->InsertItem(index, wxString::Format("%08x", dis.EIP) ); //Address
            ListView1->SetItem( index, 1, byteArray );                                //Bytes
            ListView1->SetItem( index, 2, _("db ") + byteArray );                     //Instruction

            ++(dis.EIP);
        }
    }
    //wxMessageBox( BeaEngineVersion(), "Hello" );
}
