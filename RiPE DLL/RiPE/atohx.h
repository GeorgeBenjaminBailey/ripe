#ifndef ATOHX_H
#define ATOHX_H

class AsciiToHex
{
private:
    AsciiToHex(){;}

public:
    static inline char * atohx(char *dst, const char *src)
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
};

#endif //ATOHX_H