#include "stdafx.h"
#include "artProvider.h"

wxBitmap RiuArtProvider::CreateBitmap(const wxArtID& id,
    const wxArtClient& client,
    const wxSize& size)
{
    //if( client == wxART_BUTTON ) {
    if( id == wxT("ADD_XPM") )
        return wxBitmap(add_xpm);
    else if( id == wxT("APPLE_XPM") )
        return wxBitmap(apple_xpm);
    else if( id == wxT("BLOCK_PACKET_XPM") )
        return wxBitmap(block_packet_xpm);
    else if( id == wxT("DELETE_XPM") )
        return wxBitmap(delete_xpm);
    else if( id == wxT("DOWNARROW_XPM") )
        return wxBitmap(downarrow_xpm);
    else if( id == wxT("IGNORE_PACKET_XPM") )
        return wxBitmap(ignore_packet_xpm);
    else if (id == wxT("INJECT_XPM"))
        return wxBitmap(inject_xpm);
    else if( id == wxT("MODIFY_PACKET_XPM") )
        return wxBitmap(modify_packet_xpm);
    else if (id == wxT("REFRESH_XPM"))
        return wxBitmap(refresh_xpm);
    else if( id == wxT("RIGHTARROW_XPM") )
        return wxBitmap(rightarrow_xpm);
    else if( id == wxT("RMULTI_XPM") )
        return wxBitmap(rmulti_xpm);
    else if( id == wxT("RTREE_XPM") )
        return wxBitmap(rtree_xpm);
    else if( id == wxT("SEND_XPM") )
        return wxBitmap(send_xpm);
    else if( id == wxT("SMULTI_XPM") )
        return wxBitmap(smulti_xpm);
    else if( id == wxT("STREE_XPM") )
        return wxBitmap(stree_xpm);
    else if( id == wxT("UPARROW_XPM") )
        return wxBitmap(uparrow_xpm);
    else
        return wxNullBitmap;
    // possibly more Art Ids follow
}
