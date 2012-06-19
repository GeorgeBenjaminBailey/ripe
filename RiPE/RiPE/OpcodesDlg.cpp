#include "stdafx.h"

#include <set>

#include "OpcodesDlg.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(OpcodesDlg)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(OpcodesDlg)
//*)

#include <wx/msgdlg.h>

#include "../artProvider.h"
#include "../hexconverter.hpp"

namespace {
    int wxCALLBACK MyCompareFunction( long item1, long item2, wxIntPtr sortData )
    {
        return item1-item2;
    }
}

//(*IdInit(OpcodesDlg)
const long OpcodesDlg::ID_RADIOBOX1 = wxNewId();
const long OpcodesDlg::ID_STATICTEXT1 = wxNewId();
const long OpcodesDlg::ID_STATICTEXT2 = wxNewId();
const long OpcodesDlg::ID_TEXTCTRL2 = wxNewId();
const long OpcodesDlg::ID_SPINBUTTON1 = wxNewId();
const long OpcodesDlg::ID_TEXTCTRL1 = wxNewId();
const long OpcodesDlg::ID_BUTTON1 = wxNewId();
const long OpcodesDlg::ID_LISTVIEW1 = wxNewId();
const long OpcodesDlg::ID_BUTTON2 = wxNewId();
const long OpcodesDlg::ID_BUTTON3 = wxNewId();
const long OpcodesDlg::ID_BUTTON4 = wxNewId();
const long OpcodesDlg::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OpcodesDlg,wxDialog)
	//(*EventTable(OpcodesDlg)
	//*)
END_EVENT_TABLE()

OpcodesDlg::OpcodesDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(OpcodesDlg)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer7;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Label Opcodes"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(1);
	FlexGridSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Send"),
		_("Recv")
	};
	RadioBox1 = new wxRadioBox(Panel1, ID_RADIOBOX1, _("Opcode Type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, wxRA_VERTICAL, wxDefaultValidator, _T("ID_RADIOBOX1"));
	FlexGridSizer7->Add(RadioBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Opcode"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer3->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Label"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer3->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer6 = new wxFlexGridSizer(0, 3, 0, 0);
	TextCtrl2 = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("000A"), wxDefaultPosition, wxSize(40,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrl2->SetMaxLength(4);
	FlexGridSizer6->Add(TextCtrl2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SpinButton1 = new wxSpinButton(Panel1, ID_SPINBUTTON1, wxDefaultPosition, wxSize(-1,23), wxSP_VERTICAL|wxSP_ARROW_KEYS, _T("ID_SPINBUTTON1"));
	SpinButton1->SetValue(10);
	SpinButton1->SetRange(0, 65535);
	FlexGridSizer6->Add(SpinButton1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Label"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer3->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON1"));
	Button1->SetToolTip(_("Adds an opcode to the list.  If the opcode is already in the list, the conflicting opcodes will display in red.  You will not be able to save until you resolve all conflicts."));
	Button1->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("ADD_XPM")),wxART_BUTTON));
	FlexGridSizer3->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer4->AddGrowableRow(0);
	ListView1 = new wxListView(Panel1, ID_LISTVIEW1, wxDefaultPosition, wxSize(-1,160), wxLC_REPORT|wxLC_EDIT_LABELS, wxDefaultValidator, _T("ID_LISTVIEW1"));
	FlexGridSizer4->Add(ListView1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("Remove Selected"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON2"));
	Button2->SetToolTip(_("Remove the selected opcodes / labels from the list view."));
	Button2->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("DELETE_XPM")),wxART_BUTTON));
	FlexGridSizer5->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button3 = new wxButton(Panel1, ID_BUTTON3, _("Down"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON3"));
	Button3->SetToolTip(_("Moves the selected items down by 1 opcode.\nExample:  000A would become 000B."));
	Button3->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("DOWNARROW_XPM")),wxART_BUTTON));
	FlexGridSizer5->Add(Button3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button4 = new wxButton(Panel1, ID_BUTTON4, _("Up"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON4"));
	Button4->SetToolTip(_("Moves the selected opcodes up by 1 opcode.\nExample:  000A would become 0009."));
	Button4->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("UPARROW_XPM")),wxART_BUTTON));
	FlexGridSizer5->Add(Button4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(Panel1, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(Panel1, wxID_SAVE, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer2->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel1->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel1);
	FlexGridSizer2->SetSizeHints(Panel1);
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&OpcodesDlg::OnRadioBox1Select);
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&OpcodesDlg::OnTextCtrl2Text);
	Connect(ID_SPINBUTTON1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&OpcodesDlg::OnSpinButton1Change);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpcodesDlg::OnButton1Click);
	Connect(ID_LISTVIEW1,wxEVT_COMMAND_LIST_END_LABEL_EDIT,(wxObjectEventFunction)&OpcodesDlg::OnListView1EndLabelEdit);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpcodesDlg::OnButton2Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpcodesDlg::OnButton3Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpcodesDlg::OnButton4Click);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&OpcodesDlg::OnClose);
	//*)
    Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpcodesDlg::OnCancelButtonClicked);
    Connect(wxID_SAVE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpcodesDlg::OnSaveButtonClicked);
    ListView1->InsertColumn( 0, _("Opcode"), 0, 80 );
    ListView1->InsertColumn( 1, _("Label"), 0, 250 );
}

