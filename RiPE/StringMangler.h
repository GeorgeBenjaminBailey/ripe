#ifndef __STRING_MANGLER_H
#define __STRING_MANGLER_H

#include <string>

/*
    "Encodes" strings by shifting them 13 to the right.  "Decodes" by shifting 13 to the left.
*/
class StringMangler {
public: static std::string Encode(std::string str);

public: static std::string Decode(std::string str);
};

#endif