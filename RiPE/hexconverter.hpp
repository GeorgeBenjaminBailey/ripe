#ifndef HEX_CONVERTER_HPP
#define HEX_CONVERTER_HPP

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

// converts decimal to a big-Endian hexadecimal representation.
// hexadecimal representation uses capital ASCII characters
template< typename T >
std::string DecimalToHex( T a )
{
    std::string result = "";

    // start with all 0's
    for( int i=0; i < sizeof(T)*2; ++i )
        result += '0';

    std::string::iterator iter = result.end();
    for( ; iter != result.begin() && a != 0; )
    {
        --iter; // we need to prematurely move the iter back since we start at end
        char nibble = static_cast<char>(a & 0xF);
        nibble += (nibble <= 9 ? '0' : ('A'-10));
        *iter = nibble;

        a >>= 4; // each nibble is 4 bits
    }

    return result;
}


// converts decimal to a lil-Endian hexadecimal representation.
// hexadecimal representation uses capital ASCII characters
template< typename T >
std::string DecimalToHexL( T a )
{
    std::string result = "";

    // start with all 0's
    for( int i=0; i < sizeof(T)*2; ++i )
        result += '0';

    std::string::iterator iter = result.end();
    for( std::size_t i=0; i < sizeof(T)*2 ; ++i )
    {
        int iter = i%2 == 0 ? i+1 : i-1;
        char nibble = static_cast<char>(a & 0xF);
        nibble += (nibble <= 9 ? '0' : ('A'-10));
        result[iter] = nibble;

        a >>= 4; // each nibble is 4 bits
    }

    return result;
}

// converts a non-string to a string
template< typename T >
std::string ValueToString( T a )
{
    std::stringstream result;
    result << a;

    return result.str();
}

// converts a big-Endian C-String hex value to its decimal equivalent
template< typename T >
T HexToDecimal( const char *src )
{
    T retVal = 0;
    int start = 0, end = -1;
    for( int i=0;; ++i ) //Get size
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
        T k = 0;
        //Negative value
        if( src[0] == '-' )
            ++start; //Increment the starting value
        for( int j=end; j >= start; --j )
        {
            if( (src[j] >= '0' && src[j] <= '9') ||
                (src[j] >= 'A' && src[j] <= 'F') ||
                (src[j] >= 'a' && src[j] <= 'f') )
                retVal +=  (src[j] <= '9' ? (src[j] - '0') : ((src[j] & 0xDF) - 0x37)) << k;
            else if( src[j] == '?' || src[j] == '*' )
                retVal += rand()%0x10 << k;
            k += 4;
        }
        if( src[0] == '-' ) //If negative, negate value
            retVal *= -1;
    }
    return retVal;
}

// converts a lil-Endian C-String hex value to its decimal equivalent
template< typename T >
T HexToDecimalL( const char *src )
{
    T retVal = 0;
    int start = 0, end = -1;
    for( int i=0;; ++i ) //Get size
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
        T k = 0;
        //Negative value
        if( src[0] == '-' )
            ++start; //Increment the starting value
        for( int j=start; j <= end; ++j )
        {
            int iter = j%2 == 0 ? j+1 : j-1;
            if( (src[iter] >= '0' && src[iter] <= '9') ||
                (src[iter] >= 'A' && src[iter] <= 'F') ||
                (src[iter] >= 'a' && src[iter] <= 'f') )
                retVal +=  (src[iter] <= '9' ? (src[iter] - '0') : ((src[iter] & 0xDF) - 0x37)) << k;
            else if( src[iter] == '?' || src[iter] == '*' )
                retVal += rand()%0x10 << k;
            k += 4;
        }
        if( src[0] == '-' ) //If negative, negate value
            retVal *= -1;
    }
    return retVal;
}

// Converts a C-string hex value to its decimal equivalent
std::vector<unsigned char> HexToArray( const char *src );

char * atohx(char *dst, const char *src);

// Converts a char to a printable character (ASCII codes 0x20 to 0x7E)
char CharToPrintable( const char a );

#endif //HEX_CONVERTER_HPP