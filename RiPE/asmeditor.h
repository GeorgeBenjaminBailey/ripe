#ifndef __ASM_EDITOR_H
#define __ASM_EDITOR_H

#include <wx/stc/stc.h>

class AsmEditor : public wxStyledTextCtrl {
private:
    enum {
        MARGIN_LINE_NUMBERS,
        MARGIN_FOLD
    };

    // wxWidgets' definitions are incorrect for this
    enum {
        mySTC_C_DEFAULT = 0,
        mySTC_C_COMMENT,
        mySTC_C_COMMENTLINE,
        mySTC_C_COMMENTDOC,
        mySTC_C_NUMBER,
        mySTC_C_WORD,
        mySTC_C_STRING,
        mySTC_C_CHARACTER,
        mySTC_C_UUID,
        mySTC_C_PREPROCESSOR,
        mySTC_C_OPERATOR,
        mySTC_C_IDENTIFIER,
        mySTC_C_STRINGEOL,
        mySTC_C_VERBATIM,
        mySTC_C_REGEX,
        mySTC_C_COMMENTLINEDOC,
        mySTC_C_WORD2,
        mySTC_C_COMMENTDOCKEYWORD,
        mySTC_C_COMMENTDOCKEYWORDERROR,
        mySTC_C_GLOBALCLASS
    };

    struct MouseWheelDelta {
        int x;
        int y;

        MouseWheelDelta() {
            x = 0;
            y = 0;
        }
    } m_delta;

    //handlers
    void OnMouseWheel(wxMouseEvent& event);

    // private functions
    
    // Initialize the default preferences for style
    void InitializePrefs();

public:
    AsmEditor( wxWindow *parent, 
        wxWindowID id=wxID_ANY, 
        const wxPoint &pos=wxDefaultPosition, 
        const wxSize &size=wxDefaultSize, 
        long style=0, 
        const wxString &name=wxSTCNameStr );

protected:
    void OnMarginClick(wxStyledTextEvent &event);
    DECLARE_EVENT_TABLE()
};

#endif //__ASM_EDITOR_H
