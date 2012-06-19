#include "../defs.h"

#include "packet.h"
#include "wsasendtofacade.h"
#include "../hexconverter.hpp"
#include "../hookhop.h"
#include "../memreadwrite.h"

#include "../packetitem.h"
#include "../ripecontainer.h"

// an anonymous namespace
namespace
{
    class PacketItemWSASendTo : public PacketItem
    {
    public:
        PacketItemWSASendTo( 
            GeneralPacketStruct   *pkt,
            const PacketFlag pktType,
            const DWORD      retAddr ) : PacketItem( pkt, pktType, retAddr )
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
                packetSize = *(dataPacket->lengthWordPtr);
            return packetSize;
        }
    };

    struct SendStructure //Send packet from WSASend
    {
        DWORD            returnAddress;
        SOCKET           s;
        LPWSABUF         lpBuffers;
        DWORD            dwBufferCount;
        LPDWORD          lpNumberOfBytesSent;
        DWORD            dwFlags;
        struct sockaddr *lpTo;
        int              iToLen;
        LPWSAOVERLAPPED  lpOverlapped;
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
    };

    struct RecvStructure //Receive packet from WSARecv
    {
        DWORD            returnAddress;
        SOCKET           s;
        LPWSABUF         lpBuffers;
        DWORD            dwBufferCount;
        LPDWORD          lpNumberOfBytesRecvd;
        LPDWORD          lpFlags;
        struct sockaddr *lpFrom;
        LPWSAOVERLAPPED  lpOverlapped;
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
    };

    bool __stdcall RecvPacketEvent( RecvStructure *esp );

    bool __stdcall RecvPacketEventTry( RecvStructure *esp );

    bool __stdcall SendPacketEvent( SendStructure *esp );

    bool __stdcall SendPacketEventTry( SendStructure *esp );

    // addresses for WSASend/WSARecv functions in the WS2_32 dll
    DWORD SendFunction     = (DWORD)GetProcAddress( LoadLibraryW(L"WS2_32.dll" ), "WSASendTo" );
    DWORD SendFunction_Ret = SendFunction + 0x05;
    DWORD RecvFunction     = (DWORD)GetProcAddress( LoadLibraryW(L"WS2_32.dll" ), "WSARecvFrom" );
    DWORD RecvFunction_Ret = RecvFunction + 0x05;

    SOCKET s     = INVALID_SOCKET; // socket used in sending packets -- obtained by logging a packet
    int    flags = 0;

    void __declspec(naked) PacketRecvHookCave()
    {
    }

    void __declspec(naked) PacketSendHookCave()
    {
        __asm
        {
            __asm
            {
                push esp
                call SendPacketEventTry               //Called on outgoing packets to the server
                test eax,eax                          // see if packet is blocked

                jnz  SkipSendPacket                   //If true, then block the packet
                //Original instructions
                mov  edi,edi
                push ebp
                mov  ebp,esp
                jmp  dword ptr [SendFunction_Ret]
SkipSendPacket:
                ret  0x24                             //Returns, effectively blocking the packet
            }
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

        for( unsigned int i=0; i < esp->dwBufferCount; ++i )
        {
            //generalPacket is cleaned up by PacketItemSend's destructor
            GeneralPacketStruct *generalPacket = new GeneralPacketStruct;
            generalPacket->headerShortPtr      = (short *)(esp->lpBuffers[i].buf);
            generalPacket->data                = esp->lpBuffers[i].buf+2;
            generalPacket->lengthPtr           = (DWORD *)&(esp->lpBuffers[i].len);
            generalPacket->s                   = esp->s;

            s = esp->s; // assign the current socket to whatever socket was logged (who knows?  It might work!)

            PacketItemWSASendTo *pktItem= new PacketItemWSASendTo( 
                generalPacket,
                PACKET_STREAM_SEND,
                esp->returnAddress );

            pktItem->DispatchPacket();
            result |= pktItem->IsBlocked();
            delete pktItem; //The destructor of msPacketItem takes care of generalPacket
        }

        return result;
    }
}

namespace packets
{
    class PacketInterfaceWSASendTo : public PacketInterface
    {
    private:
        //Sends a packet to the server
        void SendPacketInternal( SOCKET s )
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
        void ProcessPacketInternal( SOCKET s )
        {
            DWORD  pktSz     = *(dataPacket.lengthPtr);
            char  *pktBuf    = new char[pktSz+1];

            for( DWORD i=0; i < pktSz && i < 2; ++i )
                pktBuf[i] = dataPacket.headerCharPtr[i];
            for( DWORD i=2; i < pktSz; ++i )
                pktBuf[i] = dataPacket.data[i-2];

            if( s != 0 )
                //send( s, pktBuf, pktSz, flags );
                HookHop::FunctionHop( recv, s, pktBuf, pktSz, flags );

            delete [] pktBuf;
        }

    public:
        PacketInterfaceWSASendTo() : PacketInterface(){}

        PacketInterfaceWSASendTo(
            __in const PacketInterfaceWSASendTo &other ) :
        PacketInterface( other ){}

        PacketInterfaceWSASendTo( 
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
                    packetSize = (*(dataPacket.lengthWordPtr));
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

        PacketInterfaceWSASendTo& operator =(PacketInterfaceWSASendTo &other)
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
bool WSASendToHookFacade::HookRecv( bool enable )
{
    bool result = true;
    return result;
}

// enables / disables the send hook.
// true for enable, false for disable.
// returns true if succeeded, else false
bool WSASendToHookFacade::HookSend( bool enable )
{
    bool result = true;

    if( enable )
    {
        if( usingHardwareBreakpoints )
        {
            RiPE::Container::GetBreakpoint()->SetHardwareBreakpoint( SendFunction, (int)PacketSendHookCave, BREAK_ON_BYTE, BREAK_ON_EXECUTION, 1 );
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
            RiPE::Container::GetBreakpoint()->ClearHardwareBreakpoint(1);
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

void WSASendToHookFacade::PacketRecv( const char *cstrPacket, SOCKET s/*=0 */ )
{
    packets::Packet<packets::PacketInterfaceWSASendTo> pkt(cstrPacket, packets::PACKET_STREAM_RECV);
    pkt.SendPacket(s);
}

void WSASendToHookFacade::PacketSend( const char *cstrPacket, SOCKET s/*=0 */ )
{
    packets::Packet<packets::PacketInterfaceWSASendTo> pkt(cstrPacket);
    pkt.SendPacket(s);
}