OpcodesDlg::~OpcodesDlg()
{
	//(*Destroy(OpcodesDlg)
	//*)
}

void OpcodesDlg::OnSpinButton1Change(wxSpinEvent& event)
{
    TextCtrl2->SetValue( DecimalToHex((unsigned short)event.GetValue()) );
}

// On edit textbox for the "spin ctrl"
void OpcodesDlg::OnTextCtrl2Text(wxCommandEvent& event)
{
    const wxString str      = event.GetString();
    const long     oldPoint = TextCtrl2->GetInsertionPoint();
    long           newPoint = 0;
    wxString newString;

    for( wxString::size_type i=0; i < str.size(); ++i ) {
        if( (str[i] >= '0' && str[i] <= '9') ||
            (str[i] >= 'A' && str[i] <= 'F') ) {
            newString += str[i];
        }
        else if( str[i] >= 'a' && str[i] <= 'f' ) {
            newString += ((char)str[i] & ~0x20 ); // uppercase
        }
        // set the insertion point to the new location
        if( i+1 == oldPoint ) {
            newPoint = newString.size();
        }
    }
    TextCtrl2->ChangeValue(newString);
    TextCtrl2->SetInsertionPoint(newPoint);
    SpinButton1->SetValue( HexToDecimal<unsigned short>(newString) );
}

void OpcodesDlg::OnCancelButtonClicked( wxCommandEvent& event )
{
    Hide();
}

void OpcodesDlg::OnSaveButtonClicked( wxCommandEvent& event )
{
    int errorCode = SaveOpcodes();
    if( errorCode == OPCODES_SAVE_SEND_DUPLICATE )
        wxMessageBox( _("All send opcodes must be unique.  Please ensure that no two \"Send\" opcodes are identical.  They will be highlighted in red if duplicates are found."), _("Error: Invalid opcodes"), wxICON_ERROR|wxOK );
    else if( errorCode == OPCODES_SAVE_SEND_DUPLICATE )
        wxMessageBox( _("All recv opcodes must be unique.  Please ensure that no two \"Recv\" opcodes are identical.  They will be highlighted in red if duplicates are found."), _("Error: Invalid opcodes"), wxICON_ERROR|wxOK );
    else {
        SaveOpcodes();
        Hide();
    }
}

void OpcodesDlg::OnClose(wxCloseEvent& event)
{
    Hide();
}

// On delete selected
void OpcodesDlg::OnButton2Click(wxCommandEvent& event)
{
    long index = -1;

    // delete all selected items
    while( ListView1->GetSelectedItemCount() > 0 ) {
        index = ListView1->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
        ListView1->DeleteItem(index);
    }
    ListView1->SortItems(MyCompareFunction, (wxIntPtr)ListView1 );
    ColorOpcodes();
}

// On "Add" button clicked
void OpcodesDlg::OnButton1Click(wxCommandEvent& event)
{
    long index = ListView1->GetItemCount();
    index = ListView1->InsertItem( index, DecimalToHex<unsigned short>(SpinButton1->GetValue()) );

    ListView1->SetItem( index, 1, TextCtrl1->GetValue() );
    ListView1->SetItemData( index, SpinButton1->GetValue() ); // used in sorting
    ListView1->SortItems(MyCompareFunction, (wxIntPtr)ListView1 );
    ColorOpcodes();
}

