#ifndef __HOOK_INTERFACE_H
#define __HOOK_INTERFACE_H

#include "../breakpoint.h"
#include "../ripecontainer.h"

class HookInterface
{
protected:
    bool usingHardwareBreakpoints;
    std::string *hookRecvScript;
    std::string *hookSendScript;
    std::string *packetRecvScript;
    std::string *packetSendScript;

public:
    HookInterface()
    {
        hookRecvScript   = NULL;
        hookSendScript   = NULL;
        packetRecvScript = NULL;
        packetSendScript = NULL;
        usingHardwareBreakpoints = true;
    }

    HookInterface(const bool useHardwareBreakpoints) {
        hookRecvScript   = NULL;
        hookSendScript   = NULL;
        packetRecvScript = NULL;
        packetSendScript = NULL;
        usingHardwareBreakpoints = useHardwareBreakpoints;
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
    void SetHookRecvScript(std::string *script)
    {
        hookRecvScript = script;
    }

    void SetHookSendScript(std::string *script)
    {
        hookSendScript = script;
    }

    void SetPacketRecvScript(std::string *script)
    {
        packetRecvScript = script;
    }

    void SetPacketSendScript(std::string *script)
    {
        packetSendScript = script;
    }
};

#endif //__HOOK_INTERFACE_H
