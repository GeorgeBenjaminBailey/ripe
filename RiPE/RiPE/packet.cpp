#include "stdafx.h"
#include "packet.h"

//For the append functions, ecx = pointer to packet, arg1 = value to append
//GetBattlenetAllocator+0x179c90 = AppendByteToPacket
//GetBattlenetAllocator+0x179d50 = AppendDwordToPacket

packets::PacketInterface::PacketInterface()
{
    dataPacket      = GeneralPacketStruct();
    packetType      = PACKET_NULL;
    returnAddress   = 0;
    blocked         = false;
    blockStatusType = STATUS_NOT_BLOCKED;
    packetSize      = -1;
    recvSize        = 0;
}

packets::PacketInterface::PacketInterface(
    __in const packets::PacketInterface &other )
{
    dataPacket        = other.dataPacket;
    packetType        = other.packetType;
    returnAddress     = other.returnAddress;
    blocked           = other.blocked;
    blockStatusType   = other.blockStatusType;
    packetSize        = other.packetSize;
    recvSize          = other.recvSize;
}

packets::PacketInterface::PacketInterface(
    __in GeneralPacketStruct  pkt,
    __in const PacketFlag     pktType,
    __in const DWORD          retAddr )
{
    dataPacket        = pkt;
    packetType        = pktType;
    returnAddress     = retAddr;
    blocked           = false;
    blockStatusType   = STATUS_NOT_BLOCKED;
    packetSize        = -1;
}

packets::PacketInterface::~PacketInterface()
{
    //We don't delete the data that dataPacket points to
}

bool packets::PacketInterface::SetReturn( 
    __in DWORD addr )
{
    returnAddress = addr;
    return true;
}

void packets::PacketInterface::Block()
{
    blocked = true;
    return;
}

bool packets::PacketInterface::IsBlocked()
{
    return blocked;
}

std::string packets::PacketInterface::GetPacketString(
    __in_opt const bool addSpace )
{
    std::string result;
    
    unsigned int sizeOfHeader = 2; //size of header in bytes

    for( unsigned int i=0; i < GetSize(); ++i )
    {
        char lsb,msb;
        if( i < sizeOfHeader )
        {
            if( dataPacket.headerCharPtr != NULL )
                lsb = (dataPacket.headerCharPtr)[i];
        }
        else if( dataPacket.data != NULL )
            lsb = (dataPacket.data)[i - sizeOfHeader];
        msb = lsb;
        msb = msb >> 4;
        msb &= 0x0F; //Clear top 4 bits because we moved the important bits to the low bits
        lsb &= 0x0F; //Clear top 4 bits

        msb += (msb > 9) ? 0x37 : 0x30;
        lsb += (lsb > 9) ? 0x37 : 0x30;

        if(addSpace)
        {
            result += msb;
            result += lsb;
            result += ' ';
        }
        else
        {
            result += msb;
            result += lsb;
        }
    }
    return result;
}

bool packets::PacketInterface::GetPacketString(
    __out    char       *outbuf,
    __in_opt const bool  addSpace )
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
                if( dataPacket.headerCharPtr != NULL )
                    lsb = (dataPacket.headerCharPtr)[i];
            }
            else if( dataPacket.data != NULL )
                lsb = (dataPacket.data)[i - sizeOfHeader];
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

void packets::PacketInterface::SendPacket()
{
    if( packetType == PACKET_STREAM_RECV )
        ProcessPacketInternal();
    else
        SendPacketInternal();
}

void packets::PacketInterface::SendPacket( SOCKET s )
{
    if( packetType == PACKET_STREAM_RECV )
        ProcessPacketInternal(s);
    else
        SendPacketInternal(s);
}

packets::ErrorMessage packets::PacketInterface::SetPacketData( 
    __in std::string data )
{
    ErrorMessage result = __NO_ERROR;
    std::string  packetBuffer;
    result = ParsePacket( packetBuffer, data );

    if( result == __NO_ERROR ) //Replace the data
    {
        atohx( dataPacket.headerCharPtr, packetBuffer.c_str(), 4 ); //Copy header
        atohx( dataPacket.data, packetBuffer.c_str()+4 );           //Copy data
    }

    return result;
}