// On editing an opcode label
void OpcodesDlg::OnListView1EndLabelEdit(wxListEvent& event)
{
    const long index = event.GetIndex();
    const wxString str = event.GetLabel();
    wxString newString;

    // Strip invalid characters from string
    for( wxString::size_type i=0; i < str.size(); ++i ) {
        if( (str[i] >= '0' && str[i] <= '9') ||
            (str[i] >= 'A' && str[i] <= 'F') ) {
                newString += str[i];
        }
        else if( str[i] >= 'a' && str[i] <= 'f' ) {
            newString += ((char)str[i] & ~0x20 ); // uppercase
        }
    }

    // Format the string
    if( !newString.IsEmpty() )
        newString = DecimalToHex(HexToDecimal<short>(newString));
    else // if edit canceled, empty label, or all invalid characters
        newString = ListView1->GetItemText(index);
    ListView1->SetItem( index, 0, newString );
    ListView1->SetItemData( index, HexToDecimal<unsigned short>(newString) );
    event.Veto();
    ListView1->SortItems(MyCompareFunction, (wxIntPtr)ListView1 );
    // Check for existing opcode.  If it exists, we want to label it red (error)
    ColorOpcodes();
}

// Return true if send opcodes are valid (no duplicates)
// Else returns false
bool OpcodesDlg::IsSendValid() const
{
    std::multiset<unsigned short> ops;
    std::list<std::pair<unsigned short, wxString> >::const_iterator iter;
    iter = m_unsavedOpcodes[0].begin();
    for( ; iter != m_unsavedOpcodes[0].end(); ++iter )
        ops.insert( iter->first );
    std::multiset<unsigned short>::iterator i = ops.begin();
    for( ; i != ops.end(); ++i ) {
        if( ops.count(*i) > 1 )
            return false;
    }
    return true;
}

// Return true if recv opcodes are valid (no duplicates)
// Else returns false
bool OpcodesDlg::IsRecvValid() const
{
    std::multiset<unsigned short> ops;
    std::list<std::pair<unsigned short, wxString> >::const_iterator iter;
    iter = m_unsavedOpcodes[1].begin();
    for( ; iter != m_unsavedOpcodes[1].end(); ++iter )
        ops.insert( iter->first );
    std::multiset<unsigned short>::iterator i = ops.begin();
    for( ; i != ops.end(); ++i ) {
        if( ops.count(*i) > 1 )
            return false;
    }
    return true;
}

// Colors any duplicate opcodes red and any non-duplicate opcodes black
void OpcodesDlg::ColorOpcodes()
{
    std::multimap<wxString, long>           opcodeMap;
    std::multimap<wxString, long>::iterator iter;
    long index = ListView1->GetNextItem(-1);

    // Populate our multimap
    for( ; index != -1; index = ListView1->GetNextItem(index) ) {
        opcodeMap.insert( std::pair<wxString, long>(ListView1->GetItemText(index), index) );
    }

    // Run through the multimap looking for duplicates
    for( iter=opcodeMap.begin(); iter != opcodeMap.end(); ++iter ) {
        if( opcodeMap.count(iter->first) > 1 ) { // duplicate found
            ListView1->SetItemBackgroundColour(iter->second, wxColour(243,117,117));
        } else { // no duplicates, so color black
            ListView1->SetItemBackgroundColour(iter->second, wxColour(255,255,255));
        }
    }
}

// Updates m_savedOpcodes.
int OpcodesDlg::SaveOpcodes()
{
    // Update the lists that we use for saving
    UpdateUnsavedOpcodes(RadioBox1->GetSelection());

    // Check for duplicates
    if( !IsSendValid() )
        return OPCODES_SAVE_SEND_DUPLICATE;
    else if( !IsRecvValid() )
        return OPCODES_SAVE_RECV_DUPLICATE;
    else {
        // Save both send and recv
        for( int i=0; i < 2; ++i ) {
            m_savedOpcodes[i].clear();
            std::list<std::pair<unsigned short, wxString> >::iterator iter;
            iter = m_unsavedOpcodes[i].begin();
            // For all elements in the list, save them to the map
            for( ; iter != m_unsavedOpcodes[i].end(); ++iter )
                m_savedOpcodes[i][iter->first] = iter->second;
        }
    }
    return OPCODES_SAVE_NO_ERROR;
}

