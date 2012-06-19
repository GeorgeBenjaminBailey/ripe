#ifndef __PARTIAL_PACKET_HPP
#define __PARTIAL_PACKET_HPP

#include <list>

#include <wx/string.h>

#include "../hexconverter.hpp"

class PartialPacket
{
private:
public:
    PartialPacket()
    {
    }

    PartialPacket( const wxString &pktStr )
    {

    }

    ~PartialPacket()
    {

    }

    virtual wxString GetPacket()
    {
        return _("");
    }

    virtual int GetSize()
    {
        return 0;
    }

    virtual wxString ToString()
    {
        return _("");
    }
};

class PartialPacketString : public PartialPacket
{
protected:
    wxString currentPkt;

public:
    PartialPacketString() {

    }

    PartialPacketString( const wxString &pktStr ) {
        currentPkt = pktStr;
    }

    ~PartialPacketString() {

    }

    virtual wxString GetPacket() {
        wxString result = currentPkt;

        for( size_t i=0; i < result.size(); ++i ) {
            if( result[i] == '*' || result[i] == '?' ) {
                int r = rand()%0x10;
                result[i] = r <= 9 ? '0' + r : 'A'+r-10;
            }
        }

        return result;
    }

    // return size in nibbles
    virtual int GetSize() {
        return currentPkt.size();
    }

    virtual wxString ToString() {
        return currentPkt;
    }
};

enum PartialPacketTokenType {
    PARTIAL_PACKET_ADDITION = 0,
    PARTIAL_PACKET_SUBTRACTION,
    PARTIAL_PACKET_MULTIPLICATION,
    PARTIAL_PACKET_DIVISION,
    PARTIAL_PACKET_VALUE,
    PARTIAL_PACKET_PACKET
};

struct PartialPacketToken {
    PartialPacketTokenType type;
    wxString               value;

    PartialPacketToken() {
        type  = PARTIAL_PACKET_VALUE;
        value = _("");
    }

    PartialPacketToken( const PartialPacketTokenType typ, const wxString &val = _("") ) {
        type  = typ;
        value = val;
    }
};

class PartialPacketDynamicString : public PartialPacketString
{
private:
    unsigned long long min, max;
    unsigned char numNibbles; // number of nibbles needed to represent the partial packet
    std::list<PartialPacketToken> tokens;

    void PerformAddition( wxString &a, const wxString &b )
    {   
        unsigned long long sum = HexToDecimalL<unsigned long long>(a.mb_str()) + HexToDecimal<unsigned long long>(b);
        if( max-min+1 != 0 ) // max-min+1 should never be 0, but just in case...
            sum = ((sum-min) % (max-min+1)) + min;
        a = DecimalToHexL(sum);
    }

    void PerformSubtraction( wxString &a, const wxString &b )
    {
        unsigned long long difference = HexToDecimalL<unsigned long long>(a.mb_str()) - HexToDecimal<unsigned long long>(b);
        if( max-min+1 != 0 ) // max-min+1 should never be 0, but just in case...
            difference = ((difference-min) % (max-min+1)) + min;
        a = DecimalToHexL(difference);
    }

    void PerformMultiplication( wxString &a, const wxString &b )
    {
        unsigned long long s = HexToDecimalL<unsigned long long>(a.mb_str()) * HexToDecimal<unsigned long long>(b);
        if( max-min+1 != 0 ) // max-min+1 should never be 0, but just in case...
            s = ((s-min) % (max-min+1)) + min;
        a = DecimalToHexL(s);
    }

    void PerformDivision( wxString &a, const wxString &b )
    {
        unsigned long long s = HexToDecimalL<unsigned long long>(a.mb_str()) / HexToDecimal<unsigned long long>(b);
        if( max-min+1 != 0 ) // max-min+1 should never be 0, but just in case...
            s = ((s-min) % (max-min+1)) + min;
        a = DecimalToHexL(s);
    }

    void PerformOperation( const PartialPacketTokenType t, const PartialPacketToken &value, wxString &str )
    {
        wxString b = (value.type == PARTIAL_PACKET_PACKET ? str : value.value);
        switch( t )
        {
          case PARTIAL_PACKET_ADDITION:
            PerformAddition( str, b );
            break;
          case PARTIAL_PACKET_SUBTRACTION:
            PerformSubtraction( str, b );
            break;
          case PARTIAL_PACKET_MULTIPLICATION:
            PerformMultiplication( str, b );
            break;
          case PARTIAL_PACKET_DIVISION:
            PerformDivision( str, b );
            break;
        }
    }

    // updates the current packet by performing token operations on it
    // tokens are guaranteed to be of the following format where 
    // t = token and v = value:
    // tvtvtvtv
    void UpdateCurrentPacket()
    {
        wxString newPkt = currentPkt;
        std::list<PartialPacketToken>::iterator iter = tokens.begin();
        for( ; iter != tokens.end(); ++iter )
        {
            if( iter != tokens.end() )
            {
                std::list<PartialPacketToken>::iterator oldIter = iter;
                PerformOperation( oldIter->type, *(++iter), newPkt );
            }
        }
        currentPkt = newPkt.Mid(0, numNibbles);
    }

