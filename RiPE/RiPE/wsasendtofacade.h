#ifndef __WSASENDTO_FACADE_H
#define __WSASENDTO_FACADE_H

#include "hookinterface.h"

class WSASendToHookFacade : public HookInterface
{
public:
    WSASendToHookFacade()
    {
        usingHardwareBreakpoints = true;
    }

    ~WSASendToHookFacade()
    {
        ;
    }

    // enables / disables the recv hook.
    // true for enable, false for disable.
    // returns true if succeeded, else false
    bool HookRecv( bool enable );

    // enables / disables the send hook.
    // true for enable, false for disable.
    // returns true if succeeded, else false
    bool HookSend( bool enable );

    void PacketRecv( const char *cstrPacket, SOCKET s=0 );

    void PacketSend( const char *cstrPacket, SOCKET s=0 );
};

#endif //__WSASENDTO_FACADE_H
