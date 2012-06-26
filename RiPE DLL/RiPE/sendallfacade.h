#ifndef __SENDALL_FACADE_H
#define __SENDALL_FACADE_H

#include "hookinterface.h"

class SendAllHookFacade : public HookInterface
{
private:
    HookInterface *sendIntf;
    HookInterface *sendtoIntf;
    HookInterface *wsasendIntf;
    HookInterface *wsasendtoIntf;

public:
    SendAllHookFacade();

    SendAllHookFacade(const bool useHardwareBreakpoints);

    ~SendAllHookFacade() {
        if (sendIntf != NULL) {
            delete sendIntf;
        }
        if (sendtoIntf != NULL) {
            delete sendtoIntf;
        }
        if (wsasendIntf != NULL) {
            delete wsasendIntf;
        }
        if (wsasendtoIntf != NULL) {
            delete wsasendtoIntf;
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

#endif //__SENDALL_FACADE_H
