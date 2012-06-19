#include "stdafx.h"
#include "StringMangler.h"

std::string StringMangler::Encode(std::string str) {
    for (unsigned int i=0; i < str.length(); ++i) {
        str[i] += 13;
        if (str[i] > 126) {
            str[i] = (str[i]-126) + 31;
        }
    }
    return str;
}

std::string StringMangler::Decode(std::string str) {
    for (unsigned int i=0; i < str.length(); ++i) {
        str[i] -= 13;
        if (str[i] < 32) {
            str[i] = (str[i]+126) - 31;
        }
    }
    return str;
}
