#ifndef RIPE_H
#define RIPE_H

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#ifndef WX_PRECOMP
	//(*HeadersPCH(RiPE)
	#include <wx/listctrl.h>
	#include <wx/scrolwin.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/menu.h>
	#include <wx/textctrl.h>
	#include <wx/checklst.h>
	#include <wx/checkbox.h>
	#include <wx/panel.h>
	#include <wx/button.h>
	#include <wx/frame.h>
	#include <wx/timer.h>
	//*)
#endif
//(*Headers(RiPE)
#include <wx/treectrl.h>
#include <wx/spinctrl.h>
#include <wx/tglbtn.h>
#include <wx/aui/aui.h>
//*)
#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/imaglist.h>
#include <wx/msgdlg.h>

#include <Wininet.h>

// webview support
#if defined(__WXMSW__)
#include "webview/wxIETest2/wxiepanel.h"
#elif defined (__WXOSX__)
#include "webview/wxOSXWebkitCtrl/webkit.h"
#elif defined (__WXGTK__)
#include "webview/wxGtkWebKitCtrl/wxgtkwebkitctrl.h"
#else
#error "Sorry, your platform is not supported"
#endif
#include "webview/webview.h"

#include "defs.h"
#include "atohx.h"
#include "artProvider.h"
#include "memreadwrite.h"
//#include "PluginSearchDlg.h"
#include "RiPE/gameSelectMenu.h"
#include "RiPE/hookinterface.h"
#include "RiPE/communicationlayer.h"
#include "RiPE/OpcodesDlg.h"

#include "InjectorGadgetMain.h"
#include "NewScriptDlg.h"

// web control and url combobox ids
const int wxID_WEB = 9001;
const int wxID_URL = 9002;

const int MAX_AUTHORIZATION_ATTEMPTS = 2;
static HMODULE        hModuleProcess = 0;

struct RiPEMultiPacketItem
{
    int      delay;
    wxString data;

    RiPEMultiPacketItem()
    {
        delay = 0;
        data  = _("");
    }

    RiPEMultiPacketItem( int iDelay, wxString strData )
    {
        delay = iDelay;
        data  = strData;
    }
};

class RiPE : public wxFrame
{
    enum
    {
        ID_About = wxID_HIGHEST+9000,

        // file
        ID_OpenHref,
        ID_OpenLocation,
        ID_Close,
        ID_SaveAs,
        ID_PageSetup,
        ID_Print,
        ID_Exit,

        // edit
        ID_Undo,
        ID_Redo,
        ID_Cut,
        ID_Copy,
        ID_CopyLink,
        ID_Paste,
        ID_SelectAll,
        ID_Find,
        ID_FindAgain,

        // view
        ID_GoBack,
        ID_GoForward,
        ID_GoHome,
        ID_Stop,
        ID_Reload,
        ID_ZoomIn,
        ID_ZoomOut,
        ID_ZoomReset,
        ID_ShowSource,
        ID_ShowLinks,

        // help
        ID_GoHelp,
        ID_GoForums,
        ID_GoAbout
    };

	public:

