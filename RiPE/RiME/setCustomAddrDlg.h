#ifndef SETCUSTOMADDRDLG_H
#define SETCUSTOMADDRDLG_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(setCustomAddrDlg)
	#include <wx/sizer.h>
	#include <wx/textctrl.h>
	#include <wx/dialog.h>
	//*)
#endif
//(*Headers(setCustomAddrDlg)
//*)

#include <stack>
#include <vector>
#include <cctype>
#include <algorithm>

#include "..\hexconverter.hpp"

enum ExpressionFlag
{
    IS_REGISTER    = 0x01,
    IS_IMMEDIATE   = 0x02,
    IS_ADDRESS     = 0x04,
    IS_OPERATOR    = 0x08,
    IS_BRACE       = 0x10,
    IS_INVALID     = 0x20
};

enum ExpressionType
{
//Registers
    EAX, AX, AH, AL,
    ECX, CX, CH, CL,
    EDX, DX, DH, DL,
    EBX, BX, BH, BL,
    ESP,
    EBP,
    ESI,
    EDI,
    EIP,
    EFL,
    SEG_CS,
    SEG_DS,
    SEG_ES,
    SEG_SS,
    SEG_FS,
    SEG_GS,
    ST0,
    ST1,
    ST2,
    ST3,
    ST4,
    ST5,
    ST6,
    ST7,
//Operators
    ADD,
    SUB,
    MUL,
    DIV,
    OR,
    XOR,
    AND,
    MOD,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    DEREFERENCE,
    ASSIGN,
    NEGATION,
    ADDRESS_OF,
    SHIFT_LEFT,
    SHIFT_RIGHT,
//Miscellaneous
	ADDRESS_A,
    ADDRESS_R,
	NUMBER_VALUE,
	UNKNOWN
};

struct Expression
{
    ExpressionFlag mask;
    ExpressionType type;
    union //Only valid if type == IMM
    {
        char  immChar;
        short immShort;
        long  immLong;
    };
};

class setCustomAddrDlg: public wxDialog
{
	public:

		setCustomAddrDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~setCustomAddrDlg();

        std::vector< std::vector<Expression> > savedScript; //The saved script that is executed by RiME

		//(*Declarations(setCustomAddrDlg)
		wxTextCtrl* TextCtrl1;
		//*)

	protected:

		//(*Identifiers(setCustomAddrDlg)
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(setCustomAddrDlg)
		void OnClose(wxCloseEvent& event);
		//*)

        bool SetSavedScript( __in const wxString inputStr );
        int  GetOperatorPrecedence( __in const ExpressionType op );
        std::vector<Expression> StringToExpression( __in wxString infixStr );
        std::vector<Expression> InfixToPostfix( __in std::vector<Expression> infixVector );

		DECLARE_EVENT_TABLE()
};

#endif
