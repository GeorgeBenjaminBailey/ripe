#ifndef __PACKET_ITEM_H
#define __PACKET_ITEM_H

#include "defs.h"

#include <string>
#include <vector>

#include "memreadwrite.h"

enum PacketFlag
{
    PACKET_STREAM_RECV = 0x00,
    PACKET_STREAM_SEND = 0x01,
    PACKET_NULL        = 0x02,
    PACKET_ZERO        = 0x04
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
    SOCKET s;
    std::string ip; // IP address

    GeneralPacketStruct()
    {
        headerCharPtr = NULL;
        data          = NULL;
        lengthPtr     = NULL;
        s             = 0;
        ip            = "";
    }
};

class PacketItem
{
protected:
    GeneralPacketStruct *dataPacket;
    PacketFlag           packetType;
    DWORD                returnAddress;
    bool                 blocked;
    char                *displayString;        //Keep track of receive data because of way that the address for receive works
    char                *byteStringNoSpace;
    BlockStatus          blockStatusType;   //Used for receive packets to see if they're blocked or ignored
    DWORD                recvSize;          //Keeps track of receive packet size
    std::vector<DWORD>   usedAddresses;     //Keeps track of already used recv addresses and prevents unnecessary repetition

    DWORD              packetSize;        //An optional value that can be assigned by child classes

    //Used inside of block
    void CheckIfBlocked( std::string& packetString );

public:
    PacketItem( GeneralPacketStruct   *pkt,
                const PacketFlag pktType,
                const DWORD      retAddr,
                const char      *displayStr = NULL );

    ~PacketItem();

    DWORD GetSize()
    {
        return packetSize == 0 ? ReadAddress(dataPacket->lengthPtr) : packetSize;
    }

    DWORD GetReturn()
    {
        return returnAddress;
    }

    bool SetReturn( __in DWORD addr );

    //Blocks the packet from being processed
    void Block();
    bool IsBlocked();

    //Return string representation of packet data
    //Requires a char* buffer of ((dataPacket->size)*2)+1 for no space
    //Requires a char* buffer of ((dataPacket->size)*3)+1 for space
    //This function does NOT add a NULL-terminator
    bool GetStringData( bool addSpace, char *outbuf );

    //Function used in blocking receive packets (has to be done differently because of the way receive works)
    virtual void BlockRecv();

    void DispatchPacket();
};

#endif // __PACKET_ITEM_H
