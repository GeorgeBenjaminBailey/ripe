#include "stdafx.h"

#include "hexspinctrl.h"
#include "../hexconverter.hpp"
#include "wx/msw/private.h"

#include <wx/msgdlg.h>

void wxHexSpinCtrl::FixStyle()
{
    const DWORD styleOld = ::GetWindowLong((HWND)m_hwndBuddy, GWL_STYLE);
    DWORD styleNew = styleOld & ~ES_NUMBER;
    if( styleNew != styleOld )
        ::SetWindowLong((HWND)m_hwndBuddy, GWL_STYLE, styleNew);
}

/*
int wxHexSpinCtrl::GetValue() const
{
    return wxSpinButton::GetValue();
}*/

void wxHexSpinCtrl::OnSpinCtrlChange( wxSpinEvent &event )
{
    SetValue( wxString::Format("%04X", GetValue()) );
}

void wxHexSpinCtrl::OnTextCtrlChange( wxCommandEvent &event )
{/*
    const wxString str = event.GetString();
    bool           isValid = true;
    for( wxString::size_type i=0; i < str.size(); ++i ) {
        if(  str[i] < '0' || 
            (str[i] > '9' && str[i] < 'A') ||
            (str[i] > 'F' && str[i] < 'a') ||
             str[i] > 'f' ) {
            isValid = false;
        }
    }
    
    if( isValid ) {
        event.SetInt(HexToDecimal<unsigned short>(str));//SetValue( HexToDecimal<unsigned short>(str) );
    } else {
        event.SetInt(GetValue());
    }
    SetValue(event.GetInt());
    */
    event.Skip();
}
