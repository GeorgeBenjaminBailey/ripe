#include "stdafx.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(RiPE)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(RiPE)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/font.h>
#include <wx/image.h>
//*)
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/process.h>
#include <wx/textdlg.h>

#include "RiPE.h"

#include "RiPE/packetitem.hpp"
#include "RiPE/RiPEApp.h"

#include "processutility.h"
#include "StringMangler.h"

#include "usefulFunctions.h"

//Ugly workaround for wxSmith's "Extra Code"
#define NEWLINE '\n'
#define ESCAPED_NEWLINE _("\\n")

namespace
{
    LRESULT CALLBACK ProcessHotKey (__in int nCode, __in WPARAM wParam, __in LPARAM lParam) {
        if (nCode == HC_ACTION) {
            RiPEApp::GetRiPE()->ProcessHotKey(nCode, wParam, lParam);
        }

        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    class RiPETreeItemData : public wxTreeItemData
    {
    private:
        wxString m_label;
        wxString m_packet;
        SOCKET   m_socket;

    public:

        RiPETreeItemData()
        {
            m_label  = _("");
            m_packet = _("");
            m_socket = 0;
        }

        RiPETreeItemData( const wxString &packet )
        {
            m_label  = _("");
            m_packet = packet;
            m_socket = 0;
        }

        RiPETreeItemData( const wxString &label, const wxString &packet, const SOCKET socket )
        {
            m_label  = label;
            m_packet = packet;
            m_socket = socket;
        }

        // Getters / Setters
        const wxString GetLabel() const
        {
            return m_label;
        }

        const wxString GetPacket() const
        {
            return m_packet;
        }

        const SOCKET GetSocket() const
        {
            return m_socket;
        }

        void SetLabel( const wxString &label )
        {
            m_label = label;
        }

        void SetPacket( const wxString &packet )
        {
            m_packet = packet;
        }

        void SetSocket( const SOCKET socket )
        {
            m_socket = socket;
        }

        // Returns the wxString "value" of the item data.  An example
        // of this is:  007A  | socket: 1234  | Drop Mesos
        const wxString GetValue() const
        {
            wxString result = m_packet;
            result         += m_socket > 0 ? _("  | socket: ") + wxString::Format("%i",m_socket) : _("");
            result         += m_label.size() > 0 ? _("  | ")+m_label : _("");

            return result;
        }
    };

    /**
        Returns true if this packet is dangerous in nature, such as a "scam" packet.
     */
    bool IsBadPacket(const wxString &packet) {
        wxString shortenedPacket = packet;
        shortenedPacket.Replace(" ", "");
        return shortenedPacket.Contains("0101CEFF00000100") ||
            shortenedPacket.Contains("0001F0FF00000100") ||
            shortenedPacket.Contains("4981210001F1FF00000100") ||
            shortenedPacket.Contains("F5FF00000100") ||
            shortenedPacket.Contains("F6FF00000100") ||
            shortenedPacket.Contains("F7FF00000100") ||
            shortenedPacket.Contains("F8FF00000100") ||
            shortenedPacket.Contains("F9FF00000100") ||
            shortenedPacket.Contains("FAFF00000100") ||
            shortenedPacket.Contains("FBFF00000100") ||
            shortenedPacket.Contains("FCFF00000100") ||
            shortenedPacket.Contains("FDFF00000100") ||
            shortenedPacket.Contains("FEFF00000100") ||
            shortenedPacket.Contains("FFFF00000100") ||
            shortenedPacket.Contains("01F6FF00000100") ||
            shortenedPacket.Contains("02050000006400");
    }
}

//(*IdInit(RiPE)
const long RiPE::ID_CHECKBOX2 = wxNewId();
const long RiPE::ID_PANEL4 = wxNewId();
const long RiPE::ID_CHECKBOX7 = wxNewId();
const long RiPE::ID_TREECTRL1 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW3 = wxNewId();
const long RiPE::ID_CHECKBOX1 = wxNewId();
const long RiPE::ID_PANEL5 = wxNewId();
const long RiPE::ID_CHECKBOX8 = wxNewId();
const long RiPE::ID_LISTCTRL1 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW1 = wxNewId();
const long RiPE::ID_LISTCTRL3 = wxNewId();
const long RiPE::ID_TEXTCTRL1 = wxNewId();
const long RiPE::ID_BUTTON1 = wxNewId();
const long RiPE::ID_BUTTON4 = wxNewId();
const long RiPE::ID_BUTTON11 = wxNewId();
const long RiPE::ID_BUTTON12 = wxNewId();
const long RiPE::ID_TEXTCTRL9 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW5 = wxNewId();
const long RiPE::ID_STATICTEXT1 = wxNewId();
const long RiPE::ID_STATICTEXT2 = wxNewId();
const long RiPE::ID_STATICTEXT3 = wxNewId();
const long RiPE::ID_PANEL2 = wxNewId();
const long RiPE::ID_PANEL9 = wxNewId();
const long RiPE::ID_TEXTCTRL4 = wxNewId();
const long RiPE::ID_TEXTCTRL5 = wxNewId();
const long RiPE::ID_SPINCTRL1 = wxNewId();
const long RiPE::ID_BUTTON7 = wxNewId();
const long RiPE::ID_BUTTON13 = wxNewId();
const long RiPE::ID_LISTCTRL5 = wxNewId();
const long RiPE::ID_CHECKBOX5 = wxNewId();
const long RiPE::ID_SPINCTRL3 = wxNewId();
const long RiPE::ID_TOGGLEBUTTON1 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW6 = wxNewId();
const long RiPE::ID_CHECKBOX4 = wxNewId();
const long RiPE::ID_PANEL6 = wxNewId();
const long RiPE::ID_CHECKBOX9 = wxNewId();
const long RiPE::ID_TREECTRL2 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW2 = wxNewId();
const long RiPE::ID_CHECKBOX3 = wxNewId();
const long RiPE::ID_PANEL7 = wxNewId();
const long RiPE::ID_CHECKBOX10 = wxNewId();
const long RiPE::ID_LISTCTRL2 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW9 = wxNewId();
const long RiPE::ID_LISTCTRL4 = wxNewId();
const long RiPE::ID_TEXTCTRL3 = wxNewId();
const long RiPE::ID_BUTTON5 = wxNewId();
const long RiPE::ID_BUTTON6 = wxNewId();
const long RiPE::ID_BUTTON10 = wxNewId();
const long RiPE::ID_BUTTON9 = wxNewId();
const long RiPE::ID_TEXTCTRL8 = wxNewId();
const long RiPE::ID_SCROLLEDWINDOW4 = wxNewId();
const long RiPE::ID_BUTTON8 = wxNewId();
const long RiPE::ID_BUTTON14 = wxNewId();
const long RiPE::ID_BUTTON15 = wxNewId();
const long RiPE::ID_CHECKLISTBOX1 = wxNewId();
const long RiPE::ID_BUTTON16 = wxNewId();
const long RiPE::ID_BUTTON17 = wxNewId();
const long RiPE::ID_PANEL3 = wxNewId();
const long RiPE::ID_AUINOTEBOOK1 = wxNewId();
const long RiPE::ID_STATICTEXT4 = wxNewId();
const long RiPE::ID_SPINCTRL2 = wxNewId();
const long RiPE::ID_STATICTEXT5 = wxNewId();
const long RiPE::ID_TEXTCTRL2 = wxNewId();
const long RiPE::ID_BUTTON2 = wxNewId();
const long RiPE::ID_BUTTON3 = wxNewId();
const long RiPE::ID_PANEL1 = wxNewId();
const long RiPE::ID_TIMER1 = wxNewId();
const long RiPE::ID_SAVE = wxNewId();
const long RiPE::ID_SAVE_AS = wxNewId();
const long RiPE::ID_LOAD = wxNewId();
const long RiPE::ID_LOAD_AS = wxNewId();
const long RiPE::ID_LABEL_OPCODES = wxNewId();
const long RiPE::ID_MENUITEM1 = wxNewId();
const long RiPE::ID_RIME = wxNewId();
const long RiPE::ID_RIPESTDATABASE = wxNewId();
const long RiPE::ID_TIMER3 = wxNewId();
const long RiPE::ID_TIMER4 = wxNewId();
//*)
const long RiPE::ID_SELECTPROCESS = wxNewId();
const long RiPE::ID_PANEL8 = wxNewId();
const long RiPE::ID_HTMLWINDOW = wxNewId();

BEGIN_EVENT_TABLE(RiPE,wxFrame)
	//(*EventTable(RiPE)
	//*)
	EVT_MENU(SEND_LIST_CLEAR, RiPE::OnContextMenuDeleteSendList)
	EVT_MENU(RECV_LIST_CLEAR, RiPE::OnContextMenuDeleteRecvList)
	EVT_MENU(SEND_BLOCK_LIST_CLEAR, RiPE::OnContextMenuDeleteSendBlockList)
	EVT_MENU(RECV_BLOCK_LIST_CLEAR, RiPE::OnContextMenuDeleteRecvBlockList)
	EVT_MENU(SEND_TREE_CLEAR, RiPE::OnContextMenuDeleteSendTree)
	EVT_MENU(RECV_TREE_CLEAR, RiPE::OnContextMenuDeleteRecvTree)

	// S - Multi
	EVT_MENU(S_MULTI_SHIFT_UP,    RiPE::OnContextMenuSMultiShiftUp)
	EVT_MENU(S_MULTI_SHIFT_DOWN,  RiPE::OnContextMenuSMultiShiftDown)
	EVT_MENU(S_MULTI_EDIT_NAME,   RiPE::OnContextMenuSMultiEditName)
	EVT_MENU(S_MULTI_EDIT_PACKET, RiPE::OnContextMenuSMultiEditPacket)
	EVT_MENU(S_MULTI_EDIT_DELAY,  RiPE::OnContextMenuSMultiEditDelay)
	EVT_MENU(S_MULTI_DELETE_ITEM, RiPE::OnContextMenuSMultiDeleteItem)