packets::ErrorMessage packets::PacketInterface::ParsePacket(
    __out std::string &dst, 
    __in  std::string  src)
{
    ErrorMessage result = __NO_ERROR;
    //Remove whitespace and all other invalid symbols
    
    for( unsigned int i=0; i < src.size(); ++i )
    {
        if( (src[i] >= '0' && src[i] <= '9') || 
            (src[i] >= 'A' && src[i] <= 'Z') ||
            (src[i] >= 'a' && src[i] <= 'z') || 
            (src[i] == 'X') || 
            (src[i] == 'x') ||
            (src[i] == '*' ) )
        {
            // These operations should not go to strlen(strPacketBuf.c_str()), or else undefined behavior may occur.
            // This is because we check i and i+1
            if( i < (src.size()-1) )
            {
                // Small x is replaced with random lower-case character
                if( src[i] == 'x' && src[i+1] == 'x' )
                {
                    int iRand = rand()%26;
                    //iRand < 15 ? strPacketBuf[i] = '4' : strPacketBuf[i] = '5';
                    //strPacketBuf[i+1] = 'A'+iRand;
                    if( iRand < 15 )
                    {
                        src[i] = '6';
                        (iRand) < 9 ? src[i+1] = '0'+iRand : src[i+1] = 'a'+(iRand-9);
                    }
                    else
                    {
                        iRand -= 15;
                        src[i] = '7';
                        (iRand) < 10 ? src[i+1] = '0'+iRand : src[i+1] = 'a'+(iRand-10);
                    }
                }

                // Capital X is replaced with random upper-case character
                else if( src[i] == 'X' && src[i+1] == 'X' )
                {
                    int iRand = rand()%26;
                    //iRand < 15 ? strPacketBuf[i] = '4' : strPacketBuf[i] = '5';
                    //strPacketBuf[i+1] = 'A'+iRand;
                    if(iRand < 15)
                    {
                        src[i] = '4';
                        (iRand) < 9 ? src[i+1] = '0'+iRand : src[i+1] = 'A'+(iRand-9);
                    }
                    else
                    {
                        iRand -= 15;
                        src[i] = '5';
                        (iRand) < 10 ? src[i+1] = '0'+iRand : src[i+1] = 'A'+(iRand-10);
                    }
                }
            }

            // These operations go to the full length of strPacketBuf.c_str()
            // Asterisks should be replaced with random hex characters
            if( src[i] == '*' || src[i] == '?' )
            {
                int iRand = rand()%16;
                (iRand) < 10 ? iRand+=0x30 : iRand+= 0x37;
                src[i] = (char)(iRand);
            }
            dst += src[i];
        }
    }

    //Check for potential errors
    if( dst.size() < 4 ) //If length is less than a header
        result = __INVALID_LENGTH;
    else if( dst.size()%2 == 1 ) //If length is invalid
        result = __NOT_EVEN;
    else if( dst.size() > GetSize()*2 ) //Replacement packet exceeded length
        result = __TOO_LONG;

    return result;
}

inline char * packets::atohx(
    __out    char       *dst, 
    __in     const char *src, 
    __in_opt int         maxNibblesToCopy)
{
    char *ret = dst;
    for( int lsb, msb, i=0; (*src != '\0') && (i < maxNibblesToCopy/2); src += 2, ++i )
    {
        if( *src != '?' ) // check if mask is present.  If so, skip
        {
            msb = tolower(*src);
            lsb = tolower(*(src + 1));
            msb -= isdigit(msb) ? 0x30 : 0x57;
            lsb -= isdigit(lsb) ? 0x30 : 0x57;
            if( (msb < 0x0 || msb > 0xf) || (lsb < 0x0 || lsb > 0xf) )
            {
                *ret = 0;
                return NULL;
            }
            *dst = (char)(lsb | (msb << 4));
        }
        ++dst;
    }
    *dst = 0;
    return ret;
}
