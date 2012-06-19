#include "stdafx.h"

#include <wx/msgdlg.h>

#include "packet.h"
#include "sendfacade.h"
#include "RiPEApp.h"
#include "../defs.h"
#include "../hexconverter.hpp"
#include "../memreadwrite.h"
#include "../RiPE.h"
#include "../hookhop.h"


// an anonymous namespace
namespace
{
    class PacketItemSend : public PacketItem
    {
    public:
        PacketItemSend( RiPE *classPtr,
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
            if( packetType == PACKET_STREAM_SEND )
                packetSize = *(dataPacket->lengthPtr);
            else
                packetSize = *(dataPacket->lengthPtr);
            return packetSize;
        }
    };

    struct SendStructure //Send packet from send
    {
        DWORD  returnAddress;
        SOCKET s;
        char  *buf;
        int    len;
        int    flags;
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

    bool __stdcall SendPacketEvent( SendStructure *esp );

    bool __stdcall SendPacketEventTry( SendStructure *esp );

    // addresses for Send/Recv functions in the WS2_32 dll
    DWORD SendFunction     = (DWORD)GetProcAddress( LoadLibraryW(L"WS2_32.dll" ), "send" );
    DWORD SendFunction_Ret = SendFunction + 0x05;
    DWORD RecvFunction     = (DWORD)GetProcAddress( LoadLibraryW(L"WS2_32.dll" ), "recv" );
    DWORD RecvFunction_Ret = RecvFunction + 0x05;

    int    flags = 0;

    int __stdcall PacketRecvHookCave( SOCKET s, char *buf, int len, int flags )
    {
        HookHop::FunctionHop(recv, s, buf, len, flags);
        __asm
        {
            mov  [ebp+0x10],eax      // replace the int parameter with the returned size (although messy, this is the simplest solution)
            push eax                 // preserve eax
            push ebp
            add  dword ptr [esp],4   // offset to esp's initial value
            call RecvPacketEventTry  // called on outgoing packets to the server
            cmp  eax,1               // 1 if blocked
            pop  eax                 // restore eax
            jne  Cleanup
            xor  eax,eax             // 0-out the return size, indicating that recv "failed"
Cleanup:
        }
    }

    void __declspec(naked) PacketSendHookCave()
    {
        __asm
        {
            push esp
            call SendPacketEventTry              // called on outgoing packets to the server
            cmp  eax,1                           // see if packet is blocked

            je  SkipSendPacket                   // if true, then block the packet
            //Original instructions
            mov  edi,edi
            push ebp
            mov  ebp,esp
            jmp  dword ptr [SendFunction_Ret]
SkipSendPacket:
            mov  eax,[esp+0xC]                    // return size for success
            ret  0x10                             // returns, effectively blocking the packet
        }
    }