	EVT_CONTEXT_MENU(RiPE::OnContextMenu)
END_EVENT_TABLE()

RiPE::RiPE(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    // prematurely set the RiPEPtr.  Needed to prevent crashes when using plugins
    RiPEApp::SetRiPE(this);
	wxInitAllImageHandlers();
    wxArtProvider::Push(new RiuArtProvider);
	//(*Initialize(RiPE)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer16;
	wxFlexGridSizer* FlexGridSizer19;
	wxFlexGridSizer* FlexGridSizer23;
	wxFlexGridSizer* FlexGridSizer10;
	wxFlexGridSizer* FlexGridSizer3;
	wxMenuItem* MenuItem1;
	wxMenuItem* MenuItem4;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer25;
	wxFlexGridSizer* FlexGridSizer22;
	wxFlexGridSizer* FlexGridSizer9;
	wxFlexGridSizer* FlexGridSizer2;
	wxMenu* Menu1;
	wxFlexGridSizer* FlexGridSizer7;
	wxFlexGridSizer* FlexGridSizer15;
	wxFlexGridSizer* FlexGridSizer18;
	wxFlexGridSizer* FlexGridSizer8;
	wxFlexGridSizer* FlexGridSizer21;
	wxFlexGridSizer* FlexGridSizer14;
	wxFlexGridSizer* FlexGridSizer20;
	wxFlexGridSizer* FlexGridSizer13;
	wxFlexGridSizer* FlexGridSizer12;
	wxFlexGridSizer* FlexGridSizer6;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer11;
	wxFlexGridSizer* FlexGridSizer17;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxFULL_REPAINT_ON_RESIZE, _T("wxID_ANY"));
	SetClientSize(wxSize(648,568));
	wxFont thisFont(10,wxSWISS,wxFONTSTYLE_NORMAL,wxNORMAL,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
	SetFont(thisFont);
	SetIcon( wxIcon(apple_xpm) );
	{
		wxIcon FrameIcon;
		FrameIcon.CopyFromBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("APPLE_XPM")),wxART_FRAME_ICON));
		SetIcon(FrameIcon);
	}
	AuiManager1 = new wxAuiManager(this, wxAUI_MGR_ALLOW_FLOATING|wxAUI_MGR_ALLOW_ACTIVE_PANE|wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_RECTANGLE_HINT|wxAUI_MGR_DEFAULT);
	AuiNotebook1 = new wxAuiNotebook(this, ID_AUINOTEBOOK1, wxPoint(120,336), wxSize(300,-1), wxAUI_NB_TAB_SPLIT|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_EXTERNAL_MOVE|wxAUI_NB_WINDOWLIST_BUTTON);
	AuiNotebook1->SetMinSize(wxSize(300,-1));
	ScrolledWindow3 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW3, wxPoint(118,15), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW3"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableRow(1);
	FlexGridSizer11 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer11->AddGrowableCol(1);
	CheckBox2 = new wxCheckBox(ScrolledWindow3, ID_CHECKBOX2, _("Enable Logging"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	CheckBox2->SetValue(true);
	CheckBox2->SetToolTip(_("When checked, the send tree control will be populated with packets sent to the server."));
	FlexGridSizer11->Add(CheckBox2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	Panel4 = new wxPanel(ScrolledWindow3, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
	FlexGridSizer11->Add(Panel4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	CheckBox7 = new wxCheckBox(ScrolledWindow3, ID_CHECKBOX7, _("Hook Send"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
	CheckBox7->SetValue(false);
	CheckBox7->SetToolTip(_("When enabled, send is hooked.  \nDisabling/Enabling the send hook on the S - Tree tab has the same effect as enabling/disabling it on the S-List tab."));
	FlexGridSizer11->Add(CheckBox7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(FlexGridSizer11, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	TreeCtrl1 = new wxTreeCtrl(ScrolledWindow3, ID_TREECTRL1, wxDefaultPosition, wxSize(141,129), wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
	wxFont TreeCtrl1Font(10,wxSWISS,wxFONTSTYLE_NORMAL,wxNORMAL,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
	TreeCtrl1->SetFont(TreeCtrl1Font);
	FlexGridSizer3->Add(TreeCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow3->SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(ScrolledWindow3);
	FlexGridSizer3->SetSizeHints(ScrolledWindow3);
	ScrolledWindow1 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW1, wxPoint(7,126), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW1"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer10 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer10->AddGrowableCol(1);
	CheckBox1 = new wxCheckBox(ScrolledWindow1, ID_CHECKBOX1, _("Enable Logging"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBox1->SetValue(false);
	CheckBox1->SetToolTip(_("When checked, the send list control will be populated with packets sent to the server."));
	FlexGridSizer10->Add(CheckBox1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	Panel5 = new wxPanel(ScrolledWindow1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
	FlexGridSizer10->Add(Panel5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	CheckBox8 = new wxCheckBox(ScrolledWindow1, ID_CHECKBOX8, _("Hook Send"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
	CheckBox8->SetValue(false);
	CheckBox8->SetToolTip(_("When enabled, send is hooked.  \nDisabling/Enabling the send hook on the S - Tree tab has the same effect as enabling/disabling it on the S-List tab."));
	FlexGridSizer10->Add(CheckBox8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(FlexGridSizer10, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	ListCtrl1 = new wxListCtrl(ScrolledWindow1, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL1"));
	wxFont ListCtrl1Font(10,wxSWISS,wxFONTSTYLE_NORMAL,wxNORMAL,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
	ListCtrl1->SetFont(ListCtrl1Font);
	FlexGridSizer1->Add(ListCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow1->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(ScrolledWindow1);
	FlexGridSizer1->SetSizeHints(ScrolledWindow1);
	ScrolledWindow5 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW5, wxPoint(153,9), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW5"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	FlexGridSizer6->AddGrowableRow(0);
	ListCtrl3 = new wxListCtrl(ScrolledWindow5, ID_LISTCTRL3, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL3"));
	wxFont ListCtrl3Font(10,wxSWISS,wxFONTSTYLE_NORMAL,wxNORMAL,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
	ListCtrl3->SetFont(ListCtrl3Font);
	FlexGridSizer6->Add(ListCtrl3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer7->AddGrowableCol(0);
	FlexGridSizer22 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer22->AddGrowableCol(0);
	TextCtrl1 = new wxTextCtrl(ScrolledWindow5, ID_TEXTCTRL1, _("0a 00 \?\?"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrl1->SetToolTip(_("Enter the text to block or ignore here.  You may use \?\'s or *\'s in the packet you wish to block.  Both \?\'s and *\'s will count as wild cards for a nibble (half of a byte).  \nExample:  5e 00 \?\? ** \?* *\? 0\? 00 00 00\nThe above example will block you from sending meso drop packets of less than 16 mesos.\n\nExtra Commands:\n* Random value between 0-F\n\? Random value between 0-F"));
	FlexGridSizer22->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button1 = new wxButton(ScrolledWindow5, ID_BUTTON1, _("Block"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON1"));
	Button1->SetToolTip(_("Blocks the packet from being sent to the server."));
	Button1->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("BLOCK_PACKET_XPM")),wxART_BUTTON));
	FlexGridSizer22->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button4 = new wxButton(ScrolledWindow5, ID_BUTTON4, _("Ignore"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON4"));
	Button4->SetToolTip(_("Ignores the packet.  The packet will still be sent to the server, but will not be logged by the packet editor."));
	Button4->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("IGNORE_PACKET_XPM")),wxART_BUTTON));
	FlexGridSizer22->Add(Button4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7->Add(FlexGridSizer22, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer23 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer23->AddGrowableCol(2);
	Button11 = new wxButton(ScrolledWindow5, ID_BUTTON11, _("Intercept"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON11"));
	Button11->Disable();
	Button11->Hide();
	Button11->SetToolTip(_("Blocks the packet in the top text box, and sends the packet in the bottom text box in its stead.\n\nNote: The functionality of \?\'s is replaced when using packet interception. \?\'s will not change the original value of a packet.\nBecause of this, \?\'s can be used to skip over nibbles/bytes in a packet without changing them.\nIf you need a random value, use *\'s.\n\nNote: Intercepting packets should be treated as sending packets normally."));
	FlexGridSizer23->Add(Button11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button12 = new wxButton(ScrolledWindow5, ID_BUTTON12, _("Modify"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON12"));
	Button12->SetToolTip(_("Modifies the packet that matches the top text box with information in the bottom text box.  This does not block the original packet.  \n\nNote: The functionality of \?\'s is replaced when modifying packets. \?\'s will not change the original value of a packet.\nBecause of this, \?\'s can be used to skip over nibbles/bytes in a packet without changing them.\nIf you need a random value, use *\'s.\n\nNote: Modified packets are not allowed to be larger than the original packet in size."));
	Button12->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("MODIFY_PACKET_XPM")),wxART_BUTTON));
	FlexGridSizer23->Add(Button12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrl9 = new wxTextCtrl(ScrolledWindow5, ID_TEXTCTRL9, _("\?\? \?\? 0*"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
	TextCtrl9->SetToolTip(_("This text box is only used for intercepting and modifying packtes.\n\nExtra Commands:\n* Random value between 0-F\n\? Value of the original packet (unchanged)\n. Length of modified/intercepted packet is the same as the original packet.  This is almost the same as putting \?\'s for the rest of the packet."));
	FlexGridSizer23->Add(TextCtrl9, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7->Add(FlexGridSizer23, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer6->Add(FlexGridSizer7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow5->SetSizer(FlexGridSizer6);
	FlexGridSizer6->Fit(ScrolledWindow5);
	FlexGridSizer6->SetSizeHints(ScrolledWindow5);
	ScrolledWindow6 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW6, wxPoint(201,20), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW6"));
	FlexGridSizer14 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer14->AddGrowableCol(0);
	FlexGridSizer14->AddGrowableRow(1);
	FlexGridSizer15 = new wxFlexGridSizer(0, 5, 0, 0);
	FlexGridSizer15->AddGrowableCol(1);
	FlexGridSizer15->AddGrowableRow(1);
	StaticText1 = new wxStaticText(ScrolledWindow6, ID_STATICTEXT1, _("Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer15->Add(StaticText1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(ScrolledWindow6, ID_STATICTEXT2, _("Packet"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer15->Add(StaticText2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	StaticText3 = new wxStaticText(ScrolledWindow6, ID_STATICTEXT3, _("Delay (ms)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer15->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	Panel2 = new wxPanel(ScrolledWindow6, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	FlexGridSizer15->Add(Panel2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel9 = new wxPanel(ScrolledWindow6, ID_PANEL9, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL9"));
	FlexGridSizer15->Add(Panel9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrl4 = new wxTextCtrl(ScrolledWindow6, ID_TEXTCTRL4, _("Generic Packet"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	TextCtrl4->SetToolTip(_("Enter a name to help you remember what this packet does here."));
	FlexGridSizer15->Add(TextCtrl4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrl5 = new wxTextCtrl(ScrolledWindow6, ID_TEXTCTRL5, _("A0 00"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	TextCtrl5->SetToolTip(_("Enter the packet here.  You may use XX for random capital ASCII letters, xx for lowercase ASCII letters, and ** for random values (like a time stamp)."));
	FlexGridSizer15->Add(TextCtrl5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrl1 = new wxSpinCtrl(ScrolledWindow6, ID_SPINCTRL1, _T("1000"), wxDefaultPosition, wxSize(85,-1), 0, 1, 100000000, 1000, _T("ID_SPINCTRL1"));
	SpinCtrl1->SetValue(_T("1000"));
	SpinCtrl1->SetToolTip(_("Enter the delay for this packet in milli-seconds."));
	FlexGridSizer15->Add(SpinCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button7 = new wxButton(ScrolledWindow6, ID_BUTTON7, _("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON7"));
	Button7->SetToolTip(_("Add a send packet to the end of the list."));
	FlexGridSizer15->Add(Button7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button13 = new wxButton(ScrolledWindow6, ID_BUTTON13, _("Recv"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON13"));
	Button13->SetToolTip(_("Add a recv packet to the end of the list."));
	FlexGridSizer15->Add(Button13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer14->Add(FlexGridSizer15, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ListCtrl5 = new wxListCtrl(ScrolledWindow6, ID_LISTCTRL5, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL5"));
	ListCtrl5->SetToolTip(_("Double-clicking on a packet will send it."));
	FlexGridSizer14->Add(ListCtrl5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer16 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer16->AddGrowableCol(2);
	CheckBox5 = new wxCheckBox(ScrolledWindow6, ID_CHECKBOX5, _("Act as Group"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	CheckBox5->SetValue(false);
	CheckBox5->SetToolTip(_("When checked, packets will be sent, in order, from top to bottom.\nThe delay of the next packet will be that packet\'s delay.\n\nExample:\nFirst packet has a delay of 1000 ms\nSecond packet has a delay of 333 ms\nFirst packet is sent after 1000 ms\nSecond packet is sent 333 ms after the first packet."));
	FlexGridSizer16->Add(CheckBox5, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrl3 = new wxSpinCtrl(ScrolledWindow6, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxSize(70,-1), 0, 0, 100000, 0, _T("ID_SPINCTRL3"));
	SpinCtrl3->SetValue(_T("0"));
	SpinCtrl3->Hide();
	SpinCtrl3->SetToolTip(_("The socket number.  You can get this value from S - Tree or R - Tree to the right of the packet header.  This value must be correct if you want packet sending to work."));
	FlexGridSizer16->Add(SpinCtrl3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ToggleButton1 = new wxToggleButton(ScrolledWindow6, ID_TOGGLEBUTTON1, _("Start Spamming"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
	ToggleButton1->SetToolTip(_("Start/Stop spamming packets."));
	FlexGridSizer16->Add(ToggleButton1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer14->Add(FlexGridSizer16, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow6->SetSizer(FlexGridSizer14);
	FlexGridSizer14->Fit(ScrolledWindow6);
	FlexGridSizer14->SetSizeHints(ScrolledWindow6);
	ScrolledWindow2 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW2, wxPoint(333,18), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW2"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer4->AddGrowableRow(1);
	FlexGridSizer13 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer13->AddGrowableCol(1);
	CheckBox4 = new wxCheckBox(ScrolledWindow2, ID_CHECKBOX4, _("Enable Logging"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	CheckBox4->SetValue(true);
	CheckBox4->SetToolTip(_("When checked, the receive tree control will be populated with packets received from the server."));
	FlexGridSizer13->Add(CheckBox4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel6 = new wxPanel(ScrolledWindow2, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
	FlexGridSizer13->Add(Panel6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	CheckBox9 = new wxCheckBox(ScrolledWindow2, ID_CHECKBOX9, _("Hook Recv"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
	CheckBox9->SetValue(false);
	CheckBox9->SetToolTip(_("When enabled, the receive packet functions are hooked.  \nDisabling/Enabling the receive hook on the R - Tree tab has the same effect as enabling/disabling it on the R-List tab."));
	FlexGridSizer13->Add(CheckBox9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4->Add(FlexGridSizer13, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	TreeCtrl2 = new wxTreeCtrl(ScrolledWindow2, ID_TREECTRL2, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL2"));
	FlexGridSizer4->Add(TreeCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow2->SetSizer(FlexGridSizer4);
	FlexGridSizer4->Fit(ScrolledWindow2);
	FlexGridSizer4->SetSizeHints(ScrolledWindow2);
	ScrolledWindow9 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW9, wxPoint(255,14), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW9"));
	ScrolledWindow9->SetToolTip(_("When checked, the receive list control will be populated with packets received from the server."));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(1);
	FlexGridSizer12 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer12->AddGrowableCol(1);
	CheckBox3 = new wxCheckBox(ScrolledWindow9, ID_CHECKBOX3, _("Enable Logging"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	CheckBox3->SetValue(false);
	CheckBox3->SetToolTip(_("When checked, the receive list control will be populated with packets received from the server."));
	FlexGridSizer12->Add(CheckBox3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	Panel7 = new wxPanel(ScrolledWindow9, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
	FlexGridSizer12->Add(Panel7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	CheckBox10 = new wxCheckBox(ScrolledWindow9, ID_CHECKBOX10, _("Hook Recv"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
	CheckBox10->SetValue(false);
	CheckBox10->SetToolTip(_("When enabled, the receive packet functions are hooked.  \nDisabling/Enabling the receive hook on the R - Tree tab has the same effect as enabling/disabling it on the R-List tab."));
	FlexGridSizer12->Add(CheckBox10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2->Add(FlexGridSizer12, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	ListCtrl2 = new wxListCtrl(ScrolledWindow9, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL2"));
	FlexGridSizer2->Add(ListCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow9->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(ScrolledWindow9);
	FlexGridSizer2->SetSizeHints(ScrolledWindow9);
	ScrolledWindow4 = new wxScrolledWindow(AuiNotebook1, ID_SCROLLEDWINDOW4, wxPoint(366,20), wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW4"));
	FlexGridSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer8->AddGrowableCol(0);
	FlexGridSizer8->AddGrowableRow(0);
	ListCtrl4 = new wxListCtrl(ScrolledWindow4, ID_LISTCTRL4, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL4"));
	FlexGridSizer8->Add(ListCtrl4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer9 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer9->AddGrowableCol(0);
	FlexGridSizer9->AddGrowableRow(0);
	FlexGridSizer20 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer20->AddGrowableCol(0);
	TextCtrl3 = new wxTextCtrl(ScrolledWindow4, ID_TEXTCTRL3, _("0a 00 \?\?"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextCtrl3->SetToolTip(_("Enter the text to block or ignore here.  You may use \?\'s or *\'s in the packet you wish to block.  Both \?\'s and *\'s will count as wild cards for a nibble (half of a byte).  \nExample:  5e 00 \?\? ** \?* *\? 0\? 00 00 00\nThe above example will block you from sending meso drop packets of less than 16 mesos.\n\nExtra Commands:\n* Random value between 0-F\n\? Random value between 0-F"));
	FlexGridSizer20->Add(TextCtrl3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button5 = new wxButton(ScrolledWindow4, ID_BUTTON5, _("Block"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON5"));
	Button5->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("BLOCK_PACKET_XPM")),wxART_BUTTON));
	FlexGridSizer20->Add(Button5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button6 = new wxButton(ScrolledWindow4, ID_BUTTON6, _("Ignore"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON6"));
	Button6->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("IGNORE_PACKET_XPM")),wxART_BUTTON));
	FlexGridSizer20->Add(Button6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer9->Add(FlexGridSizer20, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer21 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer21->AddGrowableCol(2);
	Button10 = new wxButton(ScrolledWindow4, ID_BUTTON10, _("Intercept"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON10"));
	Button10->Disable();
	Button10->Hide();
	Button10->SetToolTip(_("Blocks the packet in the top text box, and sends the packet in the bottom text box in its stead.\n\nNote: Intercepting packets should be treated as sending packets normally.\n\nNote: The same limitations of sending receive packets apply."));
	FlexGridSizer21->Add(Button10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button9 = new wxButton(ScrolledWindow4, ID_BUTTON9, _("Modify"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON9"));
	Button9->SetToolTip(_("Modifies the packet that matches the top text box with information in the bottom text box.  This does not block the original packet.  \n\nNote: The functionality of \?\'s is replaced when modifying packets. \?\'s will not change the original value of a packet.\nBecause of this, \?\'s can be used to skip over nibbles/bytes in a packet without changing them.\nIf you need a random value, use *\'s.\n\nNote: Modified packets are not allowed to be large than the original packet in size."));
	Button9->SetBitmap( wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("MODIFY_PACKET_XPM")),wxART_BUTTON));
	FlexGridSizer21->Add(Button9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrl8 = new wxTextCtrl(ScrolledWindow4, ID_TEXTCTRL8, _("\?\? \?\? 0*"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	TextCtrl8->SetToolTip(_("This text box is only used for intercepting and modifying packtes.\n\nExtra Commands:\n* Random value between 0-F\n\? Value of the original packet (unchanged)\n. Length of modified/intercepted packet is the same as the original packet.  This is almost the same as putting \?\'s for the rest of the packet."));
	FlexGridSizer21->Add(TextCtrl8, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer9->Add(FlexGridSizer21, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer8->Add(FlexGridSizer9, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ScrolledWindow4->SetSizer(FlexGridSizer8);
	FlexGridSizer8->Fit(ScrolledWindow4);
	FlexGridSizer8->SetSizeHints(ScrolledWindow4);
	Panel3 = new wxPanel(AuiNotebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
	FlexGridSizer17 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer17->AddGrowableCol(0);
	FlexGridSizer17->AddGrowableRow(1);
	FlexGridSizer18 = new wxFlexGridSizer(0, 3, 0, 0);
	Button8 = new wxButton(Panel3, ID_BUTTON8, _("Find Scripts"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
	Button8->SetHelpText(_("Find scripts with the online database tool."));
	FlexGridSizer18->Add(Button8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button14 = new wxButton(Panel3, ID_BUTTON14, _("Load Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON14"));
	Button14->SetHelpText(_("Load script from a file."));
	FlexGridSizer18->Add(Button14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button15 = new wxButton(Panel3, ID_BUTTON15, _("New Script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON15"));
	Button15->SetHelpText(StringMangler::Decode("P rn\"r-n-{r%-!p v}\"-v{-\"ur-_v]R`a-rqv\"| ;"));
	FlexGridSizer18->Add(Button15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer17->Add(FlexGridSizer18, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel3, _("Scripts"));
	FlexGridSizer19 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer19->AddGrowableCol(0);
	FlexGridSizer19->AddGrowableRow(0);
	CheckListBox1 = new wxCheckListBox(Panel3, ID_CHECKLISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, wxLB_SORT, wxDefaultValidator, _T("ID_CHECKLISTBOX1"));
	FlexGridSizer19->Add(CheckListBox1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer25 = new wxFlexGridSizer(0, 3, 0, 0);
	Button16 = new wxButton(Panel3, ID_BUTTON16, _("Delete Selected"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON16"));
	Button16->SetHelpText(_("Delete the selected script.  This will remove the script from your hard drive, so be careful!"));
	FlexGridSizer25->Add(Button16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button17 = new wxButton(Panel3, ID_BUTTON17, _("Delete All"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON17"));
	Button17->Disable();
	FlexGridSizer25->Add(Button17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer19->Add(FlexGridSizer25, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1->Add(FlexGridSizer19, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer17->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel3->SetSizer(FlexGridSizer17);
	FlexGridSizer17->Fit(Panel3);
	FlexGridSizer17->SetSizeHints(Panel3);
	AuiNotebook1->AddPage(ScrolledWindow3, _("S - Tree"), false, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("STREE_XPM")),wxART_BUTTON));
	AuiNotebook1->AddPage(ScrolledWindow1, _("S - List"));
	AuiNotebook1->AddPage(ScrolledWindow5, _("S - Block"));
	AuiNotebook1->AddPage(ScrolledWindow6, _("Multi"), false, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("SMULTI_XPM")),wxART_BUTTON));
	AuiNotebook1->AddPage(ScrolledWindow2, _("R - Tree"), false, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("RTREE_XPM")),wxART_BUTTON));
	AuiNotebook1->AddPage(ScrolledWindow9, _("R - List"));
	AuiNotebook1->AddPage(ScrolledWindow4, _("R - Block"));
	AuiNotebook1->AddPage(Panel3, StringMangler::Decode("_v]R`a"));
	AuiManager1->AddPane(AuiNotebook1, wxAuiPaneInfo().Name(_T("SendReceiveNotebook")).Caption(_("Send")).CaptionVisible(false).CloseButton(false).Center().BestSize(wxSize(300,-1)).MinSize(wxSize(300,-1)));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(345,547), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	AddSponsorPanel();
	FlexGridSizer5 = new wxFlexGridSizer(0, 6, 0, 0);
	FlexGridSizer5->AddGrowableCol(3);
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Socket:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticText4->Hide();
	StaticText4->SetToolTip(StringMangler::Decode("aur-!|pxr\"-{#zor -v!-\"|-\"ur- vtu\";--f|#-pn{-tr\"-\"uv!-$ny#r-s |z-`-:-a rr-| -_-:-a rr-\"|-\"ur- vtu\"-|s-\"ur-}npxr\"-urnqr ;--auv!-$ny#r-z#!\"-or-p|  rp\"-vs-'|#-%n{\"-}npxr\"-!r{qv{t-\"|-%| x;"));
	FlexGridSizer5->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrl2 = new wxSpinCtrl(Panel1, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxSize(70,20), 0, 0, 100000, 0, _T("ID_SPINCTRL2"));
	SpinCtrl2->SetValue(_T("0"));
	SpinCtrl2->Hide();
	SpinCtrl2->SetToolTip(_("The socket number.  You can get this value from S - Tree or R - Tree to the right of the packet header.  This value must be correct if you want packet sending to work."));
	FlexGridSizer5->Add(SpinCtrl2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Packet:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText5->SetToolTip(_("The packet should be to the right."));
	FlexGridSizer5->Add(StaticText5, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl2 = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("0A00"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrl2->SetToolTip(_("This is the main textbox for sending/receiving packets to and from the program.\nNote the following features:\nXX will create a random capital letter.\nxx will create a random lowercase letter.\n*  will create a random value from 0-F.  You can use ** for a random byte."));
	FlexGridSizer5->Add(TextCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON2"));
	Button2->SetMaxSize(wxSize(-1,-1));
	Button2->SetToolTip(_("Clicking on this button will send the packet in the textbox to the left to the server."));
	FlexGridSizer5->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button3 = new wxButton(Panel1, ID_BUTTON3, _("Receive"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
	Button3->SetToolTip(_("Clicking on this button will \"send\" a receive packet in the textbox to the left to the client."));
	FlexGridSizer5->Add(Button3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel1->SetSizer(FlexGridSizer5);
	FlexGridSizer5->Fit(Panel1);
	FlexGridSizer5->SetSizeHints(Panel1);
	AuiManager1->AddPane(Panel1, wxAuiPaneInfo().Name(_T("SendReceivePackets")).Caption(_("Send Packets")).CaptionVisible().CloseButton(false).Bottom());
	AuiManager1->Update();
	Timer1.SetOwner(this, ID_TIMER1);
	MenuBar1 = new wxMenuBar();
	Menu1 = new wxMenu();
	MenuItem1 = new wxMenuItem(Menu1, ID_SAVE, _("Quick &Save"), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(MenuItem1);
	MenuItem2 = new wxMenuItem(Menu1, ID_SAVE_AS, _("Save &As...\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);
	MenuItem2->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE")),wxART_OTHER));
	Menu1->Append(MenuItem2);
	MenuItem3 = new wxMenuItem(Menu1, ID_LOAD, _("Quick &Load"), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(MenuItem3);
	MenuItem4 = new wxMenuItem(Menu1, ID_LOAD_AS, _("Load &From...\tCtrl+L"), wxEmptyString, wxITEM_NORMAL);
	MenuItem4->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_OTHER));
	Menu1->Append(MenuItem4);
	MenuBar1->Append(Menu1, _("&File"));
    MenuProcess = new wxMenu();
    MenuItemSelectProcess = new wxMenuItem(MenuProcess, ID_SELECTPROCESS, _("Select Process"), wxEmptyString, wxITEM_NORMAL);
    MenuProcess->Append(MenuItemSelectProcess);
    MenuBar1->Append(MenuProcess, _("&Process"));
	Menu2 = new wxMenu();
	MenuItem6 = new wxMenuItem(Menu2, ID_LABEL_OPCODES, _("Label Opcodes"), wxEmptyString, wxITEM_NORMAL);
	Menu2->Append(MenuItem6);
	MenuItem5 = new wxMenuItem(Menu2, ID_MENUITEM1, _("Select Game/Version"), wxEmptyString, wxITEM_NORMAL);
	Menu2->Append(MenuItem5);
//  RiPE no longer supports RiME (yet)
//  MenuItem7 = new wxMenuItem(Menu2, ID_RIME, StringMangler::Decode("_vZR"), wxEmptyString, wxITEM_NORMAL);
//  Menu2->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu2, ID_RIPESTDATABASE, StringMangler::Decode("_v]R`a-Qn\"non!r"), wxEmptyString, wxITEM_NORMAL);
    Menu2->Append(MenuItem8);
	MenuBar1->Append(Menu2, _("&Tools"));
	SetMenuBar(MenuBar1);
	Timer3.SetOwner(this, ID_TIMER3);
	Timer3.Start(240000, false);
	Timer4.SetOwner(this, ID_TIMER4);
	Timer4.Start(10, false);

	Connect(ID_CHECKBOX7,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RiPE::OnCheckBox7Click);
	Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&RiPE::OnTreeCtrl1ItemActivated);
	Connect(ID_CHECKBOX8,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RiPE::OnCheckBox8Click);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS,(wxObjectEventFunction)&RiPE::OnListCtrl1DeleteAllItems);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&RiPE::OnListCtrl1ItemActivated);
	Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_KEY_DOWN,(wxObjectEventFunction)&RiPE::OnListCtrl3KeyDown);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&RiPE::OnTextCtrl1Text);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton1Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton4Click);
	Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton11Click);
	Connect(ID_BUTTON12,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton12Click);
	Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton7Click);
	Connect(ID_BUTTON13,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton13Click);
	Connect(ID_LISTCTRL5,wxEVT_COMMAND_LIST_BEGIN_DRAG,(wxObjectEventFunction)&RiPE::OnListCtrl5BeginDrag);
	Connect(ID_LISTCTRL5,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&RiPE::OnListCtrl5ItemActivated);
	Connect(ID_LISTCTRL5,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&RiPE::OnListCtrl5ItemRClick);
	Connect(ID_LISTCTRL5,wxEVT_COMMAND_LIST_KEY_DOWN,(wxObjectEventFunction)&RiPE::OnListCtrl5KeyDown);
	Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RiPE::OnCheckBox5Click);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&RiPE::OnSpinCtrl3Change);
	Connect(ID_TOGGLEBUTTON1,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnToggleButton1Toggle);
	Connect(ID_CHECKBOX9,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RiPE::OnCheckBox9Click);
	Connect(ID_TREECTRL2,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&RiPE::OnTreeCtrl2ItemActivated);
	Connect(ID_CHECKBOX10,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RiPE::OnCheckBox10Click);
	Connect(ID_LISTCTRL2,wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS,(wxObjectEventFunction)&RiPE::OnListCtrl2DeleteAllItems);
	Connect(ID_LISTCTRL2,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&RiPE::OnListCtrl2ItemActivated);
	Connect(ID_LISTCTRL4,wxEVT_COMMAND_LIST_KEY_DOWN,(wxObjectEventFunction)&RiPE::OnListCtrl4KeyDown);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton5Click);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton6Click);
	Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton10Click);
	Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton9Click);
	Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton8Click2);
	Connect(ID_BUTTON14,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton14Click);
	Connect(ID_BUTTON15,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton15Click);
	Connect(ID_BUTTON16,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton16Click);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&RiPE::OnSpinCtrl2Change);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton2Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RiPE::OnButton3Click);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&RiPE::OnTimer1Trigger);
	Connect(ID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem1Selected);
	Connect(ID_SAVE_AS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem2Selected);
	Connect(ID_LOAD,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem3Selected);
	Connect(ID_LOAD_AS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem4Selected);
	Connect(ID_LABEL_OPCODES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem6Selected);
	Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem5Selected);
	Connect(ID_RIME,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItem7Selected);
    Connect(ID_RIPESTDATABASE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnRiPESTDatabase);
	Connect(ID_TIMER3,wxEVT_TIMER,(wxObjectEventFunction)&RiPE::OnTimer3Trigger);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&RiPE::OnClose);
	//*)
    Connect(ID_SELECTPROCESS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&RiPE::OnMenuItemSelectProcess);
    Connect(ID_TIMER4,wxEVT_TIMER,(wxObjectEventFunction)&RiPE::OnTimerLogTrigger);
    Connect(ID_CHECKLISTBOX1, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,(wxObjectEventFunction)&RiPE::OnCheckListBoxToggled);
    Connect(ID_HTMLWINDOW,wxEVT_COMMAND_WEB_VIEW_NAVIGATING,(wxObjectEventFunction)&RiPE::OnHtmlWindowHtmlLinkClicked);
	//Assign group ID's to -1 starting
	sendGroupItemId = -1;
	recvGroupItemId = -1;

    // Go ahead and inject into our first process
    igFrame = new InjectorGadgetFrame(this);
    igFrame->ShowModal();
    // We now need to initialize our connection
    communicationLayer.Connect(igFrame->GetPID());

    scriptDlg = NULL;
    psDlg = NULL; // plugin search dialog

#ifdef __RIPE_STANDALONE
	srand( time(NULL) );
#endif
    ImageListSmall = new wxImageList(16, 16, true);
   // ImageListSmall->Add( wxIcon( _("aaaa"), wxBITMAP_TYPE_ICO_RESOURCE) );
    //ImageListSmall->Add( wxIcon( toolchar_xpm ) );
    //ImageListSmall->Add( wxIcon( apple_xpm ) );
    ListCtrl1->SetImageList(ImageListSmall, wxIMAGE_LIST_SMALL);
    wxListItem itemCol;
    itemCol.SetText( _("Address") );
    //itemCol.SetImage( 0 );
	//Add columns to Send - List Control
    ListCtrl1->InsertColumn( 0, itemCol );
	//ListCtrl1->InsertColumn(0, L"Address", wxLIST_FORMAT_LEFT, 61);
	ListCtrl1->InsertColumn(1, L"Size", wxLIST_FORMAT_LEFT, 36);
	ListCtrl1->InsertColumn(2, L"Packet", wxLIST_FORMAT_LEFT, -1);

	//Add root to Send - Tree Control
	TreeCtrl1->AddRoot(_("Root"));

	//Add columns to Send - Block
	ListCtrl3->InsertColumn(0, L"Type", wxLIST_FORMAT_LEFT, 44);
	ListCtrl3->InsertColumn(1, L"Packet", wxLIST_FORMAT_LEFT, -1);
	ListCtrl3->InsertColumn(2, L"Replace", wxLIST_FORMAT_LEFT, -1);

	//Add columns to Multi
	ListCtrl5->InsertColumn(0, L"Name", wxLIST_FORMAT_LEFT, 80);
	ListCtrl5->InsertColumn(1, L"Type", wxLIST_FORMAT_LEFT, 40);
	ListCtrl5->InsertColumn(2, L"Packet", wxLIST_FORMAT_LEFT, 160);
	ListCtrl5->InsertColumn(3, L"Delay", wxLIST_FORMAT_LEFT, 50);

	//Add columns to Recv - List Control
	ListCtrl2->InsertColumn(0, L"Address", wxLIST_FORMAT_LEFT, 61);
	ListCtrl2->InsertColumn(1, L"Size", wxLIST_FORMAT_LEFT, 36);
	ListCtrl2->InsertColumn(2, L"Packet", wxLIST_FORMAT_LEFT, -1);

	//Add root to Recv - Tree Control
	TreeCtrl2->AddRoot(_("Root"));

	//Add columns to Recv - Block
	ListCtrl4->InsertColumn(0, L"Type", wxLIST_FORMAT_LEFT, 44);
	ListCtrl4->InsertColumn(1, L"Packet", wxLIST_FORMAT_LEFT, -1);
	ListCtrl4->InsertColumn(2, L"Replace", wxLIST_FORMAT_LEFT, -1);

    opcodesDlg = NULL;

    gsmDlg   = new gameSelectMenu(this, -1);
	LoadLayout();

    SelectGame();

    canLogSend = true;
    canLogRecv = true;

    communicationLayer.SetHookInterface(hookIntf);
    LoadScripts();

    m_hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)::ProcessHotKey, NULL, 0);
}

RiPE::~RiPE()
{
	SaveLayout();
    ChangeSelf();
	//(*Destroy(RiPE)
	//*)

    communicationLayer.HookRecv(false);
    communicationLayer.HookSend(false);

    if( opcodesDlg != NULL ) {
        opcodesDlg->Destroy();
    }
    if( scriptDlg != NULL ) {
        scriptDlg->Destroy();
    }
    if( psDlg != NULL )
    {
        psDlg->Destroy();
    }
    if (igFrame != NULL) {
        igFrame->Destroy();
    }

    if (m_hook != NULL) {
        UnhookWindowsHookEx(m_hook);
    }

    gsmDlg->Destroy();
    //delete adDlg;
	AuiManager1->UnInit(); //Uninitializes the Aui Manager.  Without this, we crash on closing.
    RiPEApp::SetRiPE(NULL);
}

// When the "Select Process" menu needs to be opened
void RiPE::OnMenuItemSelectProcess(wxCommandEvent& event) {
    igFrame = new InjectorGadgetFrame(this);
    igFrame->ShowModal();

    // Disable all scripts in the RiPEST tab
    for (unsigned int i=0; i < CheckListBox1->GetCount(); ++i) {
        CheckListBox1->Check(i, false);
    }

    // We now need to initialize our connection
    communicationLayer.Connect(igFrame->GetPID());
}

// Calls the updater to change the .dll slightly
// Note that the parent MUST kill itself very shortly after this.
void RiPE::ChangeSelf()
{
    wxString filename = RiPEApp::GetFilePath().BeforeLast('\\') + StringMangler::Decode("<_v]Rb}qn\"r ;r&r");

    // The updater exists.  Replace the file
    if( wxFileExists(filename) )
    {
        wxString launchCommand = filename + wxT(" \"") + RiPEApp::GetFilePath() + wxT("\"");
        wxEvtHandler *eventHandler = NULL;
        wxProcess *proc = new wxProcess(eventHandler);
        proc->Redirect();
        wxExecute(launchCommand, wxEXEC_ASYNC, proc);
        proc->Detach();
    }
}

void RiPE::OnAuiNotebook4PageClose(wxAuiNotebookEvent& event)
{
}

void RiPE::AddSponsorPanel()
{
    wxFlexGridSizer *flexGridSizer;
    Panel8 = new wxPanel(this, ID_PANEL8, wxPoint(407,564), wxSize(-1,114), wxNO_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL8"));
    flexGridSizer = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer->AddGrowableCol(0);
    flexGridSizer->AddGrowableRow(0);
    adCtrl = wxWebView::GetNew(Panel8, ID_HTMLWINDOW);
    adCtrl->Connect(ID_HTMLWINDOW, wxEVT_COMMAND_WEB_VIEW_NAVIGATING, wxWebNavigationEventHandler(RiPE::OnHtmlWindowHtmlLinkClicked), NULL, this);

    if( !AuthenticationPulse( StringMangler::Decode("u\"\"}G<<%%%; v#x#(nxv;p|z<Nq<nq_v]R;\"&\"") ) )
    {
        //Close();
    }

    flexGridSizer->Add(adCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel8->SetSizer(flexGridSizer);
    flexGridSizer->SetSizeHints(Panel8);
    AuiManager1->AddPane(Panel8, wxAuiPaneInfo().Name(_T("Sponsors")).Caption(_("Sponsors")).CaptionVisible().CloseButton(false).Layer(1).Bottom().DockFixed().Floatable(false).FloatingSize(wxSize(800,600)).BestSize(wxSize(-1,114)).Movable(false).PaneBorder(false));
}

void RiPE::OnClose(wxCloseEvent& event)
{
#ifdef __RIPE_STANDALONE
    event.Skip();
#else
    Hide();
#ifdef USING_PACKET_EDITOR
    hookIntf->HookSend( false );
    hookIntf->HookRecv( false );
#endif
#endif
	SaveLayout();
}

// Send
void RiPE::OnButton2Click(wxCommandEvent& event)
{
    StoredPacketItem pkt( TextCtrl2->GetValue() );

    switch(SendPacketToProcess( std::string(pkt.GetPacket().mb_str()), communicationLayer, __SEND_PACKET, SpinCtrl2->GetValue() ))
	{
	case __INVALID_LENGTH:
		wxMessageBox(_("Invalid length.  Packets must contain at least a few bytes before sending them."), _("Error Sending Packet"), wxICON_ERROR);
        break;
	case __NOT_EVEN:
		wxMessageBox(_("Invalid length.  Make sure that your packet has an even number of characters.\nOnly valid hexadecimal numbers are allowed."), _("Error Sending Packet"), wxICON_ERROR);
        break;
	case __DC_HACK:
		wxMessageBox(_("DC hack attempt detected.  Please do not use my packet editor for malicious purposes like DC hacking.\n\nThanks,\n~Riu."), _("DC Hack Alert"), wxICON_ERROR);
        break;
	}
}

// S - List
void RiPE::OnListCtrl1ItemActivated(wxListEvent& event)
{
	wxString *tmpstr = (wxString *)(ListCtrl1->GetItemData(event.GetItem()));
	if( tmpstr )
	    TextCtrl2->SetLabel( *tmpstr );
}

// R - List
void RiPE::OnListCtrl2ItemActivated(wxListEvent& event)
{
    wxString *tmpstr = (wxString *)(ListCtrl2->GetItemData(event.GetItem()));
    if( tmpstr )
        TextCtrl2->SetLabel( *tmpstr );
}

void RiPE::OnContextMenu(wxContextMenuEvent& event)
{
	wxMenu  menu;
	wxPoint point = event.GetPosition();
    // If from keyboard
    if ( (point.x == -1) && (point.y == -1) )
    {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
    else
    {
        point = ScreenToClient(point);
    }

	menu.Append(SEND_TREE_CLEAR, _("Clear &Send Tree"));
	menu.Append(RECV_TREE_CLEAR, _("Clear &Recv Tree"));
	menu.AppendSeparator();
	menu.Append(SEND_LIST_CLEAR, _("Clear &Send List"));
	menu.Append(RECV_LIST_CLEAR, _("Clear &Recv List"));
	menu.AppendSeparator();
	menu.Append(SEND_BLOCK_LIST_CLEAR, _("Clear Send - &Block and Ignore List"));
	menu.Append(RECV_BLOCK_LIST_CLEAR, _("Clear Recv - &Block and Ignore List"));
    PopupMenu(&menu, point.x, point.y);
}

void RiPE::OnListCtrl1DeleteAllItems(wxListEvent& event)
{
	//ListCtrl1->DeleteAllItems();
}

void RiPE::OnListCtrl2DeleteAllItems(wxListEvent& event)
{
	//ListCtrl2->DeleteAllItems();
}

void RiPE::OnContextMenuDeleteSendList(wxCommandEvent& WXUNUSED(event))
{
	canLogSend = false; //Block additional packets from being added
	Sleep(400); //Experimental prevention of badness
	long index = -1;
	for( ;; ) //Delete the pointer data
	{
		index = ListCtrl1->GetNextItem(index);
		if( index == -1 || ListCtrl1->GetItemCount() == 0 )
            break;
		wxString *tempPtr = (wxString *)ListCtrl1->GetItemData(index);
		if( tempPtr != NULL )
		{
			delete tempPtr;
			tempPtr = NULL;
		}
	}
	ListCtrl1->DeleteAllItems();
	Sleep(40);

	canLogSend = true; //Resume packets
}

void RiPE::OnContextMenuDeleteRecvList(wxCommandEvent& WXUNUSED(event))
{
#ifdef USING_PACKET_EDITOR
	canLogRecv = false;
	Sleep(400); //Experimental prevention of badness
	long index = -1;
	for( ;; ) //Delete the pointer data
	{
		index = ListCtrl2->GetNextItem(index);
		if( index == -1 || ListCtrl2->GetItemCount() == 0 )
            break;
		wxString *tempPtr = (wxString *)ListCtrl2->GetItemData(index);
		if(tempPtr != NULL)
		{
			delete tempPtr;
			tempPtr = NULL;
		}
	}
	ListCtrl2->DeleteAllItems();
	Sleep(40);

	canLogRecv = true;
#endif
}

void RiPE::OnContextMenuDeleteSendBlockList(wxCommandEvent& WXUNUSED(event))
{
	ListCtrl3->DeleteAllItems();
}

void RiPE::OnContextMenuDeleteRecvBlockList(wxCommandEvent& WXUNUSED(event))
{
	ListCtrl4->DeleteAllItems();
}

void RiPE::OnContextMenuDeleteSendTree(wxCommandEvent& WXUNUSED(event))
{
#ifdef USING_PACKET_EDITOR
	canLogSend = false;
	Sleep(50); //Experimental prevention of badness
	TreeCtrl1->DeleteChildren( TreeCtrl1->GetRootItem() );
	canLogSend = true;
#endif
}

void RiPE::OnContextMenuDeleteRecvTree(wxCommandEvent& WXUNUSED(event))
{
#ifdef USING_PACKET_EDITOR
	canLogRecv = false;
	Sleep(50); //Experimental prevention of badness
	TreeCtrl2->DeleteChildren( TreeCtrl2->GetRootItem() );
	canLogRecv = true;
#endif
}

void RiPE::OnContextMenuSMultiShiftUp(wxCommandEvent& WXUNUSED(event))
{

}

void RiPE::OnContextMenuSMultiShiftDown(wxCommandEvent& WXUNUSED(event))
{

}

void RiPE::OnContextMenuSMultiEditName(wxCommandEvent& WXUNUSED(event))
{

}

void RiPE::OnContextMenuSMultiEditPacket(wxCommandEvent& WXUNUSED(event))
{

}

void RiPE::OnContextMenuSMultiEditDelay(wxCommandEvent& WXUNUSED(event))
{

}

void RiPE::OnContextMenuSMultiDeleteItem(wxCommandEvent& WXUNUSED(event))
{

}

// Recv packet
void RiPE::OnButton3Click(wxCommandEvent& event)
{
    StoredPacketItem pkt(TextCtrl2->GetValue());
    switch(SendPacketToProcess(std::string(pkt.GetPacket().mb_str()), communicationLayer, __RECV_PACKET, SpinCtrl2->GetValue() ))
	{
	  case __INVALID_LENGTH:
		wxMessageBox(_("Invalid length.  Packets must contain at least a few bytes before sending them."), _("Error Sending Packet"));
        break;
	  case __NOT_EVEN:
	    wxMessageBox(_("Invalid length.  Make sure that your packet has an even number of characters.\nOnly valid hexadecimal numbers are allowed."), _("Error Sending Packet"));
        break;
	  case __DC_HACK:
		wxMessageBox(_("DC hack attempt detected.  Please do not use my packet editor for malicious purposes like DC hacking.\n\nThanks,\n~Riu."), _("DC Hack Alert"));
        break;
	  case __TOO_LONG:
	    wxMessageBox(_("The packet is too long.  As of now, the packet length may not exceed 18 bytes.  Sorry!"), _("Too Long"));
        break;
	}
}

void RiPE::OnTextCtrl1Text(wxCommandEvent& event)
{
}

//On "Block" button event
void RiPE::OnButton1Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl3, TextCtrl1, PACKETTYPE_BLOCK))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packet you wish to block."), _("Error Blocking Packet"));
}

//On "Ignore" button event
void RiPE::OnButton4Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl3, TextCtrl1, PACKETTYPE_IGNORE))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packet you wish to ignore."), _("Error Ignoring Packet"));
}

//On "Block" button event for receive packets
void RiPE::OnButton5Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl4, TextCtrl3, PACKETTYPE_BLOCK))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packet you wish to block."), _("Error Blocking Packet"));
}

//On "Ignore" button event for receive packets
void RiPE::OnButton6Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl4, TextCtrl3, PACKETTYPE_IGNORE))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packet you wish to ignore."), _("Error Ignoring Packet"));
}

//Intercept the link click and open the ad with the default browser.
void RiPE::OnHtmlWindowHtmlLinkClicked(wxWebNavigationEvent& event)
{
    // if docked and the timer for the advertisement has expired
    if( AuiManager1->GetPane(_("Sponsors")).IsDocked() )
    {
        if( event.GetHref().Find(StringMangler::Decode(" v#x#(nxv;p|z")) == wxNOT_FOUND ) { // if not part of riukuzaki.com
            //wxLaunchDefaultBrowser( event.GetHref() ); // launch ad in default browser
            //event.Veto();
            AuiManager1->GetPane(_("Sponsors")).CloseButton(true).Floatable(true).Resizable(true).Dockable(false).Float();
            AuiManager1->Update();
        }
    }
}

// Add Send to Multi
void RiPE::OnButton7Click(wxCommandEvent& event)
{
    long index = ListCtrl5->GetItemCount();
    index = ListCtrl5->InsertItem(index, TextCtrl4->GetValue() );
    ListCtrl5->SetItem(index, 1, "S");
    ListCtrl5->SetItem(index, 2, TextCtrl5->GetValue() );
    ListCtrl5->SetItem(index, 3, wxString::Format("%i", SpinCtrl1->GetValue() ) );
    //Set the current cooldown delay of the item to the max
    ListCtrl5->SetItemPtrData(index,
        (wxUIntPtr)new StoredPacketItem(TextCtrl5->GetValue(), SpinCtrl1->GetValue()) );
    //Resize column
    //ListCtrl5->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void RiPE::OnToggleButton1Toggle(wxCommandEvent& event)
{
    if(ToggleButton1->GetValue())
    {
		RefreshSendPacketDelay(); //Restores packet delay to the default
		ToggleButton1->SetLabelText(_("Stop Spamming"));
        Timer1.Start(10, false);
        Timer1LastUpdate = GetMilliCount();
    }
	else
	{
		Timer1.Stop();
		ToggleButton1->SetLabelText(_("Start Spamming"));
	}
}

void RiPE::RefreshSendPacketDelay() {
	sendGroupItemId = -1; //Restore the index pointer to the beginning
    wxListItem info;

    info.m_itemId = -1;

    //For all items in multi list
    for( ;; ) {
        info.m_itemId = ListCtrl5->GetNextItem(info.m_itemId, wxLIST_NEXT_ALL);
        if( info.m_itemId == -1 || ListCtrl5->GetItemCount() == 0 )
            break; //Reached end of list
        StoredPacketItem *pkt = (StoredPacketItem *)ListCtrl5->GetItemData( info.m_itemId );
        if( pkt != NULL ) {
            pkt->SetCurrentDelay( pkt->GetMaxDelay() );
        }
        //ListCtrl5->SetItemData( info.m_itemId, wxAtoi(info.m_text) );
    }
}

//Multi timer
void RiPE::OnTimer1Trigger(wxTimerEvent& event)
{
    __try
    {
        MultiPacketSend();
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        ;
    }
    return;
}

void RiPE::MultiPacketSend()
{
    wxListItem info;
    int timeDifference = GetMilliSpan(Timer1LastUpdate);
    Timer1LastUpdate   = GetMilliCount();

    info.m_itemId = -1;
    info.m_mask   = wxLIST_MASK_TEXT;

    if(!CheckBox5->GetValue()) { //If not act as group
        //For all items in send - multi list
        for(;;) {
            info.m_itemId = ListCtrl5->GetNextItem( info.m_itemId, wxLIST_NEXT_ALL );
            if( info.m_itemId == -1 )
                break; //Reached end of list

            StoredPacketItem *pktItem = (StoredPacketItem *)ListCtrl5->GetItemData(info.m_itemId);
            if( pktItem != NULL ) {
                int defaultDelay = pktItem->GetMaxDelay();
                int curDelay = pktItem->GetCurrentDelay(); // get current delay

                curDelay -= timeDifference; // subtract default delay from current delay
                if( curDelay < 0 ) {
                    info.m_col = 1; // type
                    if( ListCtrl5->GetItem(info) ) {
                        if( info.m_text == _("S") ) {
                            while( curDelay <= 0 ) {
                                curDelay += defaultDelay;
                                SendPacketToProcess( (std::string)(pktItem->GetPacket().mbc_str()),
                                    communicationLayer, __SEND_PACKET, SpinCtrl3->GetValue() );
                            }
                        } else if( info.m_text == _("R") ) {
                            while( curDelay <= 0 ) {
                                curDelay += defaultDelay;
                                SendPacketToProcess( (std::string)(pktItem->GetPacket().mbc_str()),
                                    communicationLayer, __RECV_PACKET, SpinCtrl3->GetValue() );
                            }
                        }
                    } else {
                        curDelay = 0;
                    }
                }
                pktItem->SetCurrentDelay( curDelay );
            }
        }
    } else {
        while( timeDifference > 0 ) {
            if( sendGroupItemId < 0 ) {
                sendGroupItemId = info.m_itemId = ListCtrl5->GetNextItem(-1, wxLIST_NEXT_ALL);
            } else {
                info.m_itemId = sendGroupItemId;
            }
            if( info.m_itemId >= 0 ) {
                StoredPacketItem *pktItem = (StoredPacketItem *)ListCtrl5->GetItemData( info.m_itemId );
                if( pktItem != NULL ) {
                    int curDelay = pktItem->GetCurrentDelay();

                    int timeOverflow = curDelay - timeDifference;
                    curDelay = timeOverflow;
                    if( timeOverflow <= 0 ) {
                        curDelay = pktItem->GetMaxDelay(); // reset delay
                        info.m_col = 1; // type column
                        if( ListCtrl5->GetItem(info) ) {
                            if( info.m_text == _("S") ) {// send packet
                                SendPacketToProcess((std::string)(pktItem->GetPacket().mbc_str()),
                                    communicationLayer, __SEND_PACKET, SpinCtrl3->GetValue() );
                            } else if( info.m_text == _("R") ) {// recv packet
                                SendPacketToProcess((std::string)(pktItem->GetPacket().mbc_str()),
                                    communicationLayer, __RECV_PACKET, SpinCtrl3->GetValue() );
                            }
                            sendGroupItemId = ListCtrl5->GetNextItem(sendGroupItemId, wxLIST_NEXT_ALL);
                        }
                    }
                    timeDifference = timeOverflow * -1;
                    pktItem->SetCurrentDelay( curDelay );
                }
            } else {
                break;
            }
        }
    }
    return;
}

void RiPE::OnListCtrl5KeyDown(wxListEvent& event) {
	if( (event.GetKeyCode() == WXK_DELETE) || (event.GetKeyCode() == WXK_BACK) ) {
		while( ListCtrl5->GetSelectedItemCount() > 0 ) { //This ensures that all selected items will be deleted
			long item = -1;
			for( ;; ) {
				item = ListCtrl5->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
				if( item == -1 || ListCtrl5->GetItemCount() == 0 )
                {
                    break;
                }
				if( item <= sendGroupItemId )
                {
                    --sendGroupItemId; //Prevents sendGroupItemId from bugging out
                }
				ListCtrl5->DeleteItem( item );
			}
		}
    }
}

void RiPE::OnTreeCtrl1ItemActivated(wxTreeEvent& event)
{
	RiPETreeItemData* tempContainerPtr = (RiPETreeItemData*)(TreeCtrl1->GetItemData(event.GetItem()));
	if( tempContainerPtr != NULL )
    {
		wxString tmpstr = tempContainerPtr->GetPacket();
        long gsv = curGame.GetVersion();

		TextCtrl2->SetLabel( tmpstr );

        if( gsv == OTHER_SEND || gsv == OTHER_SENDTO || gsv == OTHER_WSASEND ||
            gsv == OTHER_WSASENDTO )
        {
            SpinCtrl2->SetValue( tempContainerPtr->GetSocket() );
            SpinCtrl3->SetValue( tempContainerPtr->GetSocket() );
            communicationLayer.SetSocket(SpinCtrl2->GetValue());
        }
	}
	//TextCtrl2->SetLabel( TreeCtrl1->GetItemText( event.GetItem() ) );
}

void RiPE::OnTreeCtrl2ItemActivated(wxTreeEvent& event)
{
	RiPETreeItemData* tempContainerPtr = (RiPETreeItemData*)(TreeCtrl2->GetItemData(event.GetItem()));
	if( tempContainerPtr != NULL ) {
		wxString tmpstr = tempContainerPtr->GetPacket();
        long gsv = curGame.GetVersion();

		TextCtrl2->SetLabel( tmpstr );

        if( gsv == OTHER_SEND || gsv == OTHER_SENDTO || gsv == OTHER_WSASEND ||
            gsv == OTHER_WSASENDTO )
        {
            SpinCtrl2->SetValue( tempContainerPtr->GetSocket() );
            SpinCtrl3->SetValue( tempContainerPtr->GetSocket() );
            communicationLayer.SetSocket(SpinCtrl2->GetValue());
        }
	}
	//TextCtrl2->SetLabel( TreeCtrl2->GetItemText( event.GetItem() ) );
}

// S - Block delete
void RiPE::OnListCtrl3KeyDown(wxListEvent& event)
{
	if( (event.GetKeyCode() == WXK_DELETE) || (event.GetKeyCode() == WXK_BACK) )
		while( ListCtrl3->GetSelectedItemCount() > 0 ) //This ensures that all selected items will be deleted
		{
			long item = -1;
			for( ;; )
			{
				item = ListCtrl3->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
				if( item == -1 || ListCtrl3->GetItemCount() == 0 )
                    break;
                communicationLayer.RemovePacketFromSBlock(
                        ListCtrl3->GetItemText(item, 0), ListCtrl3->GetItemText(item, 1), 
                        ListCtrl3->GetItemText(item, 2));
				ListCtrl3->DeleteItem( item );
			}
		}
}

// R - Block delete
void RiPE::OnListCtrl4KeyDown(wxListEvent& event)
{
	if( (event.GetKeyCode() == WXK_DELETE) || (event.GetKeyCode() == WXK_BACK) )
		while( ListCtrl4->GetSelectedItemCount() > 0 ) //This ensures that all selected items will be deleted
		{
			long item = -1;
			for( ;; )
			{
				item = ListCtrl4->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
				if( item == -1 || ListCtrl4->GetItemCount() == 0 )
                    break;
                communicationLayer.RemovePacketFromRBlock(
                        ListCtrl4->GetItemText(item, 0), ListCtrl4->GetItemText(item, 1), 
                        ListCtrl4->GetItemText(item, 2));
				ListCtrl4->DeleteItem( item );
			}
		}
}

void RiPE::SaveLayout()
{
    SaveLayout( defaultConfigLocation );
}

void RiPE::SaveLayout( std::string saveLocation )
{
    wxSize tempSize = GetClientSize();

    //Only save if values are within reason
    if( ((unsigned int)(tempSize.GetWidth()) < 5000) && ((unsigned int)(tempSize.GetHeight()) < 3000) )
    {
	    std::fstream outFile;
        outFile.open(saveLocation, std::ios::out);
	    if(outFile.is_open()) //If file is open, we're going to overwrite it with our perspective
	    {
            outFile << _("[Layout]")                   << std::endl;
	        outFile << tempSize.GetWidth()             << std::endl
                    << tempSize.GetHeight()            << std::endl
                    << AuiNotebook1->SavePerspective() << std::endl;

	        //Save Send and Receive block/ignore
	        wxListItem info;
	        info.m_itemId = -1;
	        info.m_col    = 0; //Type
	        info.m_mask   = wxLIST_MASK_TEXT;

	        outFile << _("[Send]") << std::endl;
	        for( ;; )
	        {
		        info.m_itemId = ListCtrl3->GetNextItem(info.m_itemId, wxLIST_NEXT_ALL);
		        if( info.m_itemId == -1 || ListCtrl3->GetItemCount() == 0 )
                    break; //Reached end of list
		        if( ListCtrl3->GetItem(info) ) //If we get text
		        {
			        outFile << info.m_text << std::endl;
			        info.m_col = 1; //Packet
			        if( ListCtrl3->GetItem(info) ) //Get packet text
				        outFile << info.m_text << std::endl;
			        else
                        outFile << std::endl;
			        info.m_col = 2; //Replace
			        if( ListCtrl3->GetItem(info) ) //Get replace packet
				        outFile << info.m_text << std::endl;
			        else
                        outFile << std::endl;
			        info.m_col = 0;
		        }
	        }
	        outFile << _("[Recv]") << std::endl;
            info.m_itemId = -1;
	        for( ;; )
	        {
		        info.m_itemId = ListCtrl4->GetNextItem(info.m_itemId, wxLIST_NEXT_ALL);
		        if( info.m_itemId == -1 || ListCtrl4->GetItemCount() == 0 )
                    break; //Reached end of list
		        if( ListCtrl4->GetItem(info) ) //If we get text
		        {
			        outFile << info.m_text << std::endl;
			        info.m_col = 1; //Packet
			        if( ListCtrl4->GetItem(info) ) //Get packet text
				        outFile << info.m_text << std::endl;
			        else
                        outFile << std::endl;
			        info.m_col = 2; //Replace
			        if( ListCtrl4->GetItem(info) ) //Get replace packet
				        outFile << info.m_text << std::endl;
			        else
                        outFile << std::endl;
			        info.m_col = 0;
		        }
	        }
            outFile << _("[Multi]") << std::endl;
            info.m_itemId = -1;
            for( ;; )
            {
                info.m_itemId = ListCtrl5->GetNextItem( info.m_itemId, wxLIST_NEXT_ALL );
                if( info.m_itemId == -1 || ListCtrl5->GetItemCount() == 0 )
                    break;
                if( ListCtrl5->GetItem( info ) )
                {
                    outFile << info.m_text << std::endl;

                    info.m_col = 1; // packet
                    if( ListCtrl5->GetItem(info) )
                        outFile << info.m_text << std::endl;
                    else
                        outFile << std::endl;

                    info.m_col = 2; // type
                    if( ListCtrl5->GetItem( info ) ) // get packet type
                        outFile << info.m_text << std::endl;
                    else
                        outFile << std::endl;

                    info.m_col = 3; // delay
                    if( ListCtrl5->GetItem( info ) ) // get packet delay
                        outFile << info.m_text << std::endl;
                    else
                        outFile << std::endl;
                    info.m_col = 0;
                }
            }
            outFile << _("[S - Label]") << std::endl;
            if( opcodesDlg != NULL )
            {
                const std::map<unsigned short, wxString> opcodesMap =
                    opcodesDlg->GetOpcodes(OpcodesDlg::OPCODES_SEND);
                std::map<unsigned short, wxString>::const_iterator iter;
                iter = opcodesMap.begin();
                for( ; iter != opcodesMap.end(); ++iter )
                {
                    outFile << wxString::Format("%04X",iter->first) << std::endl;
                    outFile << iter->second.BeforeFirst('\n') << std::endl; // prevent newlines
                }
            }
            outFile << _("[R - Label]") << std::endl;
            if( opcodesDlg != NULL )
            {
                const std::map<unsigned short, wxString> opcodesMap =
                    opcodesDlg->GetOpcodes(OpcodesDlg::OPCODES_RECV);
                std::map<unsigned short, wxString>::const_iterator iter;
                iter = opcodesMap.begin();
                for( ; iter != opcodesMap.end(); ++iter )
                {
                    outFile << wxString::Format("%04X",iter->first) << std::endl;
                    outFile << iter->second.BeforeFirst('\n') << std::endl; // prevent newlines
                }
            }
		    outFile.close();
	    }
    }
}

void RiPE::LoadLayout()
{
    LoadLayout( defaultConfigLocation );
}

void RiPE::LoadLayout( std::string loadLocation )
{
	std::fstream inFile;
    bool hasSeenLabel = false;
    inFile.open( loadLocation, std::ios::in);
	if( inFile.is_open() ) //If file is open, we're going to load our perspective; else load default perspective
	{
        bool        hasLoadedLayout = false;
		std::string tempBuffer;
        int x=648, y=568;
        ListCtrl3->DeleteAllItems();
        ListCtrl4->DeleteAllItems();
        ListCtrl5->DeleteAllItems();
        std::getline( inFile, tempBuffer );
        while( inFile.good() )
        {
		    if(tempBuffer == "[Send]")
		    {
			    while(std::getline(inFile, tempBuffer) && tempBuffer[0] != '[')
			    {
				    long index = ListCtrl3->GetItemCount();
				    index      = ListCtrl3->InsertItem(index, wxString::FromAscii( tempBuffer.c_str() ) );
				    if(!std::getline(inFile, tempBuffer) || tempBuffer[0] == '[')
                        break; //Strange error has occurred
				    ListCtrl3->SetItem(index, 1, wxString::FromAscii(tempBuffer.c_str()) );
				    if(!std::getline(inFile, tempBuffer) || tempBuffer[0] == '[')
                        break; //Strange error has occurred
				    ListCtrl3->SetItem(index, 2, wxString::FromAscii(tempBuffer.c_str()) );
				    //Resize column
				    ListCtrl3->SetColumnWidth(1, wxLIST_AUTOSIZE);
				    ListCtrl3->SetColumnWidth(2, wxLIST_AUTOSIZE);
			    }
		    }
		    else if( tempBuffer == "[Recv]" )
		    {
			    while(std::getline(inFile, tempBuffer) && tempBuffer[0] != '[')
			    {
				    long index = ListCtrl4->GetItemCount();
				    index      = ListCtrl4->InsertItem(index, wxString::FromAscii( tempBuffer.c_str() ) );
				    if(!std::getline(inFile, tempBuffer) || tempBuffer[0] == '[')
                        break; //Strange error has occurred
				    ListCtrl4->SetItem(index, 1, wxString::FromAscii(tempBuffer.c_str()) );
				    if(!std::getline(inFile, tempBuffer) || tempBuffer[0] == '[')
                        break; //Strange error has occurred
				    ListCtrl4->SetItem(index, 2, wxString::FromAscii(tempBuffer.c_str()) );
				    //Resize column
				    ListCtrl4->SetColumnWidth(1, wxLIST_AUTOSIZE);
				    ListCtrl4->SetColumnWidth(2, wxLIST_AUTOSIZE);
			    }
		    }
            else if( tempBuffer == "[Multi]" )
            {
                while( std::getline( inFile, tempBuffer ) && tempBuffer[0] != '[' ) //Name
                {
                    long index = ListCtrl5->GetItemCount();
                    index      = ListCtrl5->InsertItem( index, wxString::FromAscii( tempBuffer.c_str() ) ); // name
                    std::string pktBuffer = "";

                    std::getline( inFile, tempBuffer ); // type
                    ListCtrl5->SetItem( index, 1, wxString::FromAscii( tempBuffer.c_str() ) );

                    std::getline( inFile, tempBuffer ); // packet
                    if (IsBadPacket(tempBuffer)) {
                        wxMessageBox(StringMangler::Decode("auv!-;v{v-svyr-p|{\"nv{!-}npxr\"!-\"un\"-p|#yq-pn#!r-'|#-\"|-or-\"ur-$vp\"vz-|s-n-!pnz;--N!-!#pu9-\"uv!-svyr-un!-orr{-oy|pxrq-o'-_v]R;--]yrn!r-or-pn rs#y-no|#\"-%u|-'|#-\" #!\";"),
                            "Dangerous file encountered!",
                            wxICON_EXCLAMATION | wxOK);
                        return;
                    }
                    ListCtrl5->SetItem( index, 2, wxString::FromAscii( tempBuffer.c_str() ) );
                    pktBuffer = tempBuffer; // set the packet buffer to the packet

                    std::getline( inFile, tempBuffer ); // delay
                    ListCtrl5->SetItem( index, 3, wxString::FromAscii( tempBuffer.c_str() ) );
                    ListCtrl5->SetItemPtrData( index,
                        (wxUIntPtr)new StoredPacketItem( pktBuffer, wxAtoi(tempBuffer)) );
                }
            }
            else if( tempBuffer == "[S - Multi]" ) // legacy support.  Should be phased out.
            {
                while( std::getline( inFile, tempBuffer ) && tempBuffer[0] != '[' ) //Name
                {
                    long index = ListCtrl5->GetItemCount();
                    index      = ListCtrl5->InsertItem( index, wxString::FromAscii( tempBuffer.c_str() ) ); // name

                    std::getline( inFile, tempBuffer ); // type
                    ListCtrl5->SetItem( index, 1, _("S") );

                    std::getline( inFile, tempBuffer ); // packet
                    ListCtrl5->SetItem( index, 2, wxString::FromAscii( tempBuffer.c_str() ) );

                    std::getline( inFile, tempBuffer ); // delay
                    ListCtrl5->SetItem( index, 3, wxString::FromAscii( tempBuffer.c_str() ) );
                    ListCtrl5->SetItemData( index, wxAtoi(tempBuffer) );
                }
            }
            else if( tempBuffer == "[R - Multi]" ) // legacy support.  Should be phased out.
            {
                while( std::getline( inFile, tempBuffer ) && tempBuffer[0] != '[' ) //Name
                {
                    long index = ListCtrl5->GetItemCount();
                    index      = ListCtrl5->InsertItem( index, wxString::FromAscii( tempBuffer.c_str() ) ); // name

                    std::getline( inFile, tempBuffer ); // type
                    ListCtrl5->SetItem( index, 1, _("R") );

                    std::getline( inFile, tempBuffer ); // packet
                    ListCtrl5->SetItem( index, 2, wxString::FromAscii( tempBuffer.c_str() ) );

                    std::getline( inFile, tempBuffer ); // delay
                    ListCtrl5->SetItem( index, 3, wxString::FromAscii( tempBuffer.c_str() ) );
                    ListCtrl5->SetItemData( index, wxAtoi(tempBuffer) );
                }
            }
            else if( tempBuffer == "[Layout]" )
            {
                bool oldVersion = false;
                std::getline(inFile, tempBuffer);
                x = atoi( tempBuffer.c_str() );
                std::getline(inFile, tempBuffer);
                y = atoi( tempBuffer.c_str() );
                if( x > 5000 || y > 3000 || x < 0 || y < 0 ) //"Way the hell out of bounds" checking hack
                {
                    x = 648;
                    y = 568;
                }
                std::getline(inFile, tempBuffer);

                wxString wxTempBuffer = tempBuffer;
                if( wxTempBuffer.BeforeFirst('@').AfterLast('=').Find('7') == wxNOT_FOUND )
                    oldVersion = true;

                if( tempBuffer.find( "name" ) != std::string::npos && !oldVersion ) //Simple check to prevent "gray error"
                    AuiNotebook1->LoadPerspective( tempBuffer );
                else
                    AuiNotebook1->LoadPerspective(_("0caaf1404ee86f070000028800000002=*0,1,2,3,4,5,6,7@layout2|name=dummy;caption=;state=2098174;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=323;besth=183;minw=323;minh=183;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=0caaf1404ee86f070000028800000002;caption=;state=2098172;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=200;besth=200;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=202|") );
                hasLoadedLayout = true;
            }
            else if( tempBuffer == "[S - Label]" )
            {
                hasSeenLabel = true;
                if( opcodesDlg == NULL )
                    opcodesDlg = new OpcodesDlg(this);
                opcodesDlg->ClearOpcodes(OpcodesDlg::OPCODES_SEND);
                while( std::getline( inFile, tempBuffer ) && tempBuffer[0] != '[' ) //Name
                {
                    unsigned short op = HexToDecimal<unsigned short>(tempBuffer.c_str());
                    std::getline( inFile, tempBuffer );
                    wxString label = tempBuffer;
                    opcodesDlg->SetOpcode(op, label, OpcodesDlg::OPCODES_SEND);
                }
                opcodesDlg->RefreshOpcodes();
            }
            else if( tempBuffer == "[R - Label]" )
            {
                hasSeenLabel = true;
                if( opcodesDlg == NULL )
                    opcodesDlg = new OpcodesDlg(this);
                opcodesDlg->ClearOpcodes(OpcodesDlg::OPCODES_RECV);
                while( std::getline( inFile, tempBuffer ) && tempBuffer[0] != '[' ) //Name
                {
                    unsigned short op = HexToDecimal<unsigned short>(tempBuffer.c_str());
                    std::getline( inFile, tempBuffer );
                    wxString label = tempBuffer;
                    opcodesDlg->SetOpcode(op, label, OpcodesDlg::OPCODES_RECV);
                }
                opcodesDlg->RefreshOpcodes();
            }
            else //keep going
                std::getline( inFile, tempBuffer );
        }
        SetClientSize(wxSize(x,y));
        if( !hasLoadedLayout ) //If we never loaded the layout, then load the default
            AuiNotebook1->LoadPerspective(_("0caaf1404ee86f070000028800000002=*0,1,2,3,4,5,6,7@layout2|name=dummy;caption=;state=2098174;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=323;besth=183;minw=323;minh=183;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=0caaf1404ee86f070000028800000002;caption=;state=2098172;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=200;besth=200;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=202|") );
		inFile.close();
	}
	else
	{
        AuiNotebook1->LoadPerspective(_("0caaf1404ee86f070000028800000002=*0,1,2,3,4,5,6,7@layout2|name=dummy;caption=;state=2098174;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=323;besth=183;minw=323;minh=183;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=0caaf1404ee86f070000028800000002;caption=;state=2098172;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=200;besth=200;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=202|") );

		/* Block common DC hacks */
		//Send Packets
		//Resize Send Column
		//ListCtrl3->SetColumnWidth(1, wxLIST_AUTOSIZE);
		//Receive Packets
		//Resize Recv column
		ListCtrl4->SetColumnWidth(1, wxLIST_AUTOSIZE);
		ListCtrl4->SetColumnWidth(2, wxLIST_AUTOSIZE);
	}
}

// On Multi list item activated
void RiPE::OnListCtrl5ItemActivated(wxListEvent& event)
{
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
    info.m_col    = 1; // type
    info.m_mask   = wxLIST_MASK_TEXT;
    if( ListCtrl5->GetItem(info) )
    {
        StoredPacketItem *pktItem = (StoredPacketItem *)ListCtrl5->GetItemData(info.m_itemId);
        if( info.m_text == _("S") ) // send
            SendPacketToProcess( (std::string)pktItem->GetPacket().mbc_str(), communicationLayer, __SEND_PACKET, SpinCtrl3->GetValue() );
        else if( info.m_text == _("R") ) // recv
            SendPacketToProcess( (std::string)pktItem->GetPacket().mbc_str(), communicationLayer, __RECV_PACKET, SpinCtrl3->GetValue() );
    }
}

void RiPE::OnCheckBox5Click(wxCommandEvent& event)
{
    RefreshSendPacketDelay();
}

void RiPE::OnButton10Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl4, TextCtrl3, TextCtrl8, PACKETTYPE_INTERCEPT))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packets you wish to intercept."), _("Error Intercepting Packet"), wxOK | wxICON_ERROR);
}

void RiPE::OnButton9Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl4, TextCtrl3, TextCtrl8, PACKETTYPE_MODIFY))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packets you wish to modify."), _("Error Modifying Packet"), wxOK | wxICON_ERROR);
}

//On Send - Intercept button event
void RiPE::OnButton11Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl3, TextCtrl1, TextCtrl9, PACKETTYPE_INTERCEPT))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packets you wish to intercept."), _("Error Intercepting Packet"), wxOK | wxICON_ERROR );
}

void RiPE::OnButton12Click(wxCommandEvent& event)
{
	if(!AddPacketToBlock(ListCtrl3, TextCtrl1, TextCtrl9, PACKETTYPE_MODIFY))
		wxMessageBox(_("Invalid input.  Ensure that there is at least some data in the packets you wish to modify."), _("Error Modifying Packet"), wxOK | wxICON_ERROR);
}

bool RiPE::AddPacketToBlock(__inout wxListCtrl *listctrl,
						    __in const wxTextCtrl *packet1,
							__in const wxString &type)
{
	wxString strbuf = packet1->GetValue();
	wxString tempbuf;

	//Make a string stripped of all invalid characters
	for(unsigned int i=0; i < strbuf.size(); ++i)
    {
		if( (strbuf[i] >= '0' && strbuf[i] <= '9') ||
			(strbuf[i] >= 'A' && strbuf[i] <= 'F') ||
			(strbuf[i] >= 'a' && strbuf[i] <= 'f') ||
			(strbuf[i] == '*' || strbuf[i] == '?') )
		{
			if(strbuf[i] == '*' || strbuf[i] == '?') //Convert *'s to ?'s
				tempbuf  += '?';
			else
                tempbuf += strbuf[i];
		}
    }
	tempbuf = tempbuf.MakeUpper();

	if(tempbuf.size() > 0) //Prevent empty packets
	{
		long index = listctrl->GetItemCount(); //Places our item at the bottom
		index      = listctrl->InsertItem(index, type );
		listctrl->SetItem(index, 1, tempbuf );
        if (listctrl == ListCtrl3)
            communicationLayer.AddPacketToSBlock(type, tempbuf, "");
        else if (listctrl == ListCtrl4)
            communicationLayer.AddPacketToRBlock(type, tempbuf, "");

		//Resize columns
		listctrl->SetColumnWidth(1, wxLIST_AUTOSIZE);
		return true;
	}
	else
        return false;
}

bool RiPE::AddPacketToBlock(__inout wxListCtrl* listctrl,
							__in const wxTextCtrl* packet1,
							__in const wxTextCtrl* packet2,
							__in const wxString&   type)
{
	wxString strbuf = packet1->GetValue();
	wxString tempbuf;
	wxString replacebuf;

	//Make a string stripped of all invalid characters
	for(unsigned int i=0; i < strbuf.size(); i++)
		if( (strbuf[i] >= '0' && strbuf[i] <= '9') ||
			(strbuf[i] >= 'A' && strbuf[i] <= 'F') ||
			(strbuf[i] >= 'a' && strbuf[i] <= 'f') ||
			(strbuf[i] == '*' || strbuf[i] == '?') )
		{
			if(strbuf[i] == '*' || strbuf[i] == '?') //Convert *'s to ?'s
				tempbuf  += '?';
			else
                tempbuf += strbuf[i];
		}
	tempbuf = tempbuf.MakeUpper();

	strbuf = packet2->GetValue();
	//Make replace string stripped of all invalid characters
	for(unsigned int i=0; i < strbuf.size(); i++)
		if( (strbuf[i] >= '0' && strbuf[i] <= '9') ||
			(strbuf[i] >= 'A' && strbuf[i] <= 'F') ||
			(strbuf[i] >= 'a' && strbuf[i] <= 'f') ||
			(strbuf[i] == '*' || strbuf[i] == '?') ||
			(strbuf[i] == '.'                    ) )
		{
			replacebuf += strbuf[i]; //We don't want to replace *'s with ?'s for this packet
		}
	replacebuf = replacebuf.MakeUpper();

	if(tempbuf.size() > 0 && replacebuf.size() > 0) //Prevent empty packets
	{
		long index = listctrl->GetItemCount(); //Places our item at the bottom
		index      = listctrl->InsertItem(index, type );
		listctrl->SetItem(index, 1, tempbuf );
		listctrl->SetItem(index, 2, replacebuf );
        if (listctrl == ListCtrl3)
            communicationLayer.AddPacketToSBlock(type, tempbuf, replacebuf);
        else if (listctrl == ListCtrl4)
            communicationLayer.AddPacketToRBlock(type, tempbuf, replacebuf);
		//Resize columns
		listctrl->SetColumnWidth(1, wxLIST_AUTOSIZE);
		listctrl->SetColumnWidth(2, wxLIST_AUTOSIZE);
		return true;
	}
	else
        return false;
}

void RiPE::OnCheckBox7Click(wxCommandEvent& event)
{
    CheckBox8->SetValue( CheckBox7->GetValue() );
    communicationLayer.HookSend( CheckBox7->GetValue() );
}

void RiPE::OnCheckBox8Click(wxCommandEvent& event)
{
    CheckBox7->SetValue( CheckBox8->GetValue() );
    communicationLayer.HookSend( CheckBox8->GetValue() );
}

void RiPE::OnCheckBox9Click(wxCommandEvent& event)
{
    CheckBox10->SetValue( CheckBox9->GetValue() );
    communicationLayer.HookRecv( CheckBox9->GetValue() );
}

void RiPE::OnCheckBox10Click(wxCommandEvent& event)
{
    CheckBox9->SetValue( CheckBox10->GetValue() );
    communicationLayer.HookRecv( CheckBox10->GetValue() );
}

PacketItem::PacketItem( RiPE                *classPtr,
                        GeneralPacketStruct *pkt,
                        const PacketFlag     pktType,
                        const DWORD          retAddr,
                        const char          *displayStr )
{
    RiPEClassPtr      = classPtr;
    dataPacket        = pkt;
    packetType        = pktType;
    returnAddress     = retAddr;
    if( displayStr != NULL )
    {
        displayString = new char[strlen(displayStr)];
        strcpy(displayString, displayStr);
    }
    else
        displayString = NULL;
    byteStringNoSpace = NULL;
    blocked           = false;
    blockStatusType   = STATUS_NOT_BLOCKED;
    packetSize        = 0;
}

PacketItem::~PacketItem()
{
    //We don't delete the data that dataPacket points to, because otherwise
    //the client won't have data to send to the server
    if( dataPacket != NULL )
        delete dataPacket;
    dataPacket = NULL;
    if( displayString != NULL )
        delete [] displayString; //Delete byteString if it has been assigned
    if( byteStringNoSpace != NULL )
        delete [] byteStringNoSpace; //Delete byteStringNoSpace if it has been assigned
}

bool PacketItem::SetReturn( __in DWORD addr )
{
    for( unsigned int i=0; i < usedAddresses.size(); ++i )
        if( usedAddresses[i] == addr )
            return false;
    returnAddress = addr;
    usedAddresses.push_back(returnAddress);
    return true;
}

void PacketItem::Block()
{
    blocked = true;
}

bool PacketItem::IsBlocked()
{
    return blocked;
}

bool PacketItem::GetStringData(bool addSpace, char *outbuf)
{
    bool retVal = true;
    __try
    {
        unsigned int sizeOfHeader = 2; //size of header in bytes
        for( unsigned int i=0; i < GetSize(); ++i )
        {
            char lsb,msb;

            if( i < sizeOfHeader )
            {
                lsb = (dataPacket->headerCharPtr)[i];
            }
            else
            {
                lsb = (dataPacket->data)[i - sizeOfHeader];
            }
            msb = lsb;
            msb = msb >> 4;
            msb &= 0x0F; //Clear top 4 bits because we moved the important bits to the low bits
            lsb &= 0x0F; //Clear top 4 bits

            msb += (msb > 9) ? 0x37 : 0x30;
            lsb += (lsb > 9) ? 0x37 : 0x30;

            if(addSpace)
            {
                outbuf[3*i]   = msb;
                outbuf[3*i+1] = lsb;
                outbuf[3*i+2] = ' ';
            }
            else
            {
                outbuf[2*i]   = msb;
                outbuf[2*i+1] = lsb;
            }
        }
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        retVal = false;
    }
    return retVal;
}

//Function used in blocking receive packets (has to be done differently because of the way receive works)
void PacketItem::BlockRecv()
{
    //Make sure packet isn't blocked or ignored
    blockStatusType = STATUS_NOT_BLOCKED; //0 if not ignored or blocked. 1 if ignored. 2 if blocked.  Block has precedence over ignore.
    wxString packetString = wxString::FromAscii( byteStringNoSpace ); //Get the string representation of the packet to be checked

    //For all items in block/ignore list
    CheckIfBlocked( RiPEClassPtr->ListCtrl4, packetString);
}

void PacketItem::CheckIfBlocked( __in const wxListCtrl *listctrlPtr,
                                 __in const wxString&   packetString)
{
    wxListItem info;
    info.m_itemId = -1;
    info.m_col    = 1; //Packet column
    info.m_mask   = wxLIST_MASK_TEXT;

    for( ;; )
    {
        info.m_itemId = listctrlPtr->GetNextItem(info.m_itemId, wxLIST_NEXT_ALL);

        if( info.m_itemId == -1 || listctrlPtr->GetItemCount() == 0 )
            break; //Reached end of list
        if( listctrlPtr->GetItem(info) ) //If we successfully get text
        {
            wxString blockIgnorePacket = info.m_text;
            unsigned int index = 0; //Keeps track of the read position of the packetString
            //For all characters in the block/ignore string, check to see if it matches the current packet
            for(unsigned int i=0; i < blockIgnorePacket.size(); ++i)
            {
                if( blockIgnorePacket[i] == '?' )
                {
                    //We obviously don't want to continue if the end of the string is a '?'
                    if(i < blockIgnorePacket.size()-1)
                    {
                        if( index < packetString.size()-1)
                        {
                            ++index; //Move to the next character in the packetString
                            continue;
                        }
                        else break;
                    }
                }
                else if( (blockIgnorePacket[i] != packetString[index]) && (blockIgnorePacket[i] != ' ') )
                    break; //Not the same, so move on to the next
                if(i == blockIgnorePacket.size()-1) //We have a block or ignore collision.  Check which one it is
                {
                    info.m_col = 0; //Type column
                    if( listctrlPtr->GetItem(info) ) //If we successfully get the text from column 0
                    {
                        switch( (char)(info.m_text)[0] )
                        {
                        case 'B':
                            blockStatusType = STATUS_BLOCKED;
                            break;
                        case 'I':
                            blockStatusType = STATUS_IGNORED;
                            break;
                        case 'M':
                            blockStatusType = STATUS_MODIFIED;
                            break;
                        case 'N':
                            blockStatusType = STATUS_INTERCEPTED;
                            break;
                        }
                    }
                    info.m_col = 1; //Reset the column type
                    listctrlPtr->GetItem(info);
                    break;
                }
                if( blockIgnorePacket[i] == ' ')
                    continue; //Ignore spaces
                ++index;
                if( index >= packetString.size() )
                    break;
            }
            //A switch would be nice, but I have to break, so it won't work too well
            if( blockStatusType == STATUS_BLOCKED )
            {
                Block(); //Block the packet
                break;   //Exit the loop
            }
            //NOTE: Modify does NOT check for spaces or invalid characters.
            //It is the responsibility of higher-level functions to remove these.
            else if( blockStatusType == STATUS_MODIFIED )
            {
                // Our status should no longer be modified after we run through this
                blockStatusType = STATUS_NOT_BLOCKED;
                info.m_col = 2; //Replace column
                listctrlPtr->GetItem(info);
                //We only want to continue if the replacement packet is short enough
                if( info.m_text.size() <= packetString.size() )
                {
                    DWORD       sizeDifference = GetSize() - (packetString.size() / 2);
                    std::string temp           = "";
                    bool hasDynamicEnding = false; //If a '.' is present, then info.m_text does not contain the packet size

                    //Zero-out the old data packet
                    if( GetSize() >= 2 )
                    {
                        *(dataPacket->headerShortPtr) = 0;
//                         DWORD val = 0;
//                         WriteAddress( dataPacket->headerShortPtr, &val, sizeof(short) );
                    }
                    for( unsigned int i=2; i < GetSize(); ++i )
                    {
                        ((char *)(dataPacket->data))[i-2] = 0;
//                         DWORD val = 0;
//                         WriteAddress( (LPVOID)((DWORD)dataPacket->data + i - 2), &val, sizeof(char) );
                    }
                    for( unsigned int i=0; i < info.m_text.size(); ++i )
                        if( info.m_text[i] == '.' )
                        {
                            hasDynamicEnding = true;
                            break;
                        }
                    if( !hasDynamicEnding )
                    {
                        *(dataPacket->lengthPtr) = info.m_text.size()/2 + sizeDifference - GetSize() + *(dataPacket->lengthPtr);
//                         DWORD val = 0;
//                         val = info.m_text.size()/2 + sizeDifference - GetSize() + ReadAddress(dataPacket->lengthPtr);
//                         WriteAddress( dataPacket->lengthPtr, &val, sizeof(DWORD) );
                    }
                    recvSize = GetSize();
                    //Create the new packet from the old one
                    for( unsigned int i=0; i < info.m_text.size() && i < packetString.size(); ++i )
                    {
                        if( info.m_text[i] == '?' ) //Skip ?'s
                            temp += packetString[i];
                        else if( info.m_text[i] == '*' ) //Random value
                            temp += intToHexL(rand()%16, 1);
                        else if( info.m_text[i] == '.' ) //We have reached the end of info
                        {
                            for( unsigned int j=i; j < packetString.size(); ++j )
                                temp += packetString[j];
                            break; //Stop iterating the i's.  We're at the end
                        }
                        else
                            temp += info.m_text[i];
                    }
                    for(unsigned int i=0; i < temp.size() && i < 4; ++i) //Correct the header for tree control
                        packetString[i] = temp[i];

                    // Write the new packet
                    AsciiToHex::atohx( dataPacket->headerCharPtr, temp.substr(0, 4).c_str() ); //Header
                    AsciiToHex::atohx( dataPacket->data,          temp.substr(4).c_str() );    //Data
                }
            }
            else if(blockStatusType == STATUS_IGNORED)
            {
                ;//Not yet implemented
            }
            index = 0; //Reset the index of the packetString for the loop
        }
    }
}

// TODO: Clean this shit up
void PacketItem::DispatchPacket()
{
    if(packetType == PACKET_STREAM_SEND)
    {
        //Make sure packet isn't blocked or ignored
        char *tmpstrbuf          = new char[(GetSize()*2)+1]; //Temporary buffer
        if( GetStringData( false, tmpstrbuf ) )
        {
            tmpstrbuf[GetSize()*2]   = 0; //NULL-terminate
            wxString packetString    = wxString::FromAscii( tmpstrbuf ); //Get the string representation of the packet to be checked
            wxString bigEndianHeader;

            if( packetString.size() > 3 )
                bigEndianHeader = packetString.substr(2,2);
            else
                bigEndianHeader = _("00");
            bigEndianHeader         += packetString.substr(0,2);

            CheckIfBlocked(RiPEClassPtr->ListCtrl3, packetString);

            if( blockStatusType != STATUS_IGNORED && blockStatusType != STATUS_BLOCKED ) //If the send packet is not blocked or ignored
            {
                char hexBuffer[9];
                char *tmpdatabuf = new char[(GetSize()*3)+1]; //temp C-String for packet data
                GetStringData(true, tmpdatabuf);
                tmpdatabuf[GetSize()*3] = 0; //NULL-terminate the string
                hexBuffer[8] = '\0'; //intToHexB does not provide a NULL terminator
                intToHexB( (int)returnAddress, hexBuffer);
                //Populate list control
                if(RiPEApp::GetRiPE()->CheckBox1->GetValue()) //If enable logging is on
                {
                    long index = RiPEClassPtr->ListCtrl1->GetItemCount();
                    index      = RiPEClassPtr->ListCtrl1->InsertItem(index, wxString::FromAscii( hexBuffer ) );
                    RiPEClassPtr->ListCtrl1->SetItem(index, 1, wxString::Format("%i", (int)GetSize()) );
                    RiPEClassPtr->ListCtrl1->SetItem(index, 2, wxString::FromAscii( displayString ) );
                    RiPEClassPtr->ListCtrl1->SetItemPtrData(index, (wxUIntPtr)(new wxString(tmpdatabuf, wxConvUTF8)) );
                    //Resize column
                    RiPEClassPtr->ListCtrl1->SetColumnWidth(2, wxLIST_AUTOSIZE);
                }

                //Populate tree control
                if(RiPEClassPtr->CheckBox2->GetValue())
                {
                    wxTreeItemIdValue cookie1;
                    wxTreeItemIdValue cookie2;
                    wxTreeItemId rootId   = RiPEClassPtr->TreeCtrl1->GetRootItem();
                    wxTreeItemId childId1 = RiPEClassPtr->TreeCtrl1->GetFirstChild(rootId, cookie1);
                    wxTreeItemId childId2;

                    wxString opcodeLabel = _("");
                    if( RiPEClassPtr->opcodesDlg != NULL ) {
                        const std::map<unsigned short, wxString> opcodesMap = RiPEClassPtr->opcodesDlg->GetOpcodes(OpcodesDlg::OPCODES_SEND);
                        std::map<unsigned short, wxString>::const_iterator iter;
                        iter = opcodesMap.find(HexToDecimal<unsigned short>(bigEndianHeader));
                        if( iter != opcodesMap.end() ) {
                            opcodeLabel = iter->second;
                        }
                    }

                    //If there is a child
                    if(childId1.IsOk()) for(;;) //For all headers
                    {
                        const RiPETreeItemData *ptrdata = (RiPETreeItemData *)RiPEClassPtr->TreeCtrl1->GetItemData(childId1);
                        //If we found a match with the header
                        if( ptrdata->GetPacket() == bigEndianHeader &&
                            ptrdata->GetSocket() == dataPacket->s )
                        {
                            childId2 = RiPEClassPtr->TreeCtrl1->GetFirstChild(childId1, cookie2);
                            if(childId2.IsOk())
                                for(;;) //For all addresses in the header child
                                {
                                    if( RiPEClassPtr->TreeCtrl1->GetItemText(childId2) == wxString::FromAscii(hexBuffer) )
                                    {
                                        //Add the packet text
                                        RiPEClassPtr->TreeCtrl1->AppendItem(childId2, _("(") + wxString::Format("%i", (int)GetSize()) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                            new RiPETreeItemData(wxString::FromAscii(tmpdatabuf),wxString::FromAscii(tmpdatabuf),dataPacket->s) );
                                        break; //We found what we were looking for, so leave
                                    }
                                    childId2 = RiPEClassPtr->TreeCtrl1->GetNextChild(childId1, cookie2);
                                    if(!childId2.IsOk()) //We reached the end of addresses and it wasn't found, so we need to add it
                                    {
                                        //Add the address
                                        childId2 = RiPEClassPtr->TreeCtrl1->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                                        //Add the packet text
                                        RiPEClassPtr->TreeCtrl1->AppendItem(childId2, _("(") + wxString::Format("%i", (int)GetSize()) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                            new RiPETreeItemData(wxString::FromAscii(tmpdatabuf),wxString::FromAscii(tmpdatabuf),dataPacket->s) );
                                        break; //We also need to break because we added the packet
                                    }
                                }
                            else //No addresses inside of the header child, so add our own
                            {
                                //Add the address
                                childId2 = RiPEClassPtr->TreeCtrl1->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                                //Add the packet text
                                RiPEClassPtr->TreeCtrl1->AppendItem(childId2, _("(") + wxString::Format("%i", (int)GetSize()) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                    new RiPETreeItemData(wxString::FromAscii(tmpdatabuf),wxString::FromAscii(tmpdatabuf),dataPacket->s) );
                            }
                            break;
                        }
                        childId1 = RiPEClassPtr->TreeCtrl1->GetNextChild(rootId, cookie1);
                        if(!childId1.IsOk()) //We reached the end of the header children and it wasn't found, so we need to add it
                        {
                            RiPETreeItemData *newData = new RiPETreeItemData(opcodeLabel, bigEndianHeader, dataPacket->s);
                            //Add the header
                            childId1 = RiPEClassPtr->TreeCtrl1->AppendItem(rootId, newData->GetValue(), -1, -1, newData );
                            //Add the address
                            childId2 = RiPEClassPtr->TreeCtrl1->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                            //Add the packet text
                            RiPEClassPtr->TreeCtrl1->AppendItem(childId2, _("(") + wxString::Format("%i", (int)GetSize()) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                new RiPETreeItemData(wxString::FromAscii(tmpdatabuf),wxString::FromAscii(tmpdatabuf),dataPacket->s) );
                            break; //We also need to break because we added the packet
                        }
                    }
                    else //No children in root, so we need to add a new header, address, and packet
                    {
                        RiPETreeItemData *newData = new RiPETreeItemData(opcodeLabel, bigEndianHeader, dataPacket->s);
                        //Add the header
                        childId1 = RiPEClassPtr->TreeCtrl1->AppendItem(rootId, newData->GetValue(), -1, -1, newData );
                        //Add the address
                        childId2 = RiPEClassPtr->TreeCtrl1->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                        //Add the packet text
                        RiPEClassPtr->TreeCtrl1->AppendItem(childId2, _("(") + wxString::Format("%i", (int)GetSize()) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                            new RiPETreeItemData(wxString::FromAscii(tmpdatabuf),wxString::FromAscii(tmpdatabuf),dataPacket->s) );
                    }
                }

                //Clean-up
                delete [] tmpdatabuf;
            }
        }
        delete [] tmpstrbuf; //No longer needed, so remove it
    }
    else if(packetType == PACKET_STREAM_RECV)
    {
        wxString packetString    = wxString::FromAscii( byteStringNoSpace ); //Get the string representation of the packet to be checked
        wxString packetStringWithSpace;
        // put spaces into packetStringWithSpace
        for( wxString::size_type i=0; i < packetString.size(); ++i ) {
            if( i%2 == 0 && i != 0 ) {
                packetStringWithSpace += ' ';
            }
            packetStringWithSpace += packetString[i];
        }
        wxString bigEndianHeader;
        if( packetString.size() > 3 )
            bigEndianHeader = packetString.substr(2,2);
        else
            bigEndianHeader = _("00");
        bigEndianHeader         += packetString.substr(0,2);

        if( (blockStatusType == STATUS_NOT_BLOCKED || blockStatusType == STATUS_MODIFIED) && RiPEApp::GetRiPE()->canLogRecv ) //If the recv packet is not blocked or ignored
        {
            char hexBuffer[9];
            hexBuffer[8] = 0; //intToHexB does not provide a NULL terminator
            intToHexB( (int)returnAddress, hexBuffer);
            //Populate list control
            if(RiPEClassPtr->CheckBox3->GetValue()) //If enable logging is on
            {
                long index = RiPEClassPtr->ListCtrl2->GetItemCount();
                index = RiPEClassPtr->ListCtrl2->InsertItem(index, wxString::FromAscii( hexBuffer ) );
                RiPEClassPtr->ListCtrl2->SetItem(index, 1, wxString::Format("%i", (int)recvSize) );
                RiPEClassPtr->ListCtrl2->SetItem(index, 2, wxString::FromAscii( displayString ) );
                RiPEClassPtr->ListCtrl2->SetItemPtrData(index, (wxUIntPtr)(new wxString(packetStringWithSpace)) );
                //Resize column
                RiPEApp::GetRiPE()->ListCtrl2->SetColumnWidth(2, wxLIST_AUTOSIZE);
            }

            //Populate tree control
            if(RiPEClassPtr->CheckBox4->GetValue()) //If enable logging is on
            {
                wxTreeItemIdValue cookie1;
                wxTreeItemIdValue cookie2;
                wxTreeItemId rootId   = RiPEClassPtr->TreeCtrl2->GetRootItem();
                wxTreeItemId childId1 = RiPEClassPtr->TreeCtrl2->GetFirstChild(rootId, cookie1);
                wxTreeItemId childId2;

                wxString opcodeLabel = _("");
                if( RiPEClassPtr->opcodesDlg != NULL ) {
                    const std::map<unsigned short, wxString> opcodesMap = RiPEClassPtr->opcodesDlg->GetOpcodes(OpcodesDlg::OPCODES_RECV);
                    std::map<unsigned short, wxString>::const_iterator iter;
                    iter = opcodesMap.find(HexToDecimal<unsigned short>(bigEndianHeader));
                    if( iter != opcodesMap.end() ) {
                        opcodeLabel = iter->second;
                    }
                }

                //If there is a child
                if( childId1.IsOk() )
                    for( ;; ) //For all headers
                    {
                        const RiPETreeItemData *ptrdata = (RiPETreeItemData *)RiPEClassPtr->TreeCtrl2->GetItemData(childId1);
                        //If we found a match with the header
                        if( ptrdata->GetPacket() == bigEndianHeader &&
                            ptrdata->GetSocket() == dataPacket->s )
                        {
                            childId2 = RiPEClassPtr->TreeCtrl2->GetFirstChild(childId1, cookie2);
                            if(childId2.IsOk()) for(;;) //For all addresses in the header child
                            {
                                if( RiPEClassPtr->TreeCtrl2->GetItemText(childId2) == wxString::FromAscii(hexBuffer) )
                                {
                                    //Add the packet text
                                    RiPEClassPtr->TreeCtrl2->AppendItem(childId2, _("(") + wxString::Format("%i", (int)recvSize) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                        new RiPETreeItemData(wxString::FromAscii(packetStringWithSpace),wxString::FromAscii(packetStringWithSpace),dataPacket->s) );
                                    break; //We found what we were looking for, so leave
                                }
                                childId2 = RiPEClassPtr->TreeCtrl2->GetNextChild(childId1, cookie2);
                                if(!childId2.IsOk()) //We reached the end of addresses and it wasn't found, so we need to add it
                                {
                                    //Add the address
                                    childId2 = RiPEClassPtr->TreeCtrl2->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                                    //Add the packet text
                                    RiPEClassPtr->TreeCtrl2->AppendItem(childId2, _("(") + wxString::Format("%i", (int)recvSize) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                        new RiPETreeItemData(wxString::FromAscii(packetStringWithSpace),wxString::FromAscii(packetStringWithSpace),dataPacket->s) );
                                    break; //We also need to break because we added the packet
                                }
                            }
                            else //No addresses inside of the header child, so add our own
                            {
                                //Add the address
                                childId2 = RiPEClassPtr->TreeCtrl2->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                                //Add the packet text
                                RiPEClassPtr->TreeCtrl2->AppendItem(childId2, _("(") + wxString::Format("%i", (int)recvSize) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                    new RiPETreeItemData(wxString::FromAscii(packetStringWithSpace),wxString::FromAscii(packetStringWithSpace),dataPacket->s) );
                            }
                            break;
                        }
                        childId1 = RiPEClassPtr->TreeCtrl2->GetNextChild(rootId, cookie1);
                        if(!childId1.IsOk()) //We reached the end of the header children and it wasn't found, so we need to add it
                        {
                            RiPETreeItemData *newData = new RiPETreeItemData(opcodeLabel, bigEndianHeader, dataPacket->s);
                            //Add the header
                            childId1 = RiPEClassPtr->TreeCtrl2->AppendItem(rootId, newData->GetValue(), -1, -1, newData );
                            //Add the address
                            childId2 = RiPEClassPtr->TreeCtrl2->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                            //Add the packet text
                            RiPEClassPtr->TreeCtrl2->AppendItem(childId2, _("(") + wxString::Format("%i", (int)recvSize) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                                new RiPETreeItemData(wxString::FromAscii(packetStringWithSpace),wxString::FromAscii(packetStringWithSpace),dataPacket->s) );
                            break; //We also need to break because we added the packet
                        }
                    }
                else //No children in root, so we need to add a new header, address, and packet
                {
                    RiPETreeItemData *newData = new RiPETreeItemData(opcodeLabel, bigEndianHeader, dataPacket->s);
                    //Add the header
                    childId1 = RiPEClassPtr->TreeCtrl2->AppendItem(rootId, newData->GetValue(), -1, -1, newData );
                    //Add the address
                    childId2 = RiPEClassPtr->TreeCtrl2->AppendItem(childId1, wxString::FromAscii(hexBuffer) );
                    //Add the packet text
                    RiPEClassPtr->TreeCtrl2->AppendItem(childId2, _("(") + wxString::Format("%i", (int)recvSize) + _(")  ") + wxString::FromAscii(displayString), -1, -1,
                                                        new RiPETreeItemData(wxString::FromAscii(packetStringWithSpace),wxString::FromAscii(packetStringWithSpace),dataPacket->s) );
                }
            }
        }
    }
    return;
}

void RiPE::OnMenuItem1Selected(wxCommandEvent& event)
{
    SaveLayout();
    return;
}

void RiPE::OnMenuItem3Selected(wxCommandEvent& event)
{
    LoadLayout();
    return;
}

//Save As...
void RiPE::OnMenuItem2Selected(wxCommandEvent& event)
{
    wxFileDialog *SaveDialog = new wxFileDialog(
        this, _("Save As"), wxEmptyString, defaultConfigLocation,
        _(".ini Files (*.ini)|*.ini"),
        wxFD_SAVE | wxFD_CHANGE_DIR, wxDefaultPosition);

	// Create an "open file" dialog for ADT files

    if (SaveDialog->ShowModal() == wxID_OK) // if the user clicks "Open" instead of "Cancel"
        SaveLayout( (std::string)SaveDialog->GetPath().mb_str() );
    SaveDialog->Destroy();
    return;
}

//Load From...
void RiPE::OnMenuItem4Selected(wxCommandEvent& event)
{
    wxFileDialog *OpenDialog = new wxFileDialog(
        this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
        _(".ini Files (*.ini)|*.ini|All Files (*.*)|*.*"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR, wxDefaultPosition);

	// Create an "open file" dialog for ADT files
    if (OpenDialog->ShowModal() == wxID_OK) // if the user clicks "Open" instead of "Cancel"
        LoadLayout( (std::string)OpenDialog->GetPath().mb_str() );
    OpenDialog->Destroy();
    return;
}

// returns true if authentication was still good
bool RiPE::AuthenticationPulse( wxString url )
{
    wxString authFile;
    HINTERNET hInternet, hFile;
    hInternet = InternetOpen( L"Browser", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL );

    if( hInternet )
    {
        hFile = InternetOpenUrl(
            hInternet,
            url.wc_str(),
            NULL,
            0,
            INTERNET_FLAG_RELOAD,
            0
            );

        if( hFile )
        {
            char buffer[4096];
            DWORD dwRead;
            while( InternetReadFile( hFile, buffer, 4095, &dwRead ) )
            {
                if( dwRead == 0 )
                    break;
                buffer[dwRead] = 0;
                authFile += buffer;
            }
        }
        InternetCloseHandle( hFile );
    }
    InternetCloseHandle( hInternet );

    bool success = ParseWebInstructions( authFile );
    if( success )
        authValid = MAX_AUTHORIZATION_ATTEMPTS;
    else
        --authValid;
    return authValid > 0;
    //return success; // disabled the pulse check
}

/** parses a file containing information about RiPE, such as HWID's,
 *  kill switch status, ad, title, and so on
 *  @return: true if it passes authorization/authentication, else false
**/
bool RiPE::ParseWebInstructions( wxString input )
{
    bool retVal = false;

    wxString instruction;
    input.Replace( _("\r"), _("") ); // remove carriage returns so that they don't mess us up
    while( input.size() > 0 )
    {
        instruction = input.BeforeFirst( NEWLINE );
        if( instruction == _("[Ad]") ) // [Ad] denotes that the next line contains an advertisement URL
        {
            input = input.AfterFirst( NEWLINE );
            wxString newURL = input.BeforeFirst( NEWLINE );
            if( adCtrl->GetCurrentURL() != newURL )
                adCtrl->LoadUrl( newURL );
            input = input.AfterFirst( NEWLINE );
        }
        else if( instruction == _("[Title]") ) // [Title] can be used to change the title of the program
        {
            input = input.AfterFirst( NEWLINE );
            SetLabel( input.BeforeFirst( NEWLINE ) );
            input = input.AfterFirst( NEWLINE );
        }
        else if( instruction == _("[Popup]") ) //When [Popup] is found, a "popup" will appear and display a simple message
        {
            wxString titleMessage, bodyMessage;

            input        = input.AfterFirst( NEWLINE );
            titleMessage = input.BeforeFirst( NEWLINE );

            input        = input.AfterFirst( NEWLINE );
            bodyMessage  = input.BeforeFirst( NEWLINE );
            bodyMessage.Replace( ESCAPED_NEWLINE, NEWLINE, true );

            wxMessageBox( bodyMessage, titleMessage );

            input = input.AfterFirst( NEWLINE );
        }
        else
            input = input.AfterFirst( NEWLINE ); // get next line
    }

    return retVal;
}

void RiPE::OnTimer3Trigger(wxTimerEvent& event)
{
}

void RiPE::OnListCtrl5ItemRClick(wxListEvent& event)
{
    wxMenu  menu;
	wxPoint point = wxGetMousePosition();
    // If from keyboard
    if ( (point.x == -1) && (point.y == -1) )
    {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
    else
    {
        point = ScreenToClient(point);
    }

	menu.Append(S_MULTI_SHIFT_UP, _("Clear &Send Tree"));
	menu.Append(S_MULTI_SHIFT_DOWN, _("Clear &Recv Tree"));
	menu.AppendSeparator();
	menu.Append(S_MULTI_EDIT_NAME, _("Clear &Recv List"));
	menu.Append(S_MULTI_EDIT_PACKET, _("Clear Send - &Block and Ignore List"));
	menu.Append(S_MULTI_EDIT_DELAY, _("Clear Recv - &Block and Ignore List"));
	menu.AppendSeparator();
	menu.Append(S_MULTI_DELETE_ITEM, _("Clear &Send List"));

    PopupMenu(&menu, point.x, point.y);
}

void RiPE::OnButton13Click(wxCommandEvent& event)
{
    long index = ListCtrl5->GetItemCount();
    index = ListCtrl5->InsertItem(index, TextCtrl4->GetValue() );
    ListCtrl5->SetItem(index, 1, "R");
    ListCtrl5->SetItem(index, 2, TextCtrl5->GetValue() );
    ListCtrl5->SetItem(index, 3, wxString::Format("%i", SpinCtrl1->GetValue() ) );
    //Set the current cooldown delay of the item to the max
    ListCtrl5->SetItemPtrData(index,
        (wxUIntPtr)new StoredPacketItem(TextCtrl5->GetValue(), SpinCtrl1->GetValue()) );
    //Resize column
    //ListCtrl5->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void RiPE::OnTextCtrl2TextEnter(wxCommandEvent& event)
{
	switch( SendPacketToProcess(std::string(TextCtrl2->GetValue().mb_str()), communicationLayer) )
	{
	case __INVALID_LENGTH:
		wxMessageBox(_("Invalid length.  Packets must contain at least a few bytes before sending them."), _("Error Sending Packet")); break;
	case __NOT_EVEN:
		wxMessageBox(_("Invalid length.  Make sure that your packet has an even number of characters.\nOnly valid hexadecimal numbers are allowed."), _("Error Sending Packet")); break;
	case __DC_HACK:
		wxMessageBox(_("DC hack attempt detected.  Please do not use my trainer for malicious purposes like DC hacking.\n\nThanks,\n~Riu."), _("DC Hack Alert")); break;
	}
}

void RiPE::SelectGame()
{
    curGame = gsmDlg->GetGame(); // get current game and store it in
    RefreshHookInterface();
}

void RiPE::RefreshHookInterface()
{
    wxString curDir = GetCurrentProcessDirectory();

    switch( curGame.GetVersion() )
    {
    case CUSTOM:
        defaultConfigLocation = curDir + "RiPE - Custom.ini";
        hookIntf = _str(CustomHookFacade);

        // The \\ prevents any other script from sharing this script's name
        communicationLayer.AddScript( wxT("\\") + curGame.GetScriptName(), curGame.GetScriptText() );
        communicationLayer.ToggleScript( wxT("\\") + curGame.GetScriptName(), true );
        break;
    case OTHER_SENDALL:
        defaultConfigLocation = curDir + "RiPE - Other.ini";
        hookIntf = _str(SendAllHookFacade);
        break;
    case OTHER_SEND:
        defaultConfigLocation = curDir + "RiPE - Other.ini";
        hookIntf = _str(SendHookFacade);
        break;
    case OTHER_SENDTO:
        defaultConfigLocation = curDir + "RiPE - Other.ini";
        hookIntf = _str(SendToHookFacade);
        break;
    case OTHER_WSASEND:
        defaultConfigLocation = curDir + "RiPE - Other.ini";
        hookIntf = _str(WSASendHookFacade);
        break;
    case OTHER_WSASENDTO:
        defaultConfigLocation = curDir + "RiPE - Other.ini";
        hookIntf = _str(WSASendToHookFacade);
        break;
    default:
        defaultConfigLocation = curDir + "RiPE - Default.ini";
        break;
    }

    communicationLayer.SetHookInterface(hookIntf);

    // Refresh to the defaults
    Button1->Enable();       // S - Block Block
    Button1->Show();         //S - Block Block
    Button2->Enable();       // Send button
    Button2->Show();         // Send button
    Button3->Show();         // Recv button
    Button3->Enable();       // Recv button
    Button4->Enable();       // S - Block Ignore
    Button4->Show();         // S - Block Ignore
    Button5->Enable();       // R - Block Block
    Button5->Show();         // R - Block Block
    Button6->Enable();       // R - Block Ignore
    Button6->Show();         // R - Block Ignore
    Button7->Enable();       // Multi - Send button
    Button7->Show();         // Multi - Send button
    Button9->Enable();       // R - Block Modify
    Button9->Show();         // R - Block Modify
    Button10->Disable();     // R - Block Intercept
    Button10->Hide();        // R - Block Intercept
    Button11->Disable();     // S - Block Intercept
    Button11->Hide();        // S - Block Intercept
    Button12->Enable();      // S - Block Modify
    Button12->Show();        // S - Block Modify
    Button13->Enable();      // Multi - Recv button
    Button13->Show();        // Multi - Recv button
    CheckBox1->Enable();     // S - List Enable Logging
    CheckBox1->Show();       // S - List Enable Logging
    CheckBox2->Enable();     // S - Tree Enable Logging
    CheckBox2->Show();       // S - Tree Enable Logging
    CheckBox3->Enable();     // R - List Enable Logging
    CheckBox3->Show();       // R - List Enable Logging
    CheckBox4->Enable();     // R - Tree Enable Logging
    CheckBox4->Show();       // R - Tree Enable Logging
    CheckBox5->Enable();     // Multi - Act as Group
    CheckBox5->Show();       // Multi - Act as Group
    CheckBox7->Enable();     // S - Tree Hook Send
    CheckBox8->Enable();     // S - List Hook Send
    CheckBox9->Enable();     // R - Tree Hook Recv
    CheckBox9->Show();       // R - Tree Hook Recv
    CheckBox10->Enable();    // R - List Hook Recv
    CheckBox10->Show();      // R - List Hook Recv
    SpinCtrl2->Hide();       // socket spin ctrl
    SpinCtrl3->Hide();       // socket spin ctrl (Multi)
    StaticText4->Hide();     // socket text
    ToggleButton1->Enable(); // Multi - Start Spamming
    ToggleButton1->Show();   // Multi - Start Spamming


    // Change defaults to the specific version
    switch( curGame.GetVersion() )
    {
    case CUSTOM:
        break;
    case OTHER_SENDALL:
    case OTHER_SEND:
    case OTHER_SENDTO:
    case OTHER_WSASEND:
    case OTHER_WSASENDTO:
        Button13->Disable();   // Multi - Recv button
        Button13->Hide();      // Multi - Recv button
        Button3->Disable();    // Recv button
        Button3->Hide();       // Recv button
        StaticText4->Show(); // socket text
        SpinCtrl2->Show();   // socket spin ctrl
        SpinCtrl3->Show();   // socket spin ctrl (Multi)
        break;
    default:
        break;
    }
    LoadLayout(); // load the new layout
    Layout();
}

// on select game / version
void RiPE::OnMenuItem5Selected(wxCommandEvent& event)
{
    SaveLayout(); // save the old layout
    SelectGame();
}

void RiPE::OnListCtrl5BeginDrag(wxListEvent& event)
{
}

// On Label Opcodes menu item selected
void RiPE::OnMenuItem6Selected(wxCommandEvent& event)
{
    if( opcodesDlg == NULL )
        opcodesDlg = new OpcodesDlg(this);
    // Display the opcodes dialog
    opcodesDlg->Show();
    SaveLayout();
    //UpdateTreeHeaders();
}

// Update headers with opcodesDlg
void RiPE::UpdateTreeHeaders()
{
    // If nothing to do
    if( opcodesDlg == NULL )
        return;

    wxTreeItemIdValue cookie;
    wxTreeItemId rootId;
    wxTreeItemId childId;
    const std::map<unsigned short, wxString> sendOps = opcodesDlg->GetOpcodes(OpcodesDlg::OPCODES_SEND);
    const std::map<unsigned short, wxString> recvOps = opcodesDlg->GetOpcodes(OpcodesDlg::OPCODES_RECV);
    std::map<unsigned short, wxString>::const_iterator iter;

    // Update Send
    iter = sendOps.begin();
    for( ; iter != sendOps.end(); ++iter )
    {
        rootId = TreeCtrl1->GetRootItem();
        childId = TreeCtrl1->GetFirstChild(rootId, cookie);
        for( ; childId.IsOk(); childId = TreeCtrl1->GetNextChild(rootId, cookie) )
        { // for all headers
            RiPETreeItemData *ptrdata = (RiPETreeItemData *)TreeCtrl1->GetItemData(childId);
            if( ptrdata != NULL )
            {
                if( iter->first == HexToDecimal<unsigned short>(ptrdata->GetPacket()) ) {
                    ptrdata->SetLabel(iter->second);
                    TreeCtrl1->SetItemText(childId, ptrdata->GetValue());
                }
            }
        }
    }

    // Update Recv
    iter = sendOps.begin();
    for( ; iter != sendOps.end(); ++iter ) {
        rootId = TreeCtrl1->GetRootItem();
        childId = TreeCtrl1->GetFirstChild(rootId, cookie);
        for( ; childId.IsOk(); childId = TreeCtrl1->GetNextChild(rootId, cookie) ) { // for all headers
            RiPETreeItemData *ptrdata = (RiPETreeItemData *)TreeCtrl1->GetItemData(childId);
            if( ptrdata != NULL )
                if( iter->first == HexToDecimal<unsigned short>(ptrdata->GetPacket()) ) {
                    ptrdata->SetLabel(iter->second);
                    TreeCtrl2->SetItemText(childId, ptrdata->GetValue());
                }
        }
    }
}

// When RiME is clicked
void RiPE::OnMenuItem7Selected(wxCommandEvent& event)
{
    wxMessageBox("Not supported yet.");
}

/**
 *  When the online RiPEST database is opened via the Tools options
 */
void RiPE::OnRiPESTDatabase(wxCommandEvent& event) {
    if( psDlg == NULL )
        psDlg = new PluginSearchDlg(this);
    if( psDlg->IsShown() )
        psDlg->Hide();
    else
    {
        psDlg->Show();
    }
}

void RiPE::OnSpinCtrl2Change(wxSpinEvent& event)
{
    SpinCtrl3->SetValue(SpinCtrl2->GetValue());
    communicationLayer.SetSocket(SpinCtrl2->GetValue());
}

void RiPE::OnSpinCtrl3Change(wxSpinEvent& event)
{
    SpinCtrl2->SetValue(SpinCtrl3->GetValue());
    communicationLayer.SetSocket(SpinCtrl3->GetValue());
}

// On open script dialog
void RiPE::OnButton15Click(wxCommandEvent& event)
{
    // open the new script dialog
    if( scriptDlg == NULL )
        scriptDlg = new NewScriptDlg(this, -1);
    scriptDlg->ClearCompiledScript();
    scriptDlg->ShowModal();

    if( scriptDlg->HasCompiledScript() )
    {
        wxTextEntryDialog *textDlg = new wxTextEntryDialog(this,
            wxT("Enter a unique name for your script"),
            wxT("Please enter a script name"),
            wxT(""),
            wxOK|wxCENTRE );
        do
        {
            textDlg->ShowModal();
        } while (!communicationLayer.AddScript(textDlg->GetValue(), scriptDlg->Script->GetText()));

        // Add script to check list
        wxString str = textDlg->GetValue();
        CheckListBox1->Insert(1, &str, CheckListBox1->GetCount());
        communicationLayer.SaveScripts();

        textDlg->Destroy();
    }
}

// When a hack is enabled / disabled
void RiPE::OnCheckListBoxToggled(wxCommandEvent& event)
{
    communicationLayer.ToggleScript( event.GetString(),
            CheckListBox1->IsChecked(event.GetSelection()));
}

// On save scripts button clicked
void RiPE::OnButton8Click1(wxCommandEvent& event)
{
    communicationLayer.SaveScripts();
    wxMessageBox("Scripts have been saved.", "Success");
}

// Searches for scripts in /scripts/ and loads all scripts found
void RiPE::LoadScripts()
{
    wxArrayString directoryFiles;
    wxDir dir(GetCurrentProcessDirectory() + wxT("scripts"));

    // Load scripts that aren't in a program folder
    if( dir.IsOpened() )
    {
        // Get files, but no sub-directories
        dir.GetAllFiles( dir.GetName(), &directoryFiles, _("*.*"), wxDIR_FILES | wxDIR_HIDDEN );
        for( wxArrayString::size_type i=0; i < directoryFiles.size(); ++i )
        {
            // Add each file and load its script
            wxString name = directoryFiles[i].AfterLast('\\');

            // If script loaded successfully, add it to the CheckListBox.
            if( communicationLayer.LoadScript( directoryFiles[i], name ) )
                CheckListBox1->Insert(1, &name, CheckListBox1->GetCount());
        }
    }

    // Load scripts that are in a program folder
    LoadProgramScripts();
}

void RiPE::LoadProgramScripts()
{
    wxString directoryName;
    wxString baseDirectory = GetCurrentProcessDirectory() + wxT("scripts\\");
    wxDir dir( baseDirectory );

    if( dir.IsOpened() )
    {
        bool keepGoing = dir.GetFirst( &directoryName, wxT("*.*"), wxDIR_DIRS );

        while( keepGoing )
        {
            wxArrayString scriptFiles;
            wxDir gameDir(baseDirectory + directoryName);

            if( gameDir.IsOpened() )
            {
                gameDir.GetAllFiles( gameDir.GetName(), &scriptFiles, _("*.*"), wxDIR_HIDDEN | wxDIR_FILES );

                // For each file
                for( wxArrayString::size_type j=0; j < scriptFiles.size(); ++j )
                {
                    // scriptName example is:  game/script
                    wxString scriptName = directoryName + wxT("\\") + scriptFiles[j].AfterLast('\\');
                    if( communicationLayer.LoadScript(baseDirectory + scriptName, scriptName) )
                    {
                        CheckListBox1->Insert( 1, &scriptName, CheckListBox1->GetCount() );
                    }
                }
            }
            keepGoing = dir.GetNext( &directoryName );
        }
    }
}

void RiPE::OnButton14Click(wxCommandEvent& event)
{
    AddPluginDlg apDlg(this);
    if( apDlg.IsShown() )
        apDlg.Hide();
    else
    {
        apDlg.ShowModal();

        // If script needs to be added
        if( apDlg.m_addScript )
        {
            // Get the filenames
            wxString pluginsLocation = GetCurrentProcessDirectory() + wxT("scripts");
            wxString gameName = apDlg.TextCtrlGame->GetValue();
            wxString gameVersion = apDlg.TextCtrlPlugin->GetValue();
            wxString filename = pluginsLocation + wxT("\\") + gameName + wxT("\\") + gameVersion;
            wxString oldFilename = apDlg.FilePickerCtrl1->GetFileName().GetFullPath();

            // Make necessary directories
            if( !wxDirExists(pluginsLocation) )
                wxMkdir( pluginsLocation );
            if( !wxDirExists(filename.BeforeLast('\\')) )
                wxMkdir( filename.BeforeLast('\\') );

            // Copy scripts to the directory
            wxCopyFile( oldFilename, filename, true );

            // Refresh the scripts
            if( communicationLayer.LoadScript(filename, filename.AfterFirst('\\')) )
                CheckListBox1->Insert( 1, &filename.AfterFirst('\\'), CheckListBox1->GetCount() );
        }
    }
}

// On delete script button clicked
void RiPE::OnButton16Click(wxCommandEvent& event)
{
    wxArrayInt arrayInt;
    if( CheckListBox1->GetSelections(arrayInt) > 0 )
    {
        for( size_t i=0; i < arrayInt.GetCount(); ++i )
        {
            wxString scriptName = CheckListBox1->GetString(arrayInt[i]+i);
            communicationLayer.ToggleScript(scriptName, false); // disable the script
            communicationLayer.DeleteScript(scriptName);
            CheckListBox1->Delete(arrayInt[i]+i); // delete from the check list box
        }
    }
}

// When "Find Plugins" is clicked
void RiPE::OnButton8Click2(wxCommandEvent& event)
{
    if( psDlg == NULL )
        psDlg = new PluginSearchDlg(this);
    if( psDlg->IsShown() )
        psDlg->Hide();
    else
    {
        psDlg->Show();
    }
}

void RiPE::DisplayPacket(wxListCtrl *listCtrl, wxTreeCtrl *treeCtrl, const LogPacket &packet, const int opcodesType) {
    // Populate list control
    if (listCtrl != NULL) { // if log list
        long index = listCtrl->GetItemCount();
        index = listCtrl->InsertItem(index, packet.returnAddress);
        listCtrl->SetItem(index, 1, packet.size);
        listCtrl->SetItem(index, 2, packet.packet);
        listCtrl->SetItemPtrData(index, (wxUIntPtr)(new wxString(packet.packetData)));
        listCtrl->SetColumnWidth(2, wxLIST_AUTOSIZE);
    }

    // Populate tree control
    if (treeCtrl != NULL) { // if log tree
        wxTreeItemIdValue cookie1;
        wxTreeItemIdValue cookie2;
        wxTreeItemId rootId = treeCtrl->GetRootItem();
        wxTreeItemId childId1 = treeCtrl->GetFirstChild(rootId, cookie1);
        wxTreeItemId childId2;

        wxString opcodeLabel = "";
        if (opcodesDlg != NULL) {
            const std::map<unsigned short, wxString> opcodesMap = opcodesDlg->GetOpcodes(
                    opcodesType);
            std::map<unsigned short, wxString>::const_iterator iter;
            iter = opcodesMap.find(HexToDecimal<unsigned short>(packet.opcode));
            if (iter != opcodesMap.end()) {
                opcodeLabel = iter->second;
            }
        }

        // If there is a child
        if (childId1.IsOk()) {
            for (;;) {
                const RiPETreeItemData *ptrdata = (RiPETreeItemData *)treeCtrl->GetItemData(childId1);
                // If we found a match with the header
                if (ptrdata->GetPacket() == packet.opcode 
                        && ptrdata->GetSocket() == packet.socket) {
                    childId2 = treeCtrl->GetFirstChild(childId1, cookie2);
                    if (childId2.IsOk()) {
                        for (;;) {
                            if (treeCtrl->GetItemText(childId2) == packet.returnAddress) {
                                // Add the packet
                                treeCtrl->AppendItem(childId2, 
                                        wxString::Format("(%s)  %s", packet.size, packet.packet), -1, -1,
                                        new RiPETreeItemData(packet.packetData, packet.packetData, packet.socket));
                                break;
                            }
                            childId2 = treeCtrl->GetNextChild(childId1, cookie2);
                            if (!childId2.IsOk()) { // We reached the end of addresses and it wasn't found, so we need to add it
                                // Add the address
                                childId2 = treeCtrl->AppendItem(childId1, packet.returnAddress);
                                // Add the packet text
                                treeCtrl->AppendItem(childId2, wxString::Format("(%s)  %s", packet.size, packet.packet), -1, -1,
                                        new RiPETreeItemData(packet.packetData, packet.packetData, packet.socket));
                                break;
                            }
                        }
                    } else { // No addresses inside of the header child, so add our own
                        // Add the address
                        childId2 = treeCtrl->AppendItem(childId1, packet.returnAddress);
                        // Add the packet text
                        treeCtrl->AppendItem(childId2, wxString::Format("(%s)  %s", packet.size, packet.packet), -1, -1,
                                new RiPETreeItemData(packet.packetData, packet.packetData, packet.socket));
                    }
                    break;
                }
                childId1 = treeCtrl->GetNextChild(rootId, cookie1);
                if (!childId1.IsOk()) { // We reached the end of the header children and it wasn't found, so we need to add it
                    RiPETreeItemData *newData = new RiPETreeItemData(opcodeLabel, packet.opcode, packet.socket);
                    // Add the header
                    childId1 = treeCtrl->AppendItem(rootId, newData->GetValue(), -1, -1, newData);
                    // Add the address
                    childId2 = treeCtrl->AppendItem(childId1, packet.returnAddress);
                    // Add the packet text
                    treeCtrl->AppendItem(childId2, wxString::Format("(%s)  %s", packet.size, packet.packet), -1, -1,
                            new RiPETreeItemData(packet.packetData, packet.packetData, packet.socket));
                    break;
                }
            }
        } else { // No children in root, so we need to add a new header, address, and packet
            RiPETreeItemData *newData = new RiPETreeItemData(opcodeLabel, packet.opcode, packet.socket);
            // Add the header
            childId1 = treeCtrl->AppendItem(rootId, newData->GetValue(), -1, -1, newData);
            // Add the address
            childId2 = treeCtrl->AppendItem(childId1, packet.returnAddress);
            // Add the packet text
            treeCtrl->AppendItem(childId2, wxString::Format("(%s)  %s", packet.size, packet.packet), -1, -1,
                    new RiPETreeItemData(packet.packetData, packet.packetData, packet.socket));
        }
    }
}

// Logs the results from the dll
void RiPE::OnTimerLogTrigger(wxTimerEvent& event) {
    LogPacket packet;
    
    // Log the send packets
    while (communicationLayer.GetNextSendPacket(packet)) {
        DisplayPacket(
            CheckBox1->IsChecked() ? ListCtrl1 : NULL, 
            CheckBox2->IsChecked() ? TreeCtrl1 : NULL, packet, OpcodesDlg::OPCODES_SEND);
    }
    
    // Log the recv packets
    while (communicationLayer.GetNextRecvPacket(packet)) {
        DisplayPacket(
            CheckBox3->IsChecked() ? ListCtrl2 : NULL, 
            CheckBox4->IsChecked() ? TreeCtrl2 : NULL, packet, OpcodesDlg::OPCODES_RECV);
    }
}

void RiPE::ProcessHotKey( __in int nCode, __in WPARAM wParam, __in LPARAM lParam) {

    KBDLLHOOKSTRUCT *kbhsPtr = (KBDLLHOOKSTRUCT *)lParam;

    // Only process on key up
    if (wParam != WM_KEYUP) {
        return;
    }

    // Check to see if we need to process this based on the PID
    HWND hWindow = GetForegroundWindow();
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWindow, &dwProcessId);
    if (dwProcessId == igFrame->GetPID()) {
        communicationLayer.ProcessHotKey(kbhsPtr->vkCode);
    }
}
