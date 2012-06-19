#include "stdafx.h"

#include "hexconverter.hpp"

// Converts a C-string hex value to its decimal equivalent
std::vector<unsigned char> HexToArray( const char *src )
{
    size_t len = strlen(src);
    std::vector<unsigned char> result;
    char buf[3] = {0};
    size_t bufPos = 0;

    for( size_t i=0; i < len; ++i )
    {
        if( src[i] != ' ' )
        {
            buf[bufPos++] = src[i];
            if( bufPos == 2 )
            {
                bufPos = 0;
                result.push_back(HexToDecimal<unsigned char>(buf));
            }
        }
    }

    // incomplete packet
    if( bufPos == 1 )
    {
        buf[1] = buf[0];
        buf[0] = '0';
        result.push_back(HexToDecimal<unsigned char>(buf));
    }

    return result;
}

char * atohx(char *dst, const char *src)
{
    char *ret = dst;
    for( int lsb, msb; *src; src += 2 )
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
        *dst++ = (char)(lsb | (msb << 4));
    }
    *dst = 0;
    return ret;
}

// Converts a char to a printable character (ASCII codes 0x20 to 0x7E)
char CharToPrintable( const char a )
{
    return (a >= 20 && a <= 0x7E) ? a : '.';
}