    bool __stdcall RecvPacketEvent( RecvStructure *esp )
    {
        bool result = false;

        //generalPacket is cleaned up by PacketItemSend's destructor
        GeneralPacketStruct *generalPacket = new GeneralPacketStruct;
        generalPacket->headerShortPtr      = (short *)(esp->buf);//sendPacketPointer->dataPacket->bytes;
        generalPacket->data                = esp->buf+2;
        generalPacket->lengthPtr           = (DWORD *)&(esp->len);
        generalPacket->s                   = esp->s;

        PacketItemSend *pktItem= new PacketItemSend( 
            RiPEApp::GetRiPE(),
            generalPacket,
            PACKET_STREAM_RECV,
            esp->returnAddress );

        pktItem->DispatchPacket();
        result = pktItem->IsBlocked();
        delete pktItem; //The destructor of msPacketItem takes care of generalPacket
        
        return result;
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

    bool __stdcall SendPacketEventTry( SendStructure *esp )
    {
        bool result = false;
        __try
        {
            result = SendPacketEvent( esp );
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            ;
        }
        return result;
    }

    bool __stdcall SendPacketEvent( SendStructure *esp )
    {
        bool result = false;

        //generalPacket is cleaned up by PacketItemSend's destructor
        GeneralPacketStruct *generalPacket = new GeneralPacketStruct;
        generalPacket->headerShortPtr      = (short *)(esp->buf);//sendPacketPointer->dataPacket->bytes;
        generalPacket->data                = esp->buf+2;
        generalPacket->lengthPtr           = (DWORD *)&(esp->len);
        generalPacket->s                   = esp->s;

        PacketItemSend *pktItem= new PacketItemSend( 
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
    class PacketInterfaceSend : public PacketInterface
    {
    private:
        //Sends a packet to the server
        void SendPacketInternal( SOCKET s=0 )
        {
            DWORD  pktSz     = *(dataPacket.lengthPtr);
            char  *pktBuf    = new char[pktSz+1];

            for( DWORD i=0; i < pktSz && i < 2; ++i )
                pktBuf[i] = dataPacket.headerCharPtr[i];
            for( DWORD i=2; i < pktSz; ++i )
                pktBuf[i] = dataPacket.data[i-2];

            if( s != 0 )
                //send( s, pktBuf, pktSz, flags );
                HookHop::FunctionHop( send, s, pktBuf, pktSz, flags );

            delete [] pktBuf;
        }

        void SendPacketInternal(
            __in GeneralPacketStruct pkt );

        //Processes a packet "from" the server
        void ProcessPacketInternal( SOCKET s=0 )
        {
            DWORD  pktSz     = *(dataPacket.lengthPtr);
            char  *pktBuf    = new char[pktSz+1];

            for( DWORD i=0; i < pktSz && i < 2; ++i )
                pktBuf[i] = dataPacket.headerCharPtr[i];
            for( DWORD i=2; i < pktSz; ++i )
                pktBuf[i] = dataPacket.data[i-2];

            if( s != 0 )
                HookHop::FunctionHop(recv, s, pktBuf, pktSz, flags );

            delete [] pktBuf;
        }

    public:
        PacketInterfaceSend() : PacketInterface(){}

        PacketInterfaceSend(
            __in const PacketInterfaceSend &other ) :
        PacketInterface( other ){}

        PacketInterfaceSend( 
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
            {
                if( packetType == PACKET_STREAM_SEND )
                    packetSize = (*(dataPacket.lengthPtr));
                else
                    packetSize = (*(dataPacket.lengthPtr));
            }
            return packetSize;
        }

        //Send/Process packet based on type
        void SendPacket( SOCKET s=0 )
        {
            if( packetType == PACKET_STREAM_RECV )
                ProcessPacketInternal(s);
            else if( packetType == PACKET_STREAM_SEND )
                SendPacketInternal(s);
        }

        PacketInterfaceSend& operator =(PacketInterfaceSend &other)
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
bool SendHookFacade::HookRecv( bool enable )
{
    bool result = true;

    if( enable )
    {
        if( usingHardwareBreakpoints )
        {
            if( drPtr == NULL )
                drPtr = new Breakpoint;
            drPtr->SetHardwareBreakpoint( RecvFunction, (int)PacketRecvHookCave, BREAK_ON_BYTE, BREAK_ON_EXECUTION, 2 );
        }
        else
        {
            BYTE packetRecvHookArr[] = { SHORT_JUMP, 0, 0, 0, 0 };
            *(DWORD *)(packetRecvHookArr+1) = JMP(RecvFunction, PacketRecvHookCave);
            result &= WriteAddress( (LPVOID)RecvFunction, packetRecvHookArr, sizeof( packetRecvHookArr ) );
            result = false;
        }
    }
    else // disable
    {
        if( usingHardwareBreakpoints )
        {
            if( drPtr != NULL )
            {
                drPtr->ClearHardwareBreakpoint(2);
            }
        }
        else
        {
            const BYTE packetRecvHookOffArr[]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };
            WriteAddress( (LPVOID)RecvFunction, packetRecvHookOffArr, sizeof( packetRecvHookOffArr ) );
            result = false;
        }
    }
    return result;
}

// enables / disables the send hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool SendHookFacade::HookSend( bool enable )
{
    bool result = true;

    if( enable )
    {
        if( usingHardwareBreakpoints )
        {
            if( drPtr == NULL )
                drPtr = new Breakpoint;
            drPtr->SetHardwareBreakpoint( SendFunction, (int)PacketSendHookCave, BREAK_ON_BYTE, BREAK_ON_EXECUTION, 1 );
        }
        else
        {
            BYTE packetSendHookArr[] = { SHORT_JUMP, 0, 0, 0, 0 };
            *(DWORD *)(packetSendHookArr+1) = JMP(SendFunction, PacketSendHookCave);
            result &= WriteAddress( (LPVOID)SendFunction, packetSendHookArr, sizeof( packetSendHookArr ) );
            result = false;
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
            const BYTE packetSendHookOffArr[]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };
            WriteAddress( (LPVOID)SendFunction, packetSendHookOffArr, sizeof( packetSendHookOffArr ) );
            result = false;
        }
    }
    return result;
}

void SendHookFacade::PacketRecv( const char *cstrPacket, SOCKET s/*=0 */ )
{
    packets::Packet<packets::PacketInterfaceSend> pkt(cstrPacket, packets::PACKET_STREAM_RECV);
    pkt.SendPacket(s);
}

void SendHookFacade::PacketSend( const char *cstrPacket, SOCKET s/*=0 */ )
{
    packets::Packet<packets::PacketInterfaceSend> pkt(cstrPacket);
    pkt.SendPacket(s);
}
