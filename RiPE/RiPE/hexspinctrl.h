#ifndef __HEX_SPIN_CTRL_H
#define __HEX_SPIN_CTRL_H

#include <wx/spinctrl.h>
#include <wx/event.h>

class wxHexSpinCtrl : public wxSpinCtrl {
protected:
    //int m_cur;

    void FixStyle();

public:
    wxHexSpinCtrl() {}

    wxHexSpinCtrl(wxWindow *parent,
        wxWindowID          id    = wxID_ANY,
        const wxString&     value = wxEmptyString,
        const wxPoint&      pos   = wxDefaultPosition,
        const wxSize&       size  = wxDefaultSize,
        long                style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
        int min = 0, int max = 100, int initial = 0,
        const wxString&     name  = wxT("wxSpinCtrl"))
    {
        Create(parent, id, value, pos, size, style, min, max, initial, name);

        FixStyle(); // fixes the style so we can use alphanumeric characters

        Connect(id, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(wxHexSpinCtrl::OnSpinCtrlChange));
        Connect(id, wxEVT_COMMAND_TEXT_UPDATED,     (wxObjectEventFunction)&wxHexSpinCtrl::OnTextCtrlChange);
    }

    ~wxHexSpinCtrl() {}

    void OnSpinCtrlChange( wxSpinEvent& event );
    void OnTextCtrlChange( wxCommandEvent& event );
};

#endif //__HEX_SPIN_CTRL_H