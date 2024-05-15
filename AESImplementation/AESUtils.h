#pragma once
#include "AESConstants.h"

#include <string>
#include <sstream>

class CAESUtils
{
public:
    static bool IsNumber(const std::string s);

    static ULONG GetLeastSignificantBits(long value);
    static ULONG GetMostSignificantBits(long value);

    static WORD RotWord (const WORD& word);
    static WORD SubWord (const WORD& word);
    static WORD XORWords(const WORD& wordA, const WORD& wordB);

    static std::string WordToString(const WORD& word);
};
