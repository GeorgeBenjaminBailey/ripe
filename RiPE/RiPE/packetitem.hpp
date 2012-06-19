#ifndef __PACKET_ITEM_HPP
#define __PACKET_ITEM_HPP

#include <list>

#include <wx/list.h>
#include <wx/regex.h>
#include <wx/string.h>
#include "partialpacket.hpp"

class StoredPacketItem : public wxListItem {
private:
    wxString buf;
    int      delay;
    int      maxDelay;
    std::list<PartialPacket *> partialPackets;

    /**
     * Converts a "string" to its hex representation.
     * e.g. "abc123" becomes 61 62 63 31 32 33.
     */
    wxString StringToHex (wxString str) {
        wxString result = "";
        str.Replace("\"", ""); // remove quotes

        for (unsigned int i=0; i < str.Len(); ++i) {
            result += wxString::Format("%02X", str[i]);
        }

        return result;
    }

    // parse and store the packet in partialPackets
    void ParsePacket( const wxString &pkt )
    {
        wxString newPkt = pkt;
        partialPackets.clear(); // remove all partialPackets
        wxRegEx removeWhite( _("[ \\r\\n\\t]"), wxRE_ADVANCED );
        if( removeWhite.Matches(newPkt) )
        {
            removeWhite.ReplaceAll(&newPkt, _(""));
        }
        wxRegEx expression( _("(\\[[;:, ?+-/*a-fA-F0-9p]*\\])|([a-fA-F0-9*? ]+)|([\"][^\"]*[\"])"), wxRE_ADVANCED );
        while( expression.Matches( newPkt ) )
        {
            size_t i=0, start=0, len=0;
            if( expression.GetMatch(&start, &len) )
            {
                wxString expMatch = newPkt.Mid(start, len);

                // add packet to our list
                if( expMatch.StartsWith(_("[")) ) // if dynamic partial packet
                {
                    partialPackets.push_back( new PartialPacketDynamicString(expMatch) );
                } else if (expMatch.StartsWith(_("\"")) ) { // if a string
                    partialPackets.push_back( new PartialPacketString(StringToHex(expMatch)) );
                }
                else // basic partial packet
                {
                    partialPackets.push_back( new PartialPacketString(expMatch) );
                }

                newPkt = newPkt.Mid(start+len);
            }
            else
                break;
        }
    }

public:
    StoredPacketItem()
    {
    }

    StoredPacketItem( const wxString &pkt, const int maximumDelay=0 )
    {
        maxDelay = maximumDelay;
        delay    = maxDelay;
        ParsePacket(pkt);
    }

    ~StoredPacketItem()
    {
        std::list<PartialPacket *>::iterator iter = partialPackets.begin();
        for( ; iter != partialPackets.end(); ++iter )
        {
            if( *iter != NULL )
                delete *iter; // free up memory
        }
    }

    int GetCurrentDelay()
    {
        return delay;
    }

    int GetMaxDelay()
    {
        return maxDelay;
    }

    // return size in nibbles
    int GetSize()
    {
        int result = 0;
        std::list<PartialPacket *>::iterator iter = partialPackets.begin();
        for( ; iter != partialPackets.end(); ++iter )
        {
            result += (*iter)->GetSize();
        }

        return result;
    }

    wxString GetPacket()
    {
        wxString result;
        std::list<PartialPacket *>::iterator iter = partialPackets.begin();
        for( ; iter != partialPackets.end(); ++iter )
        {
            result += (*iter)->GetPacket();
        }
        return result;
    }

    void SetCurrentDelay( int d )
    {
        delay = d;
    }

    wxString ToString()
    {
        wxString result;
        std::list<PartialPacket *>::iterator iter = partialPackets.begin();
        for( ; iter != partialPackets.end(); ++iter )
        {
            result += (*iter)->ToString();
        }
        return result;
    };
};

#endif //__PACKET_ITEM_HPP