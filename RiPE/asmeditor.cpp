#include "stdafx.h"

#include "asmeditor.h"

// TODO: Split the ASM unit into another file and separate it from the GUI
namespace
{

    // supported registers
    const wxString wordList1 = 
        _("null ") +         // null
        _("def ") + 
        _("return ") +
        _("calling_convention cdecl msfastcall borlandfastcall syscall ") + // calling conventions
        _("gccthiscall msthiscall stdcall ") + // calling conventions
        _("if for while in range s r k else time enable disable "); // logical expressions

    // instructions and recognized functions
    const wxString wordList2 = 
        //_("byte word dword ") + // size functions
        _("sethookrecv sethooksend setpacketrecv setpacketsend ") + // RiPE hooks
        _("logpacketrecv logpacketsend ") + // RiPE hooks
        _("send recv block ") + // networking
        _("messagebox messageboxa logsend ") + // output
        _("cleardr clearint3 setdr setint3 ") + // debugging
        _("malloc free read write addressof ") + // memory
        _("scan len spoofreturn vtoa "); // utility
}

BEGIN_EVENT_TABLE(AsmEditor, wxStyledTextCtrl)
    EVT_STC_MARGINCLICK(wxID_ANY, AsmEditor::OnMarginClick)
END_EVENT_TABLE()

AsmEditor::AsmEditor( wxWindow *parent, 
    wxWindowID id/*=wxID_ANY*/, 
    const wxPoint &pos/*=wxDefaultPosition*/, 
    const wxSize &size/*=wxDefaultSize*/, 
    long style/*=0*/, 
    const wxString &name/*=wxSTCNameStr*/ ) : wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    InitializePrefs();

    SetKeyWords( 0, wordList1 );
    SetKeyWords( 1, wordList2 ); 
    SetTabWidth(4);
    SetUseTabs(false);

    Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(AsmEditor::OnMarginClick), NULL, this);
    Connect(wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&AsmEditor::OnMouseWheel,NULL,this);
}

// this enables support for margin folding
void AsmEditor::OnMarginClick( wxStyledTextEvent &event )
{
    if( event.GetMargin() == MARGIN_FOLD )
    {
        int lineClick = LineFromPosition(event.GetPosition());
        int levelClick = GetFoldLevel(lineClick);

        if( (levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0 )
        {
            ToggleFold(lineClick);
        }
    }
}

// when the ASM editor is scrolled
void AsmEditor::OnMouseWheel(wxMouseEvent& event)
{
    // if on the y axis
    if( event.GetWheelAxis() == 0 )
        m_delta.y += event.GetWheelRotation();
    else // if on the x axis
        m_delta.x += event.GetWheelRotation();

    // if y's delta is greater than the threshold
    if( (m_delta.y > 0) && (m_delta.y >= event.GetWheelDelta()) )
    {
        ScrollLines(m_delta.y / event.GetWheelDelta() *-1);
        m_delta.y = m_delta.y % event.GetWheelDelta();
    }
    else if( (m_delta.y < 0) && ((m_delta.y*-1) >= event.GetWheelDelta()) )
    {
        ScrollLines(m_delta.y / event.GetWheelDelta() * -1);
        m_delta.y = m_delta.y % event.GetWheelDelta();
    }

    /* As I am unable to test it, this will remain disabled for now.
    if( (m_delta.x != 0) ) {
        ScrollWindow(m_delta.x, 0, NULL);
        m_delta.x = 0;
    }*/
}

// private member functions

void AsmEditor::InitializePrefs()
{
    SetMarginWidth (MARGIN_LINE_NUMBERS, 40);
    StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (75, 75, 75) );
    StyleSetBackground (wxSTC_STYLE_LINENUMBER, wxColour (220, 220, 220));
    SetMarginType (MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);

    SetWrapMode (wxSTC_WRAP_NONE); //wxSTC_WRAP_WORD); // other choice is wxSCI_WRAP_NONE

    StyleClearAll();
    SetLexer(wxSTC_LEX_CPP); // we want to use CPP format -- not ASM
    wxFont f(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));

    for( int i=0; i < wxSTC_STYLE_LASTPREDEFINED; ++i )
        StyleSetFont( i, f );
    
    StyleSetForeground( mySTC_C_DEFAULT,                wxColour(0,0,0) ); 
    StyleSetForeground( mySTC_C_COMMENT,                wxColour(0,0,0) ); // /* */ comment
    StyleSetForeground( mySTC_C_COMMENTLINE,            wxColour(0,128,0) );   // // comment
    StyleSetForeground( mySTC_C_COMMENTDOC,             wxColour(0,0,0) ); // /** */ comment
    StyleSetForeground( mySTC_C_NUMBER,                 wxColour(90,90,90) );
    StyleSetForeground( mySTC_C_WORD,                   wxColour(40,86,165) ); // custom 0
    StyleSetForeground( mySTC_C_STRING,                 wxColour(163,21,21) );
    StyleSetForeground( mySTC_C_CHARACTER,              wxColour(163,21,21) );
    StyleSetForeground( mySTC_C_UUID,                   wxColour(0,0,0) ); // needs color
    StyleSetForeground( mySTC_C_PREPROCESSOR,           wxColour(0,0,0) ); // needs color
    StyleSetForeground( mySTC_C_OPERATOR,               wxColour(150,0,0) );
    StyleSetForeground( mySTC_C_IDENTIFIER,             wxColour(0,0,0) );
    StyleSetForeground( mySTC_C_STRINGEOL,              wxColour(0,0,0) ); // needs color
    StyleSetForeground( mySTC_C_VERBATIM,               wxColour(0,0,0) ); // needs color
    StyleSetForeground( mySTC_C_REGEX,                  wxColour(0,0,0) ); // needs color
    StyleSetForeground( mySTC_C_COMMENTLINEDOC,         wxColour(0,128,0) );
    StyleSetForeground( mySTC_C_WORD2,                  wxColour(150,0,150) ); // custom 1
    StyleSetForeground( mySTC_C_COMMENTDOCKEYWORD,      wxColour(0,0,0) ); // needs color
    StyleSetForeground( mySTC_C_COMMENTDOCKEYWORDERROR, wxColour(0,0,0) );
    StyleSetForeground( mySTC_C_GLOBALCLASS,            wxColour(0,0,0) ); // custom 3
    
    StyleSetBold( mySTC_C_NUMBER, true );
    StyleSetBold( mySTC_C_WORD2,  true );
    StyleSetBold( mySTC_C_WORD,   true );
    StyleSetFont( mySTC_C_COMMENT, f );
    StyleSetFont( mySTC_C_COMMENTDOC, f );
}
