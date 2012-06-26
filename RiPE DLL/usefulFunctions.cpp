#include "defs.h"

#include <algorithm>

#include "memreadwrite.h"
#include "usefulFunctions.h"

//srand( (unsigned int)(time(NULL)) ); //Seed it with time

void parsePacket(char* cstrPacketBuffer, std::string strPacketBuf)
{
    //Remove spaces and all other odd symbols
    int counter=0; //Keeps track of current length of cstrPacketBuffer
    for(int i=0; i < (int)strlen(strPacketBuf.c_str()); ++i)
        if( (strPacketBuf[i] >= '0' && strPacketBuf[i] <= '9') || (strPacketBuf[i] >= 'A' && strPacketBuf[i] <= 'Z') ||
            (strPacketBuf[i] >= 'a' && strPacketBuf[i] <= 'z') || (strPacketBuf[i] == 'X') || (strPacketBuf[i] == 'x') ||
            (strPacketBuf[i] == '*' ) )
        {
            /*//Formatting (converts decimals to hexadecimal) WIP -- do NOT uncomment before finishing this
            if(strPacketBuf[i] == '#')
            {
                int length=0;
                while(strPacketBuf[i+1] >= '0' && strPacketBuf[i+1] <= '9')
                {
                    ++length;
                    ++i;
                }
                if(length > 3)
                    wxMessageBox(std::string("Error processing decimal representation.\nMake sure that you include a space after the decimal number\nand that the value is not over 255."), std::string("Error Processing Decimal"));
            }*/

            /* Add strings */

            /* These operations should not go to strlen(strPacketBuf.c_str()), or else undefined behavior may occur */
            if(i < (int)(strlen(strPacketBuf.c_str())-1))
            {
                /* Small x is replaced with random lower-case character */
                if(strPacketBuf[i] == 'x' && strPacketBuf[i+1] == 'x')
                {
                    int iRand = rand()%26;
                    //iRand < 15 ? strPacketBuf[i] = '4' : strPacketBuf[i] = '5';
                    //strPacketBuf[i+1] = 'A'+iRand;
                    if(iRand < 15)
                    {
                        strPacketBuf[i] = '6';
                        (iRand) < 9 ? strPacketBuf[i+1] = '0'+iRand : strPacketBuf[i+1] = 'a'+(iRand-9);
                    }
                    else
                    {
                        iRand          -= 15;
                        strPacketBuf[i] = '7';
                        (iRand) < 10 ? strPacketBuf[i+1] = '0'+iRand : strPacketBuf[i+1] = 'a'+(iRand-10);
                    }
                }

                /* Capital X is replaced with random upper-case character */
                if(strPacketBuf[i] == 'X' && strPacketBuf[i+1] == 'X')
                {
                    int iRand = rand()%26;
                    //iRand < 15 ? strPacketBuf[i] = '4' : strPacketBuf[i] = '5';
                    //strPacketBuf[i+1] = 'A'+iRand;
                    if(iRand < 15)
                    {
                        strPacketBuf[i] = '4';
                        (iRand) < 9 ? strPacketBuf[i+1] = '0'+iRand : strPacketBuf[i+1] = 'A'+(iRand-9);
                    }
                    else
                    {
                        iRand          -= 15;
                        strPacketBuf[i] = '5';
                        (iRand) < 10 ? strPacketBuf[i+1] = '0'+iRand : strPacketBuf[i+1] = 'A'+(iRand-10);
                    }
                }

            }

            /* These operations go to the full length of strPacketBuf.c_str() */
            /* Asterisks should be replaced with random hex characters */
            if(strPacketBuf[i] == '*' || strPacketBuf[i] == '?')
            {
                int iRand       = rand()%16;
                (iRand) < 10 ? iRand+=0x30 : iRand+= 0x37;
                strPacketBuf[i] = (char)(iRand);
            }

            cstrPacketBuffer[counter] = strPacketBuf[i];
            ++counter;
        }
    cstrPacketBuffer[counter] = '\0';
}

//Returns 0 for no error or a positive value for an error code
ErrorMessage SendPacketToProcess(std::string strPacketBuf, HookInterface *hIntf, sendPacketType pktTyp, SOCKET s)
{
    ErrorMessage errorCode = __NO_ERROR;
    char *cstrPacketBuffer = new char[strlen(strPacketBuf.c_str())+1]; //This is the longest we'll need.  Probably too large, but that's okay
    parsePacket(cstrPacketBuffer, strPacketBuf);
    
    if(strlen(cstrPacketBuffer) < 4) //If length is less than a header
        errorCode = __INVALID_LENGTH;
    else if(strlen(cstrPacketBuffer)%2 == 1) //If length is invalid
        errorCode = __NOT_EVEN;
    /*else if( (cstrPacketBuffer[0] == '3' && cstrPacketBuffer[1] == '7') || //DC hack detected
             (cstrPacketBuffer[0] == '8' && cstrPacketBuffer[1] == '9'))
        errorCode = __DC_HACK;*/
    else
    {
        if (hIntf != NULL) {
            switch(pktTyp)
            {
            case __SEND_PACKET:
                hIntf->PacketSend(cstrPacketBuffer, s); 
                break;
            case __RECV_PACKET:
                hIntf->PacketRecv(cstrPacketBuffer, s); 
                break;
            }
        }
    }
    delete [] cstrPacketBuffer; //Free memory
    return errorCode;
}

