#include "sendallfacade.h"

#include "sendfacade.h"
#include "sendtofacade.h"
#include "wsasendfacade.h"
#include "wsasendtofacade.h"

SendAllHookFacade::SendAllHookFacade() : HookInterface() {
    sendIntf      = new SendHookFacade(false);
    sendtoIntf    = new SendToHookFacade(false);
    wsasendIntf   = new WSASendHookFacade(false);
    wsasendtoIntf = new WSASendToHookFacade(false);
}

SendAllHookFacade::SendAllHookFacade(const bool useHardwareBreakpoints) : HookInterface(useHardwareBreakpoints) {
    sendIntf      = new SendHookFacade(useHardwareBreakpoints);
    sendtoIntf    = new SendToHookFacade(useHardwareBreakpoints);
    wsasendIntf   = new WSASendHookFacade(useHardwareBreakpoints);
    wsasendtoIntf = new WSASendToHookFacade(useHardwareBreakpoints);
}

// enables / disables the recv hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool SendAllHookFacade::HookRecv( bool enable )
{
    if (sendIntf != NULL) {
        sendIntf->HookRecv(enable);
    }
    if (sendtoIntf != NULL) {
        sendtoIntf->HookRecv(enable);
    }
    if (wsasendIntf != NULL) {
        wsasendIntf->HookRecv(enable);
    }
    if (wsasendtoIntf != NULL) {
        wsasendtoIntf->HookRecv(enable);
    }

    return true;
}

// enables / disables the send hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool SendAllHookFacade::HookSend( bool enable )
{
    if (sendIntf != NULL) {
        sendIntf->HookSend(enable);
    }
    if (sendtoIntf != NULL) {
        sendtoIntf->HookSend(enable);
    }
    if (wsasendIntf != NULL) {
        wsasendIntf->HookSend(enable);
    }
    if (wsasendtoIntf != NULL) {
        wsasendtoIntf->HookSend(enable);
    }

    return true;
}

void SendAllHookFacade::PacketRecv( const char *cstrPacket, SOCKET s/*=0 */ )
{
    if (sendIntf != NULL) {
        sendIntf->PacketRecv(cstrPacket, s);
    }
}

void SendAllHookFacade::PacketSend( const char *cstrPacket, SOCKET s/*=0 */ )
{
    if (sendIntf != NULL) {
        sendIntf->PacketSend(cstrPacket, s);
    }
}
