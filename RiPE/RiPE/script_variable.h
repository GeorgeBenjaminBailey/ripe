#ifndef __RIPE_SCRIPT_VARIABLE_H
#define __RIPE_SCRIPT_VARIABLE_H

#include <vector>

#include <wx/string.h>

#include "../hexconverter.hpp"

namespace Script
{
    // storage structure for a script variable.
    // e.g.:  0x40, 42, "0A 00"
    class Variable
    {
    private:
        enum Type
        {
            TYPE_ARRAY,
            TYPE_VALUE
        };

        unsigned int value;
        std::vector<unsigned char> byteArray;
        bool isNull;
        unsigned char trash;  // this is used to prevent errors from out-of-bounds accesses
        Type type;
    
    public:
        Variable()
        {
            isNull = true;
            type   = TYPE_VALUE;
            value  = 0;
        }

        Variable(const unsigned int num)
        {
            isNull = false;
            type   = TYPE_VALUE;
            value  = num;
        }

        // interpolation is "" vs ''.  "" are interpolated, and transformed
        // into hexadecimal.  '' aren't, and are directly converted to a byte
        // array.
        Variable(const wxString &str, bool interpolation=true)
        {
            isNull    = false;
            type      = TYPE_ARRAY;
            if( interpolation )
                byteArray = HexToArray(str.mb_str());
            else
            {
                wxString::size_type strLength = str.size();
                byteArray.resize(strLength);
                for( wxString::size_type i=0; i < strLength; ++i )
                    byteArray[i] = str[i];
            }
        }

        Variable(const std::vector<unsigned char> bytes)
        {
            isNull = false;
            type   = TYPE_ARRAY;
            byteArray = bytes;
        }

        ~Variable()
        {}

        bool IsNull()
        {
            return isNull;
        }

        // Returns the address of the int or byte array
        void * GetAddress()
        {
            if( type == TYPE_VALUE )
                return &value;
            else
                return byteArray.data();
        }

        unsigned int GetSize()
        {
            if( type == TYPE_ARRAY )
                return byteArray.size();
            else
                return sizeof(value);
        }

        // Convert data type to array
        Variable ToArray(const unsigned int valueSize=4)
        {
            if( type != TYPE_ARRAY )
            {
                byteArray.clear();
                if( isNull )
                    value = 0;
                for( unsigned int i=0; i < valueSize && i < sizeof(value); ++i )
                {
                    byteArray.push_back( (unsigned char)((value >> i*8) & 0xFF) );
                }
                type = TYPE_ARRAY;
            }
            return *this;
        }

        // Convert data type to value
        Variable ToValue()
        {
            if( type != TYPE_VALUE )
            {
                value = 0;
                if( !isNull )
                {
                    for( unsigned int i=0; i < sizeof(value) && i < byteArray.size(); ++i )
                    {
                        value |= (((unsigned int)byteArray[i]) << i*8);
                    }
                }
                type = TYPE_VALUE;
            }
            return *this;
        }

        // Return data type as if it were a value
        unsigned int AsValue() const
        {
            unsigned int result = 0;
            if( isNull )
                result = 0;
            else if( type == TYPE_VALUE )
                result = value;
            else if( type == TYPE_ARRAY )
            {
                for( unsigned int i=0; i < sizeof(value) && i < byteArray.size(); ++i )
                    result |= (((unsigned int)byteArray[i]) << i*8);
            }
            return result;
        }

        std::vector<unsigned char> AsArray(const unsigned int valueSize=4) const
        {
            if( !isNull && type == TYPE_ARRAY )
                return byteArray;

            std::vector<unsigned char> result;
            unsigned int val = value;

            if( isNull )
                val = 0;
            for( unsigned int i=0; i < valueSize && i < sizeof(value); ++i )
            {
                result.push_back( (unsigned char)((val >> i*8) & 0xFF) );
            }
            return result;
        }