		RiPE(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~RiPE();

		//(*Declarations(RiPE)
		wxAuiManager* AuiManager1;
		wxTextCtrl* TextCtrl4;
		wxPanel* Panel5;
		wxTimer Timer3;
		wxCheckBox* CheckBox9;
		wxMenuItem* MenuItem7;
        wxMenuItem* MenuItem8;
		wxListCtrl* ListCtrl1;
		wxButton* Button4;
		wxSpinCtrl* SpinCtrl1;
		wxMenuItem* MenuItem5;
		wxStaticText* StaticText2;
		wxTimer Timer4;
		wxPanel* Panel4;
		wxMenuItem* MenuItem2;
		wxScrolledWindow* ScrolledWindow3;
		wxScrolledWindow* ScrolledWindow1;
		wxScrolledWindow* ScrolledWindow6;
		wxButton* Button1;
		wxScrolledWindow* ScrolledWindow5;
		wxButton* Button14;
		wxListCtrl* ListCtrl4;
		wxPanel* Panel9;
		wxCheckBox* CheckBox3;
		wxScrolledWindow* ScrolledWindow4;
		wxCheckBox* CheckBox2;
		wxCheckListBox* CheckListBox1;
		wxToggleButton* ToggleButton1;
		wxTreeCtrl* TreeCtrl2;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxListCtrl* ListCtrl3;
		wxButton* Button2;
		wxPanel* Panel6;
		wxPanel* Panel3;
		wxListCtrl* ListCtrl2;
		wxCheckBox* CheckBox1;
		wxButton* Button6;
		wxButton* Button10;
		wxButton* Button11;
		wxButton* Button5;
		wxMenuItem* MenuItem3;
		wxButton* Button3;
		wxSpinCtrl* SpinCtrl3;
		wxStaticText* StaticText5;
		wxCheckBox* CheckBox10;
		wxButton* Button7;
		wxPanel* Panel7;
		wxScrolledWindow* ScrolledWindow2;
		wxMenuItem* MenuItem6;
		wxButton* Button9;
		wxTextCtrl* TextCtrl8;
		wxTextCtrl* TextCtrl2;
		wxSpinCtrl* SpinCtrl2;
		wxCheckBox* CheckBox4;
		wxTreeCtrl* TreeCtrl1;
		wxAuiNotebook* AuiNotebook1;
		wxCheckBox* CheckBox8;
		wxTextCtrl* TextCtrl1;
		wxCheckBox* CheckBox7;
		wxButton* Button17;
		wxTextCtrl* TextCtrl9;
		wxMenuBar* MenuBar1;
		wxPanel* Panel2;
		wxTextCtrl* TextCtrl5;
		wxCheckBox* CheckBox5;
		wxButton* Button15;
		wxButton* Button13;
		wxMenu* Menu2;
		wxStaticText* StaticText4;
		wxTextCtrl* TextCtrl3;
		wxButton* Button16;
		wxListCtrl* ListCtrl5;
		wxScrolledWindow* ScrolledWindow9;
		wxButton* Button8;
		wxTimer Timer1;
		wxButton* Button12;
		//*)
        wxMenu *MenuProcess;
        wxMenuItem *MenuItemSelectProcess;
        wxPanel *Panel8;
        wxWebView *adCtrl;
        wxImageList    *ImageListSmall;
        wxString  hookIntf; // an interface for logging and sending packets
        OpcodesDlg     *opcodesDlg;
        NewScriptDlg   *scriptDlg;
        PluginSearchDlg *psDlg;
        InjectorGadgetFrame *igFrame;

        GameClient      curGame;
        CommunicationLayer communicationLayer;
        gameSelectMenu *gsmDlg;
        int          authValid; // a counter that decreases each time auth fails, and resets every time auth succeeds
		int          Timer1LastUpdate;
		int          Timer2LastUpdate;
		long         sendGroupItemId;
		long         recvGroupItemId;
        bool         canLogSend; // TODO:  Remove these shitty requirements and replace with a queue
        bool         canLogRecv;

        std::string  defaultConfigLocation;

		void         RefreshSendPacketDelay(); //Restores the delay on all packets
		void         refreshRecvPacketDelay();

        void MultiPacketSend();
        void MultiPacketRecv();

		bool AddPacketToBlock(__inout wxListCtrl* listctrl,
							  __in const wxTextCtrl* packet1,
							  __in const wxString&   type);

		bool AddPacketToBlock(__inout wxListCtrl* listctrl,
							  __in const wxTextCtrl* packet1,
							  __in const wxTextCtrl* packet2,
							  __in const wxString&   type);

        // opens a menu for selecting the current game / version
        void SelectGame();

        void LoadScripts();
        void LoadProgramScripts();
        void ProcessHotKey(__in int nCode, __in WPARAM wParam, __in LPARAM lParam);

	protected:

		//(*Identifiers(RiPE)
		static const long ID_CHECKBOX2;
		static const long ID_PANEL4;
		static const long ID_CHECKBOX7;
		static const long ID_TREECTRL1;
		static const long ID_SCROLLEDWINDOW3;
		static const long ID_CHECKBOX1;
		static const long ID_PANEL5;
		static const long ID_CHECKBOX8;
		static const long ID_LISTCTRL1;
		static const long ID_SCROLLEDWINDOW1;
		static const long ID_LISTCTRL3;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON4;
		static const long ID_BUTTON11;
		static const long ID_BUTTON12;
		static const long ID_TEXTCTRL9;
		static const long ID_SCROLLEDWINDOW5;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_PANEL2;
		static const long ID_PANEL9;
		static const long ID_TEXTCTRL4;
		static const long ID_TEXTCTRL5;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON7;
		static const long ID_BUTTON13;
		static const long ID_LISTCTRL5;
		static const long ID_CHECKBOX5;
		static const long ID_SPINCTRL3;
		static const long ID_TOGGLEBUTTON1;
		static const long ID_SCROLLEDWINDOW6;
		static const long ID_CHECKBOX4;
		static const long ID_PANEL6;
		static const long ID_CHECKBOX9;
		static const long ID_TREECTRL2;
		static const long ID_SCROLLEDWINDOW2;
		static const long ID_CHECKBOX3;
		static const long ID_PANEL7;
		static const long ID_CHECKBOX10;
		static const long ID_LISTCTRL2;
		static const long ID_SCROLLEDWINDOW9;
		static const long ID_LISTCTRL4;
		static const long ID_TEXTCTRL3;
		static const long ID_BUTTON5;
		static const long ID_BUTTON6;
		static const long ID_BUTTON10;
		static const long ID_BUTTON9;
		static const long ID_TEXTCTRL8;
		static const long ID_SCROLLEDWINDOW4;
		static const long ID_BUTTON8;
		static const long ID_BUTTON14;
		static const long ID_BUTTON15;
		static const long ID_CHECKLISTBOX1;
		static const long ID_BUTTON16;
		static const long ID_BUTTON17;
		static const long ID_PANEL3;
		static const long ID_AUINOTEBOOK1;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_PANEL1;
		static const long ID_TIMER1;
		static const long ID_SAVE;
		static const long ID_SAVE_AS;
		static const long ID_LOAD;
		static const long ID_LOAD_AS;
		static const long ID_LABEL_OPCODES;
		static const long ID_MENUITEM1;
		static const long ID_RIME;
		static const long ID_TIMER3;
		static const long ID_TIMER4;
        static const long ID_RIPESTDATABASE;
		//*)
        static const long ID_SELECTPROCESS;
        static const long ID_PANEL8;
        static const long ID_HTMLWINDOW;

	private:

		void OnHtmlWindowHtmlLinkClicked(wxWebNavigationEvent& event);
        void AddSponsorPanel();
		//(*Handlers(RiPE)
		void OnAuiManager1PaneButton(wxAuiManagerEvent& event);
		void OnAuiNotebook4PageClose(wxAuiNotebookEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnListCtrl1ItemActivated(wxListEvent& event);
		void OnListCtrl2ItemActivated(wxListEvent& event);
		void OnListCtrl1ItemRClick(wxListEvent& event);
		void OnListCtrl1DeleteAllItems(wxListEvent& event);
		void OnListCtrl2DeleteAllItems(wxListEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton5Click(wxCommandEvent& event);
		void OnButton6Click(wxCommandEvent& event);
		void OnButton7Click(wxCommandEvent& event);
		void OnButton8Click(wxCommandEvent& event);
		void OnToggleButton2Toggle(wxCommandEvent& event);
		void OnToggleButton1Toggle(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnTimer2Trigger(wxTimerEvent& event);
		void OnTimer3Trigger(wxTimerEvent& event);
		void OnListCtrl5KeyDown(wxListEvent& event);
		void OnTreeCtrl1ItemActivated(wxTreeEvent& event);
		void OnTreeCtrl2ItemActivated(wxTreeEvent& event);
		void OnListCtrl3KeyDown(wxListEvent& event);
		void OnListCtrl4KeyDown(wxListEvent& event);
		void OnListCtrl5ItemActivated(wxListEvent& event);
		void OnCheckBox5Click(wxCommandEvent& event);
		void OnCheckBox6Click(wxCommandEvent& event);
		void OnButton10Click(wxCommandEvent& event);
		void OnButton9Click(wxCommandEvent& event);
		void OnButton11Click(wxCommandEvent& event);
		void OnButton12Click(wxCommandEvent& event);
		void OnCheckBox7Click(wxCommandEvent& event);
		void OnCheckBox8Click(wxCommandEvent& event);
		void OnCheckBox9Click(wxCommandEvent& event);
		void OnCheckBox10Click(wxCommandEvent& event);
		void OnMenuItem1Selected(wxCommandEvent& event);
		void OnMenuItem3Selected(wxCommandEvent& event);
		void OnMenuItem2Selected(wxCommandEvent& event);
		void OnMenuItem4Selected(wxCommandEvent& event);
		void OnAuiNotebook1PageChanging(wxAuiNotebookEvent& event);
		void OnListCtrl5ItemRClick(wxListEvent& event);
		void OnButton13Click(wxCommandEvent& event);
		void OnTextCtrl2TextEnter(wxCommandEvent& event);
		void OnMenuItem5Selected(wxCommandEvent& event);
		void OnListCtrl5BeginDrag(wxListEvent& event);
		void OnTimerAdvertisementTrigger(wxTimerEvent& event);
		void OnMenuItem6Selected(wxCommandEvent& event);
		void OnMenuItem7Selected(wxCommandEvent& event);
        void OnRiPESTDatabase(wxCommandEvent& event);
		void OnSpinCtrl2Change(wxSpinEvent& event);
		void OnSpinCtrl3Change(wxSpinEvent& event);
		void OnButton15Click(wxCommandEvent& event);
		void OnButton8Click1(wxCommandEvent& event);
		void OnButton14Click(wxCommandEvent& event);
		void OnButton16Click(wxCommandEvent& event);
		void OnButton8Click2(wxCommandEvent& event);
		//*)
        void OnMenuItemSelectProcess(wxCommandEvent& event);
        void OnTimerLogTrigger(wxTimerEvent& event);
		void OnContextMenuDeleteSendList(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuDeleteRecvList(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuDeleteSendBlockList(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuDeleteRecvBlockList(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuDeleteSendTree(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuDeleteRecvTree(wxCommandEvent& WXUNUSED(event));

        void DisplayPacket(wxListCtrl *listCtrl, wxTreeCtrl *treeCtrl, const LogPacket &packet, const int opcodesType);

		// S - Multi
		void OnContextMenuSMultiShiftUp(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuSMultiShiftDown(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuSMultiEditName(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuSMultiEditPacket(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuSMultiEditDelay(wxCommandEvent& WXUNUSED(event));
		void OnContextMenuSMultiDeleteItem(wxCommandEvent& WXUNUSED(event));

        void OnCheckListBoxToggled(wxCommandEvent& event);

		void OnContextMenu(wxContextMenuEvent& event);

        // returns true if authentication was still good
        bool AuthenticationPulse( wxString url );

        /** parses a file containing information about RiPE, such as HWID's,
         *  kill switch status, ad, title, and so on
         *  @return-vale: true if it passes authorization/authentication, else false
        **/
        bool ParseWebInstructions( wxString input );
        void UpdateTreeHeaders();

        void SaveLayout();
        void LoadLayout();
        void SaveLayout( std::string saveLocation );
        void LoadLayout( std::string loadLocation );

        void RefreshHookInterface();

        // Calls the updater to change the .dll slightly
        void ChangeSelf();

        HHOOK m_hook;

		DECLARE_EVENT_TABLE()
};

//IDs for the menu commands
enum
{
	SEND_LIST_CLEAR = wxID_HIGHEST+1000,
	RECV_LIST_CLEAR,
	SEND_BLOCK_LIST_CLEAR,
	RECV_BLOCK_LIST_CLEAR,
	SEND_TREE_CLEAR,
	RECV_TREE_CLEAR,
	// S - Multi
    S_MULTI_SHIFT_UP,
    S_MULTI_SHIFT_DOWN,
	S_MULTI_EDIT_NAME,
	S_MULTI_EDIT_PACKET,
	S_MULTI_EDIT_DELAY,
	S_MULTI_DELETE_ITEM
};

enum PacketFlag
{
    PACKET_STREAM_RECV = 0x00,
    PACKET_STREAM_SEND = 0x01,
    PACKET_NULL        = 0x02,
    PACKET_ZERO        = 0x04
};

enum BlockStatus
{
    STATUS_NOT_BLOCKED = 0,
    STATUS_IGNORED,
    STATUS_BLOCKED,
    STATUS_MODIFIED,
    STATUS_INTERCEPTED
};

//General-purpose packet structure that should work for most, if not all, games
struct GeneralPacketStruct
{
    union
    {
        char  *headerCharPtr;
        short *headerShortPtr;
    };
    char  *data;
    union
    {
        DWORD *lengthPtr;  //By default this should be the length of the header + data
                           //If, for whatever reason, it's something else, then you will
                           //need to change the GetSize() function accordingly
        WORD  *lengthWordPtr;
    };
    SOCKET s;
    std::string ip; // IP address

    GeneralPacketStruct()
    {
        headerCharPtr = NULL;
        data          = NULL;
        lengthPtr     = NULL;
        s             = 0;
        ip            = "";
    }
};

class PacketItem
{
protected:
    RiPE                *RiPEClassPtr;
    GeneralPacketStruct *dataPacket;
    PacketFlag           packetType;
    DWORD                returnAddress;
    bool                 blocked;
    char                *displayString;        //Keep track of receive data because of way that the address for receive works
    char                *byteStringNoSpace;
    BlockStatus          blockStatusType;   //Used for receive packets to see if they're blocked or ignored
    DWORD                recvSize;          //Keeps track of receive packet size
    std::vector<DWORD>   usedAddresses;     //Keeps track of already used recv addresses and prevents unnecessary repetition

    DWORD              packetSize;        //An optional value that can be assigned by child classes

    //Used inside of block
    void CheckIfBlocked( __in const wxListCtrl *listctrlPtr,
                         __in const wxString&   packetString );

public:
    PacketItem( RiPE            *classPtr,
                GeneralPacketStruct   *pkt,
                const PacketFlag pktType,
                const DWORD      retAddr,
                const char      *displayStr = NULL );

    ~PacketItem();

    DWORD GetSize()
    {
        return packetSize == 0 ? ReadAddress(dataPacket->lengthPtr) : packetSize;
    }

    DWORD GetReturn()
    {
        return returnAddress;
    }

    bool SetReturn( __in DWORD addr );

    //Blocks the packet from being processed
    void Block();
    bool IsBlocked();

    //Return string representation of packet data
    //Requires a char* buffer of ((dataPacket->size)*2)+1 for no space
    //Requires a char* buffer of ((dataPacket->size)*3)+1 for space
    //This function does NOT add a NULL-terminator
    bool GetStringData( bool addSpace, char *outbuf );

    //Function used in blocking receive packets (has to be done differently because of the way receive works)
    virtual void BlockRecv();

    void DispatchPacket();
};

#endif
