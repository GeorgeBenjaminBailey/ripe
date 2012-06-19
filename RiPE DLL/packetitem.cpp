#include "packetitem.h"

#include "ripecontainer.h"
#include "ripe_dll.h"

PacketItem::PacketItem( GeneralPacketStruct *pkt,
                        const PacketFlag     pktType,
                        const DWORD          retAddr,
                        const char          *displayStr )
{
    dataPacket        = pkt;
    packetType        = pktType;
    returnAddress     = retAddr;
    if( displayStr != NULL )
    {
        displayString = new char[strlen(displayStr)];
        strcpy(displayString, displayStr);
    }
    else
        displayString = NULL;
    byteStringNoSpace = NULL;
    blocked           = false;
    blockStatusType   = STATUS_NOT_BLOCKED;
    packetSize        = 0;
}

PacketItem::~PacketItem()
{
    //We don't delete the data that dataPacket points to, because otherwise
    //the client won't have data to send to the server
    if( dataPacket != NULL )
        delete dataPacket;
    dataPacket = NULL;
    if( displayString != NULL )
        delete [] displayString; //Delete byteString if it has been assigned
    if( byteStringNoSpace != NULL )
        delete [] byteStringNoSpace; //Delete byteStringNoSpace if it has been assigned
}

bool PacketItem::SetReturn( __in DWORD addr )
{
    for( unsigned int i=0; i < usedAddresses.size(); ++i )
        if( usedAddresses[i] == addr )
            return false;
    returnAddress = addr;
    usedAddresses.push_back(returnAddress);
    return true;
}

void PacketItem::Block()
{
    blocked = true;
}

bool PacketItem::IsBlocked()
{
    return blocked;
}

bool PacketItem::GetStringData(bool addSpace, char *outbuf)
{
    bool retVal = true;
    __try
    {
        unsigned int sizeOfHeader = 2; //size of header in bytes
        for( unsigned int i=0; i < GetSize(); ++i )
        {
            char lsb,msb;

            if( i < sizeOfHeader )
            {
                lsb = (dataPacket->headerCharPtr)[i];
            }
            else
            {
                lsb = (dataPacket->data)[i - sizeOfHeader];
            }
            msb = lsb;
            msb = msb >> 4;
            msb &= 0x0F; //Clear top 4 bits because we moved the important bits to the low bits
            lsb &= 0x0F; //Clear top 4 bits

            msb += (msb > 9) ? 0x37 : 0x30;
            lsb += (lsb > 9) ? 0x37 : 0x30;

            if(addSpace)
            {
                outbuf[3*i]   = msb;
                outbuf[3*i+1] = lsb;
                outbuf[3*i+2] = ' ';
            }
            else
            {
                outbuf[2*i]   = msb;
                outbuf[2*i+1] = lsb;
            }
        }
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        retVal = false;
    }
    return retVal;
}

//Function used in blocking receive packets (has to be done differently because of the way receive works)
void PacketItem::BlockRecv()
{
    //Make sure packet isn't blocked or ignored
    blockStatusType = STATUS_NOT_BLOCKED; //0 if not ignored or blocked. 1 if ignored. 2 if blocked.  Block has precedence over ignore.
    std::string packetString = byteStringNoSpace; //Get the string representation of the packet to be checked

    //For all items in block/ignore list
    CheckIfBlocked( packetString);
}

void PacketItem::CheckIfBlocked( std::string& packetString)
{
    blockStatusType = RiPE::Container::GetHackDLL()->GetBlockStatus(
            packetType, packetString, dataPacket, GetSize() );

    //A switch would be nice, but I have to break, so it won't work too well
    if( blockStatusType == STATUS_BLOCKED ) {
        Block(); //Block the packet
    } else if( blockStatusType == STATUS_MODIFIED ) {
        recvSize = GetSize();
    }
}

// TODO: Clean this shit up
void PacketItem::DispatchPacket()
{
    //Make sure packet isn't blocked or ignored
    char *tmpstrbuf          = new char[(GetSize()*2)+1]; //Temporary buffer
    if( GetStringData( false, tmpstrbuf ) )
    {
        tmpstrbuf[GetSize()*2]   = 0; //NULL-terminate
        std::string packetString    = tmpstrbuf; //Get the string representation of the packet to be checked
        std::string bigEndianHeader;

        if( packetString.size() > 3 )
            bigEndianHeader = packetString.substr(2,2);
        else
            bigEndianHeader = "00";
        bigEndianHeader         += packetString.substr(0,2);

        if (packetType != PACKET_STREAM_RECV) {
            CheckIfBlocked(packetString);
        }

        // send event to script manager
        if (blockStatusType != STATUS_BLOCKED) {
            std::vector<unsigned char> r;
            unsigned int sizeOfHeader = 2;
            for( unsigned int i=0; i < GetSize(); ++i )
            {
                if( i < sizeOfHeader )
                    r.push_back((dataPacket->headerCharPtr)[i]);
                else
                    r.push_back((dataPacket->data)[i - sizeOfHeader]);
            }

            if (packetType == PACKET_STREAM_SEND) {
                RiPE::Container::GetHackDLL()->ProcessSendEvent(r);
            } else if (packetType == PACKET_STREAM_RECV) {
                RiPE::Container::GetHackDLL()->ProcessRecvEvent(r);
            }

            // After processing, modify the dataPacket as needed
            for( size_t i=0; i < r.size() && i < GetSize(); ++i )
            {
                if( i < sizeOfHeader )
                    dataPacket->headerCharPtr[i] = r[i];
                else
                    dataPacket->data[i - sizeOfHeader] = r[i];
            }
        }

        if( blockStatusType != STATUS_IGNORED && blockStatusType != STATUS_BLOCKED ) //If the send packet is not blocked or ignored
        {
            char *tmpdatabuf = new char[(GetSize()*3)+1]; //temp C-String for packet data
            GetStringData(true, tmpdatabuf);
            tmpdatabuf[GetSize()*3] = 0; //NULL-terminate the string

            RiPE::LogPacket packet;
            char buf[20];
            sprintf(buf, "%08X", returnAddress);
            packet.returnAddress = buf;
            sprintf(buf, "%i", GetSize());
            packet.size          = buf;
            packet.opcode        = bigEndianHeader;
            packet.packet        = displayString;
            packet.packetData    = tmpdatabuf;
            packet.socket        = dataPacket->s;
            if (packetType == PACKET_STREAM_SEND) {
                RiPE::Container::GetHackDLL()->LogSend(packet);
            } else if (packetType == PACKET_STREAM_RECV) {
                RiPE::Container::GetHackDLL()->LogRecv(packet);
            }
        }
    }

    delete [] tmpstrbuf; //No longer needed, so remove it
    return;
}