// On "Down" button pressed.  This moves all selected opcodes down, changing
// their value by +1.
void OpcodesDlg::OnButton3Click(wxCommandEvent& event)
{
    long index = ListView1->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    // delete all selected items
    for( ; index != -1; index = ListView1->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ) {
        const unsigned short newData = ListView1->GetItemData(index)+1;
        ListView1->SetItem( index, 0, DecimalToHex(newData) );
        ListView1->SetItemData( index, newData );
    }
    ListView1->SortItems(MyCompareFunction, (wxIntPtr)ListView1 );
    ColorOpcodes();
}

// On "Up" button pressed.  This moves all selected opcodes up, changing
// their value by -1.
void OpcodesDlg::OnButton4Click(wxCommandEvent& event)
{
    long index = ListView1->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    // delete all selected items
    for( ; index != -1; index = ListView1->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ) {
        const unsigned short newData = ListView1->GetItemData(index)-1;
        ListView1->SetItem( index, 0, DecimalToHex(newData) );
        ListView1->SetItemData( index, newData );
    }
    ListView1->SortItems(MyCompareFunction, (wxIntPtr)ListView1 );
    ColorOpcodes();
}

// On "Opcode Type" selected.  (Send / Recv)
void OpcodesDlg::OnRadioBox1Select(wxCommandEvent& event)
{
    // event.GetSelection() will return the new selection.
    // Since there are only two selections, 1 - the new selection
    // will equal the old selection.
    const int newSelection = event.GetSelection();
    const int oldSelection = 1 - newSelection;
    long index = -1;

    UpdateUnsavedOpcodes( oldSelection );

    RefreshOpcodes();
}

// Given an index (0 for send, 1 for recv), updates the unsaved opcodes list
// to whatever ListView1 contains.  Doesn't strip duplicates.
void OpcodesDlg::UpdateUnsavedOpcodes( const int index )
{
    long i = ListView1->GetNextItem(-1);

    m_unsavedOpcodes[index].clear(); // clear old list
    for( ; i != -1; i = ListView1->GetNextItem(i) ) {
        m_unsavedOpcodes[index].push_front(
        std::pair<unsigned short, wxString>(static_cast<unsigned short>(ListView1->GetItemData(i)), ListView1->GetItemText(i,1)) );
    }
}

// Returns a map of all opcodes of a given type.  To specify type, use OPCODES_SEND
// or OPCODES_RECV.
std::map<unsigned short, wxString> OpcodesDlg::GetOpcodes( const int type ) const
{
    return m_savedOpcodes[type];
}

// Sets or inserts a send or recv opcode (depending on if the type is OPCODES_SEND
// or OPCODES_RECV).  This will ALWAYS insert a new m_unsavedOpcodes, so be careful
void OpcodesDlg::SetOpcode( const unsigned short op, const wxString &label, const int type )
{
    m_savedOpcodes[type][op] = label;
    m_unsavedOpcodes[type].push_back( std::pair<unsigned short, wxString>(op,label) );
}

// Clears all opcodes of a given type (OPCODES_SEND or OPCODES_RECV)
void OpcodesDlg::ClearOpcodes( const int type )
{
    m_savedOpcodes[type].clear();
    m_unsavedOpcodes[type].clear();
}

// Refreshes the List View to the type of RadioBox.  This uses m_unsavedOpcodes
// to generate the list.
void OpcodesDlg::RefreshOpcodes()
{
    std::list<std::pair<unsigned short, wxString> >::iterator iter;
    const int sel = RadioBox1->GetSelection();
    long index = -1;

    // Clear old opcodes
    ListView1->DeleteAllItems();
    index = ListView1->GetItemCount(); // should be 0

    // Display the new opcodes
    iter = m_unsavedOpcodes[sel].begin();
    for( ; iter != m_unsavedOpcodes[sel].end(); ++iter ) {
        index = ListView1->InsertItem( index, DecimalToHex(iter->first) ); // opcode in hex
        ListView1->SetItem( index, 1, iter->second ); // label
        ListView1->SetItemData( index, iter->first ); // opcode value
    }

    // Refresh / sort the items
    ListView1->SortItems(MyCompareFunction, (wxIntPtr)ListView1 );
    ColorOpcodes();
}
