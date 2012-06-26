#ifndef __WSASEND_FACADE_H
#define __WSASEND_FACADE_H

#include "hookinterface.h"

class WSASendHookFacade : public HookInterface
{
public:
    WSASendHookFacade()
    {
        usingHardwareBreakpoints = true;
    }

    WSASendHookFacade(const bool useHardwareBreakpoints) : HookInterface(useHardwareBreakpoints){
    }

    ~WSASendHookFacade()
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

#endif //__WSASEND_FACADE_H