    // tokenizes the string.  Edges must NOT have []'s
    void Tokenize( const wxString &pkt )
    {
        wxString temp = pkt;
        wxString minStr, maxStr;
        unsigned long long cur; // temporary long long to hold the "current" packet
        wxString str[4]; // at most we allow 4 parts to the packet
        temp.Replace(_(":"), _(",")); // replace :'s with ,'s (they're both separators)
        temp.Replace(_(";"), _(",")); // replace ;'s with ,'s (they're both separators)
        // explode the string
        for( int i=0; i < 4; ++i ) {
            str[i] = temp.BeforeFirst(',');
            temp   = temp.AfterFirst(',');
        }

        // if there are 4 parts to the packet and not more than 4
        if( str[3].size() > 0 && temp.size() == 0 ) {
            // minimum
            if( str[1].size() > 0 ) {
                min = HexToDecimal<unsigned long long>(str[1]);
            } else {
                min = 0;
            }
            // maximum
            if( str[2].size() > 0 ) {
                max = HexToDecimal<unsigned long long>(str[2]);
            } else {
                max = 0;
            }
            // current
            if( str[0].size() > 0 ) {
                cur = HexToDecimal<unsigned long long>(str[0]);
            } else {
                cur = min;
            }
            // numNibbles
            numNibbles = str[2].size() > str[1].size() ? str[2].size() : str[1].size();
            numNibbles = str[0].size() > numNibbles ? str[0].size() : numNibbles;
            temp = str[3]; // we use temp later on to hold the expressions
        } else if( str[2].size() > 0 && temp.size() == 0 ) { // if there are 3 parts
            // minimum
            if( str[0].size() > 0 ) {
                min = HexToDecimal<unsigned long long>(str[0]);
            } else {
                min = 0;
            }
            // maximum
            if( str[1].size() > 0 ) {
                max = HexToDecimal<unsigned long long>(str[1]);
            } else {
                max = 0;
            }
            // current
            cur = min;
            // numNibbles
            numNibbles = str[1].size() > str[0].size() ? str[1].size() : str[0].size();
            temp = str[2]; // we use temp later on to hold the expressions
        } // else error
        numNibbles %= 16+1; // can't have more than 16 nibbles

        if( min > max ) // if start is greater than stop
        {
            std::swap(min,max);
        }
        // if cur is out of range
        if( !(cur >= min && cur <= max) ) {
            cur = min;
        }
        currentPkt = DecimalToHexL(cur);
        currentPkt = currentPkt.Mid(0,numNibbles);

        wxString val = _("");
        PartialPacketToken pktToken;
        for( size_t i=0, j=0; i < temp.size(); ++i )
        {
            bool foundToken = true;
            switch( static_cast<char>(temp[i]))
            {
            case '+':
                pktToken.type  = PARTIAL_PACKET_ADDITION;
                pktToken.value = _("+");
                break;
            case '-':
                pktToken.type  = PARTIAL_PACKET_SUBTRACTION;
                pktToken.value = _("-");
                break;
            case '*':
                pktToken.type  = PARTIAL_PACKET_MULTIPLICATION;
                pktToken.value = _("*");
                break;
            case '/':
                pktToken.type  = PARTIAL_PACKET_DIVISION;
                pktToken.value = _("/");
                break;
            case 'P': // case p should always follow this
            case 'p':
                pktToken.type  = PARTIAL_PACKET_PACKET;
                pktToken.value = _("p");
                break;
            default:
                foundToken = false;
            }

            // add token with a value
            if( !foundToken )
            {
                val += temp[i];
            }
            else
            {
                if( val.size() > 0 ) // append the partial packet value before the other token
                {
                    if( tokens.size() == 0 ) // if there was no expression token before this value, then default to addition
                        tokens.push_back( PartialPacketToken(PARTIAL_PACKET_ADDITION, _("+")) );
                    tokens.push_back( PartialPacketToken(PARTIAL_PACKET_VALUE, val) );
                    val.Clear();
                }

                if( tokens.back().type == PARTIAL_PACKET_ADDITION ||
                    tokens.back().type == PARTIAL_PACKET_SUBTRACTION ||
                    tokens.back().type == PARTIAL_PACKET_MULTIPLICATION ||
                    tokens.back().type == PARTIAL_PACKET_DIVISION )
                {
                    tokens.back() = pktToken; // only store last symbol
                }
                else
                    tokens.push_back(pktToken);
            }
        }
        if( val.size() > 0 )
        {
            if( tokens.size() == 0 ) // if there was no expression token before this value, then default to addition
                tokens.push_back( PartialPacketToken(PARTIAL_PACKET_ADDITION, _("+")) );
            tokens.push_back( PartialPacketToken(PARTIAL_PACKET_VALUE, val) );
        }
        else if( 
            tokens.size() > 0 &&
            (tokens.back().type == PARTIAL_PACKET_ADDITION ||
             tokens.back().type == PARTIAL_PACKET_SUBTRACTION ||
             tokens.back().type == PARTIAL_PACKET_MULTIPLICATION ||
             tokens.back().type == PARTIAL_PACKET_DIVISION) )
        {
            tokens.pop_back(); // can't end with a symbol
        }
    }

public:

    PartialPacketDynamicString()
    {
    }

    PartialPacketDynamicString( const wxString &pkt )
    {
        if( pkt.size() > 2 )
            Tokenize( pkt.Mid(1,pkt.size()-2) );
    }

    ~PartialPacketDynamicString()
    {
    }

    virtual wxString GetPacket()
    {
        wxString result = currentPkt;
        UpdateCurrentPacket();
        return result;
    }

    virtual int GetSize()
    {
        return numNibbles/2;
    }

    virtual wxString ToString()
    {
        wxString result = "[";
        result         += DecimalToHex(min);
        result         += ",";
        result         += DecimalToHex(max);
        result         += ",";

        // for all tokens, append the appropriate character(s)
        std::list<PartialPacketToken>::iterator iter = tokens.begin();
        for( ; iter != tokens.end(); ++iter )
        {
            result += iter->value;
        }
        result += "]";

        return result;
    }
};

#endif //__PARTIAL_PACKET_HPP