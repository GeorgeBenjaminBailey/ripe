#include "stdafx.h"

#include <wx/msgdlg.h>

#include "../RiME/memscan.hpp"
#include "packet.h"
#include "genericfacade.h"
#include "RiPEApp.h"
#include "../defs.h"
#include "../hexconverter.hpp"
#include "../memreadwrite.h"
#include "../RiPE.h"

// an anonymous namespace
namespace
{
    class PacketItemCust : public PacketItem
    {
    public:
        PacketItemCust( RiPE *classPtr,
            GeneralPacketStruct   *pkt,
            const PacketFlag pktType,
            const DWORD      retAddr ) : PacketItem( classPtr, pkt, pktType, retAddr )
        {
            recvSize = GetSize();

            displayString = new char[((recvSize)*3)+1];
            GetStringData(true, displayString);
            displayString[(recvSize)*3] = '\0'; //NULL-terminate

            byteStringNoSpace = new char[((recvSize)*2)+1];
            GetStringData(false, byteStringNoSpace);
            byteStringNoSpace[(recvSize)*2] = '\0'; //NULL-terminate
        }

        DWORD GetSize()
        {
            return *(dataPacket->lengthWordPtr);
        }
    };

    struct SendStructure //Send packet from send
    {
        DWORD returnAddress;
    };

    struct RecvStructure //Receive packet from recv
    {
        DWORD  returnAddress;
        SOCKET s;
        char  *buf;
        int    len;
        int    flags;
    };

    bool __stdcall RecvPacketEvent( RecvStructure *esp );

    bool __stdcall RecvPacketEventTry( RecvStructure *esp );

    bool __stdcall SendPacketEvent( SendStructure *esp, DWORD ecx );

    bool __stdcall SendPacketEventTry( SendStructure *esp, DWORD ecx );

    // addresses for Send/Recv functions in the WS2_32 dll
    DWORD SendFunction         = 0;
    DWORD CSNSocket_Size       = 0;
    DWORD CSNSocket_Packet     = 0;
    DWORD CSNSocket_Add_unsignedchar = 0;
    DWORD CSNSocket_Add_unsignedcharptr_unsignedshort = 0;
    DWORD SendFunction_Class   = 0;
    DWORD SendFunction_Ret     = 0;

    DWORD RecvFunction       = 0;
    DWORD RecvFunction_Ret   = 0;

    // Populates addresses
    void PopulateAddresses()
    {
        Scanner scanner;

        //  

    }

    void __declspec(naked) PacketRecvHookCave()
    {
        __asm
        {
        }
    }

    void __declspec(naked) PacketSendHookCave()
    {
        __asm
        {
            pushad
                pushfd
                push ecx
                push esp
                add  dword ptr [esp],0x24             // account for pushad + push ecx
                call SendPacketEventTry               //Called on outgoing packets to the server
                cmp  eax,0                            // see if packet is blocked

                //jnz  SkipSendPacket                   //If true, then block the packet
                popfd
                popad
                //Original instructions
                push esi
                jmp  dword ptr [SendFunction_Ret]

            //SkipSendPacket:
            popfd
                popad
                ret                                   //Returns, effectively blocking the packet
        }
    }

    bool __stdcall RecvPacketEvent( RecvStructure *esp )
    {
        return false;
    }

    bool __stdcall RecvPacketEventTry( RecvStructure *esp )
    {
        bool result = false;
        __try
        {
            result = RecvPacketEvent( esp );
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            ;
        }
        return result;
    }

    bool __stdcall SendPacketEventTry( SendStructure *esp, DWORD ecx )
    {
        bool result = false;

        __try
        {
            result = SendPacketEvent( esp, ecx );
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            ;
        }

        return result;
    }

    bool __stdcall SendPacketEvent( SendStructure *esp, DWORD ecx )
    {
        bool result = false;

        //GeneralPacket is cleaned up by PacketItemSend's destructor
        GeneralPacketStruct *generalPacket = new GeneralPacketStruct;
        generalPacket->headerShortPtr      = (short *)(ecx+CSNSocket_Packet);//sendPacketPointer->dataPacket->bytes;
        generalPacket->data                = (char *)(ecx+CSNSocket_Packet+2); // this is really +1, but we want good formatting
        generalPacket->lengthPtr           = (DWORD *)(ecx+CSNSocket_Size);

        PacketItemCust *pktItem = new PacketItemCust( 
            RiPEApp::GetRiPE(),
            generalPacket,
            PACKET_STREAM_SEND,
            esp->returnAddress );

        pktItem->DispatchPacket();
        result = pktItem->IsBlocked();
        delete pktItem; //The destructor of msPacketItem takes care of generalPacket

        return result;
    }
}

