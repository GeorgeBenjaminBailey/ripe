#ifndef __GENERIC_FACADE_H
#define __GENERIC_FACADE_H

#include "hookinterface.h"

class GenericHookFacade : public HookInterface
{
public:
    GenericHookFacade();

    ~GenericHookFacade()
    {
        HookSend( false );
        HookRecv( false );
        if( drPtr != NULL )
        {
            delete drPtr;
            drPtr = NULL;
        }
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

#endif //__GENERIC_FACADE_H
