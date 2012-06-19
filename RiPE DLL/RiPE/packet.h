#ifndef __PACKET_H
#define __PACKET_H
//#include "windows.h"
#include <algorithm>
#include <queue>
#include <string>

#include "../defs.h"

namespace packets
{
    enum ErrorMessage
    {
        //Packet errors
        __NO_ERROR = 0,
        __INVALID_LENGTH,
        __NOT_EVEN,
        __DC_HACK,
        __TOO_LONG,

        //Hack errors
        __HACK_NOT_ENABLED
    };

    enum PacketFlag
    {
        PACKET_STREAM_RECV = 0x01,
        PACKET_STREAM_SEND = 0x02,
        PACKET_NULL        = 0x04,
        PACKET_ZERO        = 0x08
    };

    enum BlockStatus
    {
        STATUS_NOT_BLOCKED = 0,
        STATUS_IGNORED,
        STATUS_BLOCKED,
        STATUS_MODIFIED,
        STATUS_INTERCEPTED
    };

    //General-purpose packet structure that should work for most, if not all, games
    struct GeneralPacketStruct
    {
        union
        {
            char  *headerCharPtr;
            short *headerShortPtr;
        };
        char  *data;
        union
        {
            DWORD *lengthPtr;  //By default this should be the length of the header + data
            //If, for whatever reason, it's something else, then you will
            //need to change the GetSize() function accordingly
            WORD  *lengthWordPtr;
        };

        GeneralPacketStruct()
        {
            headerCharPtr = NULL;
            data          = NULL;
            lengthPtr     = NULL;
        }
    };

    //Note:  Due to the nature of how a packet needs to function in most
    //       games, this class does NOT provide any sort of cleanup nor
    //       creation of packet data.  If you need this added cleanup,
    //       then you should use the Packet class instead. 
    class PacketInterface
    {
    protected:
        GeneralPacketStruct  dataPacket;
        PacketFlag           packetType;
        DWORD                returnAddress;
        bool                 blocked;
        BlockStatus          blockStatusType;   //Used for receive packets to see if they're blocked or ignored
        DWORD                recvSize;          //Keeps track of receive packet size
        DWORD                packetSize;        //An optional value that can be assigned by child classes

        //Sends a packet to the server
        void SendPacketInternal(){;}
        void SendPacketInternal( SOCKET s ){;}

        //Processes a packet "from" the server
        void ProcessPacketInternal(){;}
        void ProcessPacketInternal( SOCKET s ){;}

    public:

        PacketInterface();        

        PacketInterface(
            __in const PacketInterface &other );

        //pkt should be a pointer to an already-created GeneralPacketStruct
        //with valid data.
        PacketInterface( 
            __in GeneralPacketStruct  pkt, 
            __in const PacketFlag     pktType, 
            __in const DWORD          retAddr );

        ~PacketInterface();

        //Blocks the packet from being processed
        void Block();

        inline GeneralPacketStruct& GetDataPacket()
        {
            return dataPacket;
        }

        //Return string representation of packet data
        std::string GetPacketString(
            __in_opt const bool addSpace = false );

        //Requires a char * buffer of (GetSize()*2)+1 for no space
        //Requires a char * buffer of (GetSize()*3)+1 for space
        //This function does NOT add a NULL-terminator
        bool GetPacketString( 
            __out    char       *outbuf,
            __in_opt const bool  addSpace = false );

        inline DWORD GetReturn()
        {
            return returnAddress;
        }

        DWORD GetSize()
        {
            DWORD result = 0;
            if( dataPacket.lengthPtr != NULL )
                result = packetSize == -1 ? *(dataPacket.lengthPtr) : packetSize;
            return result;
        }

        inline PacketFlag GetType()
        { 
            return packetType; 
        }

        //Returns true if the packet is blocked, else false
        bool IsBlocked();

        //Create a "real" packet (no whitespace or invalid characters)
        ErrorMessage ParsePacket(
            __out std::string &dst, 
            __in  std::string  src);

        //Send/Process packet based on type
        void SendPacket();
        void SendPacket(SOCKET s);

        //Send/Process packet based on type
        void SendPacket( 
            __in GeneralPacketStruct& generalPacket, 
            __in const                PacketFlag pType);