namespace packets
{
    class PacketInterfaceWindSlayer2 : public PacketInterface
    {
    private:
        //Sends a packet to the server
        void SendPacketInternal()
        {
            int   pktLen    = (*dataPacket.lengthPtr);
            int   dataLen   = pktLen - 2;
            char *pktBuf    = dataPacket.data;
            DWORD header    = *(dataPacket.headerCharPtr);
            DWORD data1     = *(char *)((DWORD)dataPacket.headerCharPtr+1);

            // Block gold exploit
            if( header == 0xA1 )
                return;

            __asm
            {
                push header
                    mov  ecx,[SendFunction_Class]
                mov  ecx,[ecx]
                call dword ptr [CSNSocket_Add_unsignedchar]
            }
            if( pktLen > 1 )
            {
                __asm
                {
                    push data1
                        mov  ecx,[SendFunction_Class]
                    mov  ecx,[ecx]
                    call dword ptr [CSNSocket_Add_unsignedchar]
                }
            }
            if( pktLen > 2 )
            {
                __asm
                {
                    push dword ptr [dataLen]
                    push dword ptr [pktBuf]
                    mov  ecx,[SendFunction_Class]
                    mov  ecx,[ecx]
                    call dword ptr [CSNSocket_Add_unsignedcharptr_unsignedshort]
                }
            }
            __asm
            {
                push 1
                mov  ecx,[SendFunction_Class]
                mov  ecx,[ecx]
                call dword ptr [SendFunction]
            }
        }

        void SendPacketInternal(
            __in GeneralPacketStruct pkt );

        //Processes a packet "from" the server
        void ProcessPacketInternal()
        {
            DWORD  pktSz   = *(dataPacket.lengthPtr);
            char  *pktData = dataPacket.data;
        }

    public:
        PacketInterfaceWindSlayer2() : PacketInterface(){}

        PacketInterfaceWindSlayer2(
            __in const PacketInterfaceWindSlayer2 &other ) :
        PacketInterface( other ){}

        PacketInterfaceWindSlayer2( 
            __in     GeneralPacketStruct  pkt, 
            __in     const PacketFlag     pktType, 
            __in     const DWORD          retAddr,
            __in_opt const bool           modifySize=false ) : 
        PacketInterface( pkt, pktType, retAddr )
        {
            recvSize = GetSize();
        }

        DWORD GetSize()
        {
            packetSize = 0;
            if( dataPacket.lengthPtr != NULL )
                packetSize = (*(dataPacket.lengthWordPtr));
            return packetSize;
        }

        //Send/Process packet based on type
        void SendPacket()
        {
            if( packetType == PACKET_STREAM_RECV )
                ProcessPacketInternal();
            else if( packetType == PACKET_STREAM_SEND )
                SendPacketInternal();
        }

        PacketInterfaceWindSlayer2& operator =(PacketInterfaceWindSlayer2 &other)
        {
            dataPacket        = other.dataPacket;
            packetType        = other.packetType;
            returnAddress     = other.returnAddress;
            blocked           = other.blocked;
            blockStatusType   = other.blockStatusType;
            packetSize        = other.packetSize;
            recvSize          = other.recvSize;
            return *this;
        }
    };
}

// enables / disables the recv hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool GenericHookFacade::HookRecv( bool enable )
{
    PopulateAddresses();
    bool result = true;
    return result;
}

unsigned char *bypassRegion = NULL;
unsigned long  bypassSize   = 0;

void GenericBypass(bool enable, DWORD addr )
{
    Scanner s;
    RegionInfo ri = s.GetRegion(addr);

    if( ri.IsReadable )
    {
        DWORD trash = 0;

        // Make copy of region
        bypassSize   = ri.Size;
        bypassRegion = new unsigned char[bypassSize];
        
        // Make the memory copy capable of execution
        if( VirtualProtect(bypassRegion, bypassSize, PAGE_EXECUTE_READWRITE, &trash) )
        {

        }
    }
}

// enables / disables the send hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool GenericHookFacade::HookSend( bool enable )
{
    bool result = true;

    if( enable )
    {
//         if( drPtr == NULL )
//             drPtr = new Breakpoint;
//         drPtr->SetHardwareBreakpoint( SendFunction, (DWORD)GetReturnHookCave, BREAK_ON_BYTE, BREAK_ON_EXECUTION, 1 );
        GenericBypass(true, 0x00401000);
    }
    else // disable
    {

    }

    if( enable )
    {
        PopulateAddresses();
        if( usingHardwareBreakpoints )
        {
            if( drPtr == NULL )
                drPtr = new Breakpoint;
            drPtr->SetHardwareBreakpoint( SendFunction, (DWORD)PacketSendHookCave, BREAK_ON_BYTE, BREAK_ON_EXECUTION, 1 );
        }
        else
        {
        }
    }
    else // disable
    {
        if( usingHardwareBreakpoints )
        {
            if( drPtr != NULL )
            {
                drPtr->ClearHardwareBreakpoint(1);
            }
        }
        else
        {
        }
    }
    return result;
}

void GenericHookFacade::PacketRecv( const char *cstrPacket, SOCKET s/*=0 */ )
{
    PopulateAddresses();
    packets::Packet<packets::PacketInterfaceWindSlayer2> pkt(cstrPacket, packets::PACKET_STREAM_RECV);
    pkt.SendPacket();
}

void GenericHookFacade::PacketSend( const char *cstrPacket, SOCKET s/*=0 */ )
{
    PopulateAddresses();
    packets::Packet<packets::PacketInterfaceWindSlayer2> pkt(cstrPacket);
    pkt.SendPacket();
}
