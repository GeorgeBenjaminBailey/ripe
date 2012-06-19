#include "../defs.h"

#include "../RiME/memscan.hpp"
#include "packet.h"
#include "script.h"
#include "customfacade.h"
#include "../defs.h"
#include "../hexconverter.hpp"
#include "../memreadwrite.h"

#include "../packetitem.h"
#include "../ripecontainer.h"
#include "../ripe_dll.h"

// an anonymous namespace
namespace
{
    class PacketItemCust : public PacketItem
    {
    private:
        int lengthOffset;

    public:
        PacketItemCust( GeneralPacketStruct   *pkt,
            const PacketFlag pktType,
            const DWORD      retAddr,
            const int lengthOff,
            const char *displayStr = NULL ) : PacketItem( pkt, pktType, retAddr, displayStr )
        {
            lengthOffset = lengthOff;
            packetSize = ReadAddress(dataPacket->lengthPtr) + lengthOffset;
            recvSize = GetSize();

            displayString = new char[((recvSize)*3)+1];
            GetStringData(true, displayString);
            displayString[(recvSize)*3] = '\0'; //NULL-terminate

            byteStringNoSpace = new char[((recvSize)*2)+1];
            GetStringData(false, byteStringNoSpace);
            byteStringNoSpace[(recvSize)*2] = '\0'; //NULL-terminate
        }
    };
}

// enables / disables the recv hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool CustomHookFacade::HookRecv( bool enable )
{
    if( hookRecvScript != NULL )
        RiPE::Container::GetHackDLL()->ProcessHookRecvEvent(*hookRecvScript, enable);
    return true;
}

// enables / disables the send hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool CustomHookFacade::HookSend( bool enable )
{
    if( hookSendScript != NULL )
        RiPE::Container::GetHackDLL()->ProcessHookSendEvent(*hookSendScript, enable);
    return true;
}

void CustomHookFacade::PacketRecv( const char *cstrPacket, SOCKET s/*=0 */ )
{
    if( packetRecvScript != NULL )
        RiPE::Container::GetHackDLL()->ProcessPacketRecvEvent(*packetRecvScript, cstrPacket, s);
}

void CustomHookFacade::PacketSend( const char *cstrPacket, SOCKET s/*=0 */ )
{
    if( packetSendScript != NULL )
        RiPE::Container::GetHackDLL()->ProcessPacketSendEvent(*packetSendScript, cstrPacket, s);
}

bool CustomHookFacade::LogPacket( short *headerPointer, char *dataPointer, DWORD *lengthPointer,
    long pktType, DWORD retAddr, int lengthCorrection, const char *displayStr )
{
    bool result = false;

    GeneralPacketStruct *generalPacket = new GeneralPacketStruct;
    generalPacket->headerShortPtr = headerPointer;
    generalPacket->data           = dataPointer;
    generalPacket->lengthPtr      = lengthPointer;

    PacketItemCust *pktItem = new PacketItemCust(
        generalPacket,
        (PacketFlag)pktType,
        retAddr,
        lengthCorrection,
        displayStr );

    pktItem->DispatchPacket();
    result = pktItem->IsBlocked();
    delete pktItem;

    return result;
}
