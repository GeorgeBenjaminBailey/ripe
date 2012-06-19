#ifndef __CUSTOM_FACADE_H
#define __CUSTOM_FACADE_H

#include "hookinterface.h"

#include "../ripecontainer.h"

class CustomHookFacade : public HookInterface
{
public:
    CustomHookFacade()
    {
        usingHardwareBreakpoints = true;
    }

    ~CustomHookFacade()
    {
        HookSend( false );
        HookRecv( false );
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

    virtual bool LogPacket( short *headerPointer, char *dataPointer, DWORD *lengthPointer,
        long pktType, DWORD retAddr, int lengthCorrection, const char *displayStr = NULL );
};

#endif //__CUSTOM_FACADE_H