        wxString AsString(const unsigned int valueSize=4) const
        {
            std::vector<unsigned char> arr = AsArray(valueSize);
            wxString result;

            std::vector<unsigned char>::iterator iter = arr.begin();
            for( ; iter != arr.end(); ++iter )
            {
                result += DecimalToHex(*iter);
                result += " ";
            }

            return result;
        }

        /**
         * Converts Variable to an ASCII string.  This assumes that the Variable
         * does NOT need any conversion from its raw vector data.
         */
        wxString AsASCIIString() const
        {
            wxString result = "";
            std::vector<unsigned char> arr = AsArray();
            std::vector<unsigned char>::iterator iter = arr.begin();
            for( ; iter != arr.end(); ++iter )
                result += (char)*iter;

            return result;
        }

        Variable& operator=(const Variable &other)
        {
            if( this == &other )
                return *this;

            isNull    = other.isNull;
            byteArray = other.byteArray;
            value     = other.value;
            type      = other.type;
            return *this;
        }

        Variable& operator=(const wxString &other)
        {
            return *this = Variable(other);
        }

        Variable& operator=(const unsigned int &other)
        {
            return *this = Variable(other);
        }

        Variable & operator+=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();

            if( !other.isNull )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value += otherCopy.value;
                }
                else
                    value += other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator+(const Variable &other) const
        {
            return Variable(*this) += other;
        }

