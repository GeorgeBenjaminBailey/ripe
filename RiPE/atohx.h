#ifndef __RIU_ATOHX_H
#define __RIU_ATOHX_H

#include <cctype>

#include "memreadwrite.h"

class AsciiToHex
{
private:
    AsciiToHex(){;}

public:
    static inline char * atohx(char *dst, const char *src)
    {
        unsigned char nullValue = 0, replaceValue=0;
        char *ret = dst;
        for( int lsb, msb; *src; src += 2 )
        {
            msb = tolower(*src);
            //msb = tolower( (char)ReadAddress(src) );
            lsb = tolower(*(src + 1));
            //lsb = tolower( (char)ReadAddress((LPCVOID)((DWORD)src+1)) );
            msb -= isdigit(msb) ? 0x30 : 0x57;
            lsb -= isdigit(lsb) ? 0x30 : 0x57;
            if( (msb < 0x0 || msb > 0xf) || (lsb < 0x0 || lsb > 0xf) )
            {
                *ret = 0;
                //WriteAddress( ret, &nullValue, sizeof(char) );
                return NULL;
            }
            *dst++ = (char)(lsb | (msb << 4));
//             replaceValue = (char)(lsb | (msb << 4));
//             WriteAddress( dst, &replaceValue, sizeof(char) );
        }
        *dst = 0;
        //WriteAddress( dst, &nullValue, sizeof(char) );
        return ret;
    }
};

#endif //__RIU_ATOHX_H