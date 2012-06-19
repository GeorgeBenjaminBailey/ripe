#ifndef RIU_ART_PROVIDER_H
#define RIU_ART_PROVIDER_H

#include <wx/artprov.h>

//xpm resources
#include "resources/add.xpm"
#include "resources/apple.xpm"
#include "resources/block_packet.xpm"
#include "resources/delete.xpm"
#include "resources/downarrow.xpm"
#include "resources/ignore_packet.xpm"
#include "resources/inject.xpm"
#include "resources/modify_packet.xpm"
#include "resources/refresh.xpm"
#include "resources/rightarrow.xpm"
#include "resources/rmulti.xpm"
#include "resources/rtree.xpm"
#include "resources/send.xpm"
#include "resources/smulti.xpm"
#include "resources/stree.xpm"
#include "resources/uparrow.xpm"

class RiuArtProvider : public wxArtProvider
{
public:
    RiuArtProvider() {}
    virtual ~RiuArtProvider() {}

protected:
    wxBitmap CreateBitmap(const wxArtID& id,
        const wxArtClient& client,
        const wxSize& size);
};

#endif // RIU_ART_PROVIDER_H
