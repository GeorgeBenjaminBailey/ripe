#ifndef __HOOK_INTERFACE_H
#define __HOOK_INTERFACE_H

#include "../breakpoint.h"

class HookInterface
{
protected:
    bool usingHardwareBreakpoints;
    Breakpoint *drPtr;
    wxString *hookRecvScript;
    wxString *hookSendScript;
    wxString *packetRecvScript;
    wxString *packetSendScript;

public:
    HookInterface()
    {
        drPtr = NULL;
        hookRecvScript   = NULL;
        hookSendScript   = NULL;
        packetRecvScript = NULL;
        packetSendScript = NULL;
        usingHardwareBreakpoints = true;
    }

    ~HookInterface()
    {
        if( hookRecvScript != NULL )
            delete hookRecvScript;
        if( hookSendScript != NULL )
            delete hookSendScript;
        if( packetRecvScript != NULL )
            delete packetRecvScript;
        if( packetSendScript != NULL )
            delete packetSendScript;

        if( drPtr != NULL )
            delete drPtr;
    }

    // enables / disables the recv hook.
    // true for enable, false for disable.
    // returns true if succeeded, else false
    virtual bool HookRecv( bool enable ) = 0;

    // enables / disables the send hook.
    // true for enable, false for disable.
    // returns true if succeeded, else false
    virtual bool HookSend( bool enable ) = 0;

    // Send a packet to the client from the server
    virtual void PacketRecv( const char *cstrPacket, SOCKET s=0 ) = 0;

    // Send a packet to the server from the client
    virtual void PacketSend( const char *cstrPacket, SOCKET s=0 ) = 0;

    // Enable / disable the use of hardware breakpoints.
    // When disabled, uses memory hooks.
    void EnableHardwareBreakpoints( bool enable ) {
        usingHardwareBreakpoints = enable;
    }

    virtual bool LogPacket( short *headerPointer, char *dataPointer, DWORD *lengthPointer,
                            long pktType, DWORD retAddr, int lengthCorrection, 
                            const char *displayStr = NULL )
    {
        return false;
    }

    // GETTERS / SETTERS
    void SetHookRecvScript(wxString *script)
    {
        hookRecvScript = script;
    }

    void SetHookSendScript(wxString *script)
    {
        hookSendScript = script;
    }

    void SetPacketRecvScript(wxString *script)
    {
        packetRecvScript = script;
    }

    void SetPacketSendScript(wxString *script)
    {
        packetSendScript = script;
    }
};

#endif //__HOOK_INTERFACE_H