        Variable & operator-=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value -= otherCopy.value;
                }
                else
                    value -= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator-(const Variable &other) const
        {
            return Variable(*this) -= other;
        }

        Variable & operator*=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value *= otherCopy.value;
                }
                else
                    value *= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator*(const Variable &other) const
        {
            return Variable(*this) *= other;
        }

        Variable & operator/=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull && other.AsValue() != 0 )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value /= otherCopy.value;
                }
                else
                    value /= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator/(const Variable &other) const
        {
            return Variable(*this) /= other;
        }

        Variable & operator%=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull && other.AsValue() != 0 )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value %= otherCopy.value;
                }
                else
                    value %= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator%(const Variable &other) const
        {
            return Variable(*this) %= other;
        }

        Variable & operator^=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value ^= otherCopy.value;
                }
                else
                    value ^= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator^(const Variable &other) const
        {
            return Variable(*this) ^= other;
        }

        Variable & operator&=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value &= otherCopy.value;
                }
                else
                    value &= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator&(const Variable &other) const
        {
            return Variable(*this) &= other;
        }

        Variable & operator|=(const Variable &other)
        {
            // Both sides must be a value for addition
            ToValue();
            if( !other.isNull )
            {
                if( other.type != TYPE_VALUE )
                {
                    Variable otherCopy = other;
                    otherCopy.ToValue();
                    value |= otherCopy.value;
                }
                else
                    value |= other.value;
                isNull = false;
            }

            return *this;
        }

        const Variable operator|(const Variable &other) const
        {
            return Variable(*this) |= other;
        }

        /*const Variable operator~=() const
        {
            // Both sides must be a value for addition
            Variable other = *this;
            other.ToValue();
            other.value ~= other.value;
            other.isNull = false;

            return other;
        }

        const Variable operator~() const
        {
            return Variable(*this) ~= *this;
        }*/

        Variable & operatorDotEquals(const Variable &other)
        {
            // Both sides must be an array for concatenation
            ToArray();
            if( !other.isNull )
            {
                if( other.type != TYPE_ARRAY )
                {
                    Variable otherCopy = other;
                    otherCopy.ToArray();
                    for( unsigned int i=0; i < otherCopy.byteArray.size(); ++i )
                        byteArray.push_back(otherCopy.byteArray[i]);
                }
                else
                {
                    for( unsigned int i=0; i < other.byteArray.size(); ++i )
                        byteArray.push_back(other.byteArray[i]);
                }
                isNull = false;
            }

            return *this;
        }

        const Variable operatorDot(const Variable &other) const
        {
            return Variable(*this).operatorDotEquals(other);
        }

        bool operator==(const Variable &other) const 
        {
            bool result = true;

            if( isNull == true && other.isNull == true )
                result = true;
            else if( isNull == true || other.isNull == true )
                result = false;
            else if( type == TYPE_VALUE && other.type == TYPE_VALUE )
                result = (value == other.value);
            else if( type == TYPE_VALUE && other.type == TYPE_ARRAY )
            {
                std::vector<unsigned char> thisArr = AsArray(other.byteArray.size());
                for( unsigned int i=0; i < thisArr.size() && i < other.byteArray.size(); ++i )
                {
                    if( thisArr[i] != other.byteArray[i] )
                        result = false;
                }
            }
            else if( type == TYPE_ARRAY && other.type == TYPE_VALUE )
            {
                Variable otherCopy = other;
                std::vector<unsigned char> otherArr = otherCopy.AsArray(byteArray.size());
                for( unsigned int i=0; i < byteArray.size() && i < otherArr.size(); ++i )
                {
                    if( byteArray[i] != otherArr[i] )
                        result = false;
                }
            }
            else if( type == TYPE_ARRAY && other.type == TYPE_ARRAY )
            {
                for( unsigned int i=0; i < byteArray.size() && i < other.byteArray.size(); ++i )
                {
                    if( byteArray[i] != other.byteArray[i] )
                        result = false;
                }
            }
            return result;
        }

        bool operator!=(const Variable &other) const
        {
            return !(*this == other);
        }

        bool operator&&(const Variable &other) const
        {
            Variable n, z=0;
            return (*this != n && *this != z) &&
                   (other != n && other != z);
        }

        bool operator||(const Variable &other) const
        {
            Variable n, z=0;
            return (*this != n && *this != z) ||
                (other != n && other != z);
        }

        bool operator<(const Variable &other) const
        {
            bool result = true;

            if( isNull == true && other.isNull == true )
                result = false;
            else if( isNull == true || other.isNull == true )
                result = false; // undefined behavior
            else if( type == TYPE_VALUE && other.type == TYPE_VALUE )
                result = (value < other.value);
            else // We're not allowing huge byte arrays for this comparison
            {
                Variable thisCopy, otherCopy;
                thisCopy = this->AsValue();
                otherCopy = other.AsValue();
                result = (thisCopy.value < otherCopy.value);
            }
            
            return result;
        }

        bool operator<=(const Variable &other) const
        {
            return (*this < other) || (*this == other);
        }

        bool operator>(const Variable &other) const
        {
            return !(*this <= other);
        }

        bool operator>=(const Variable &other) const
        {
            return (*this > other) || (*this == other);
        }

        bool operator!() const
        {
            bool result = false;

            if( isNull )
                result = true;
            else
                result = !AsValue();

            return result;
        }

        const Variable operator++(int)
        {
            Variable temp(*this);
            *this += Variable(1);
            return temp;
        }

        const Variable operator--(int)
        {
            Variable temp(*this);
            *this -= Variable(1);
            return temp;
        }

        unsigned char & operator[](int i)
        {
            if( !isNull && i >= 0 )
            {
                if( type == TYPE_VALUE && i < 4 )
                    return *(((unsigned char *)&value)+i);
                else if( type == TYPE_ARRAY && static_cast<unsigned int>(i) < byteArray.size() )
                    return byteArray[i];
                // else badness
            }
            trash = 0;
            return trash; // badness happened
        }

        std::vector<unsigned char> operatorSplit(unsigned int i, unsigned int j=0) const
        {
            std::vector<unsigned char> result;
            if( !isNull && i >= 0 )
            {
                if( type == TYPE_VALUE && i < 4 )
                {
                    result.push_back( *(((unsigned char *)&value)+i) );
                }
                else if( type == TYPE_ARRAY && static_cast<unsigned int>(i) < byteArray.size() )
                {
                    if( j == 0 )
                        j = 1;
                    for( j+=i; i < j && i < byteArray.size(); ++i )
                    {
                        result.push_back( byteArray[i] );
                    }
                }
            }
            return result;
        }
    };
}

#endif // __RIPE_SCRIPT_VARIABLE_H
