#pragma once
#include "AESConstants.h"
#include "StateMatrix.h"

#include <string>
#include <sstream>

class CAESUtils
{
public:
    static bool IsNumber(const std::string s);

    static ULONG GetLeastSignificantBits(long value);
    static ULONG GetMostSignificantBits(long value);

    static long SubByte(const long lValue);

    static AESWORD RotWord (const AESWORD& word);
    static AESWORD SubWord (const AESWORD& word);

    static AESWORD      XORWords (const AESWORD&      wordA       , const AESWORD&      wordB       );
    static CStateMatrix XORStates(const CStateMatrix& stateMatrixA, const CStateMatrix& stateMatrixB);

    static std::string WordToString(const AESWORD& word);
};