//Converts an integer into its big-ENDian hexadecimal character representation
//Requires buffer of 8 bytes
void intToHexB(int iVal, char* output)
{
    const unsigned int numNibbles = 8; //Must be 8 nibbles for an int

    /* Positive 2's complement */
    if( iVal >= 0 )
        for( int i=0; i < numNibbles; ++i )
        {
            output[numNibbles-1-i] = charToHex((char)(iVal%16));
            iVal /= 16;
        }
    /* Negative 2's complement */
    else
    {
        iVal *= -1; //We want to deal with positive numbers, so make it positive
        iVal -= 1;  //Change because of 2's complement
        for( int i=0; i < numNibbles; ++i )
        {
            output[numNibbles-1-i] = charToHex((char)(15 - (iVal%16)));
            iVal /= 16;
        }
    }
}

std::string intToHexB(int iVal)
{
    const unsigned int numNibbles = 8; //Must be 8 nibbles for an int
    std::string output;
    output.resize(numNibbles);
    // Positive 2's complement
    if( iVal >= 0 )
        for( int i=0; i < numNibbles; ++i )
        {
            output[numNibbles-1-i] = charToHex((char)(iVal%16));
            iVal /= 16;
        }
    // Negative 2's complement 
    else
    {
        iVal *= -1; //We want to deal with positive numbers, so make it positive
        iVal -= 1;  //Change because of 2's complement
        for( int i=0; i < numNibbles; ++i )
        {
            output[numNibbles-1-i] = charToHex((char)(15 - (iVal%16)));
            iVal /= 16;
        }
    }
    return output;
}

// Converts a big-ENDian hexadecimal character to its integer representation.
// Requires 8 bytes for input buffer
int hexToIntB(const char *src)
{
    const unsigned int maxSize = 12; //Must be 12 -- 1 for negative, 8 for nibbles
    int retVal = 0;
    int start = 0, end = -1;
    for( int i=0; i < maxSize; ++i ) //Get size
    {
        if( ( (src[i] | 0x20) == 'x') ) //Hexadecimal check
            start = i+1;
        if( src[i] == '\0' )
		{
            end = i-1;
			break;
		}
    }
    if( end >= start ) //Get value
    {
        int k = 0;
        //Negative value
        if( src[0] == '-' )
            ++start; //Increment the starting value
        for( int j=end; j >= start; --j )
        {
            if( (src[j] >= '0' && src[j] <= '9') ||
                (src[j] >= 'A' && src[j] <= 'Z') ||
                (src[j] >= 'a' && src[j] <= 'z') )
            retVal +=  (src[j] <= '9' ? (src[j] - '0') : (src[j] - 0x37)) << k;
            k += 4;
        }
        if( src[0] == '-' ) //If negative, negate value
            retVal *= -1;
    }
    return retVal;
}

//Converts an integer into its lil-ENDian hexadecimal character representation
std::string intToHexL(int iVal, int numNibbles, bool includeSpaces)
{
    std::string strOutput;

    /* Positive 2's complement */
    if( iVal >= 0 )
        for( int i=0; i < numNibbles; ++i )
        {
            if( i%2 == 0 )
                strOutput.push_back(charToHex((char)(iVal%16)));
            else
            {
                strOutput.insert( strOutput.end()-1, charToHex((char)(iVal%16)) );
                if( includeSpaces )
                    strOutput.push_back( ' ' );
            }
            iVal /= 16;
        }
    /* Negative 2's complement */
    else
    {
        iVal *= -1; //We want to deal with positive numbers, so make it positive
        iVal -= 1; //Change because of 2's complement
        for( int i=0; i < numNibbles; ++i )
        {
            if( i%2 == 0 )
                strOutput.push_back(charToHex((char)(15 - (iVal%16))));
            else
            {
                strOutput.insert( strOutput.end()-1, charToHex((char)(15 - (iVal%16))) );
                if( includeSpaces )
                    strOutput.push_back( ' ' );
            }
            iVal /= 16;
        }
    }
    return strOutput;
}

char charToHex(char cVal)
{
    char retVal = 'F';
    if(cVal <= 15) 
    {
        if(cVal < 10) 
            retVal = (char)(cVal+0x30);
        else 
            retVal = (char)(cVal+0x37);
    }
    return retVal;
}

// doesn't work
std::string LongDoubleToString( __in const long double val )
{
    std::stringstream ss;
    ss << val;

    return ss.str();
}

DWORD DereferenceD( DWORD *adr )
{
    DWORD retVal = 0;
    
    ReadPointer( &static_cast<unsigned long>(retVal), (LPCVOID)adr );

    return retVal;
}
