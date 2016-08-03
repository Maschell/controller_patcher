#include <string>
#include <vector>

#include <stdio.h>
#include <string.h>
#include <gctypes.h>
#include "string_tools.hpp"

bool EndsWith(const std::string& a, const std::string& b) {
    if (b.size() > a.size()) return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

std::vector<std::string> MyStringSplit(const std::string & inValue, const std::string & splitter)
{
    std::string value = inValue;
    std::vector<std::string> result;
    while (true) {
        unsigned int index = value.find(splitter);
        if (index == std::string::npos) {
            result.push_back(value);
            break;
        }
        std::string first = value.substr(0, index);
        result.push_back(first);
        if (index + splitter.size() == value.length()) {
            result.push_back("");
            break;
        }
        if(index + splitter.size() > value.length()) {
            break;
        }
        value = value.substr(index + splitter.size(), value.length());
    }
    return result;
}


extern "C" const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}