        inline void SetDataPacket(
            __in GeneralPacketStruct src )
        {
            dataPacket = src;
        }

        //Replaces the packet's data (header, body, and size) by the parameters
        //
        //Warning:  If used on a packet that has been logged by the game itself,
        //          you might experience problems with memory allocation.  If
        //          this is the case, then you should use the alternative
        //          ModifyPacketData() member.
        ErrorMessage SetPacketData( 
            __in std::string data );

        inline void SetType(
            __in PacketFlag typ )
        { 
            packetType = typ; 
        }

        bool SetReturn( 
            __in DWORD addr );

        PacketInterface& operator =(PacketInterface &other)
        {
            dataPacket        = other.dataPacket;
            packetType        = other.packetType;
            returnAddress     = other.returnAddress;
            blocked           = other.blocked;
            blockStatusType   = other.blockStatusType;
            packetSize        = other.packetSize;
            return *this;
        }
    };

    //NOTE:  You should only use templates of classes derived from PacketInterface
    template< class T>
    class Packet
    {
    private:
        T pktInterface;

    public:

        Packet()
        {
            ZeroMemory( &pkt, sizeof(GeneralPacketStruct) );
        }

        //Create packet from string
        Packet( 
            const char      *src,
            const PacketFlag pktType    = PACKET_STREAM_SEND,
            const DWORD      retAddr    = 0,
            const bool       modifySize = false)
        {
            SetPacketData( src );
            pktInterface = T( pktInterface.GetDataPacket(), pktType, retAddr, modifySize );
        }

        ~Packet()
        {
            if( pktInterface.GetType() != PACKET_NULL )
            {
                if( pktInterface.GetDataPacket().lengthPtr     != NULL )
                    delete pktInterface.GetDataPacket().lengthPtr;
                if( pktInterface.GetDataPacket().headerCharPtr != NULL )
                    delete [] pktInterface.GetDataPacket().headerCharPtr;
                if( pktInterface.GetDataPacket().data          != NULL )
                    delete [] pktInterface.GetDataPacket().data;
            }
        }

        inline GeneralPacketStruct& GetDataPacket()
        {
            return pktInterface.GetDataPacket();
        }

        inline std::string GetPacketString(
            __in_opt const bool addSpace = false )
        {
            return pktInterface.GetPacketString( addSpace );
        }

        inline DWORD GetReturn()
        {
            return pktInterface.GetReturn();
        }

        inline DWORD GetSize()
        {
            return pktInterface.GetSize();
        }

        inline PacketFlag GetType()
        {
            return pktInterface.GetType();
        }

        //Sets packet data and length.
        ErrorMessage SetPacketData( 
            __in const char *src )
        {
            std::string   buffer;
            ErrorMessage  err    = pktInterface.ParsePacket( buffer, src );
            if( err == __NO_ERROR || err == __TOO_LONG )
            {
                pktInterface.GetDataPacket().lengthPtr     = new DWORD( buffer.size()/2 );
                pktInterface.GetDataPacket().headerCharPtr = new char[4];
                pktInterface.GetDataPacket().data          = new char[ buffer.size()-4 ];
                packets::atohx( pktInterface.GetDataPacket().headerCharPtr, buffer.c_str(), 4 );
                packets::atohx( pktInterface.GetDataPacket().data,          buffer.c_str()+4  );
            }

            return err;
        }

        void SendPacket(){ pktInterface.SendPacket(); }
        void SendPacket(SOCKET s){ pktInterface.SendPacket(s); }
    };

    class PacketHook
    {
    public:
        // hook/unhook to/from current process
        // hook if true, unhook if false
        virtual bool HookSend( bool enable ) {}

        // hook/unhook to/from current process
        // hook if true, unhook if false
        virtual bool HookRecv( bool enable ) {}

        // send a newly created packet to the server
        virtual void SendPacket( char *buf ) {}

        // recv a newly created packet from the "server"
        virtual void RecvPacket( char *buf ) {}
    };

    //This atohx converts an ASCII-representation of hex to its memory representation.
    //You may use '?' characters as a "mask."  Parts of the mask will not be copied.
    //Instead, the memory at that location will remain intact.
    inline char * atohx(
        __out    char       *dst, 
        __in     const char *src, 
        __in_opt int         maxNibblesToCopy = INT_MAX );
}
#endif