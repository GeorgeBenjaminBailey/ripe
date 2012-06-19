#ifndef __USEFUL_FUNCTIONS
#define __USEFUL_FUNCTIONS

#include <string>
#include <windows.h>
#include <queue>
#include <sstream>

#include "defs.h"

#include "millitime.h"
#include "RiPE/hookinterface.h"
#include "RiPE/communicationlayer.h"
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

enum sendPacketType
{
	__RECV_PACKET = 0,
	__SEND_PACKET
};

//Useful functions that can be used by multiple classes should go here

ErrorMessage SendPacketToProcess(std::string, CommunicationLayer &communicationLayer, sendPacketType pktTyp = __SEND_PACKET, SOCKET s=0);

/** Converts an int to the big-ENDian hexadecimal character representation.
 * Requires 8 bytes for output buffer */
void intToHexB( int, char * );

std::string intToHexB( int );

/** Converts a big-ENDian hexadecimal character to its integer representation. */
int hexToIntB( const char *src );

void parsePacket(char *cstrPacketBuffer, std::string strPacketBuf);

/* Converts an int to the Lil-endian string representation of that int.
 * The second parameter is optional and designates number of chars that
 * are returned as a string. */
std::string intToHexL(int, int numNibbles=8, bool includeSpaces=false);

/* Takes a char from 0-15 as input and outputs a char representing the
 * hexadecimal character */
char charToHex(char);

//Safely dereferences a DWORD *
DWORD DereferenceD( __in DWORD *adr );

std::string LongDoubleToString( __in const long double val );

#endif //__USEFUL_FUNCTIONS
