#pragma once
#include "AESConstants.h"
#include "StateMatrix.h"

#include <string>
#include <sstream>

class CAESUtils
{
public:
    static bool IsNumber(const std::string s);

    static long GetLeastSignificantBits(const long lValor);
    static long GetMostSignificantBits (const long lValor);

    static long SubByte(const long lValor);

    static AESWORD RotWord(const AESWORD& word);
    static AESWORD SubWord(const AESWORD& word);

    static AESWORD      XORWords (const AESWORD&      wordA       , const AESWORD&      wordB       );
    static CStateMatrix XORStates(const CStateMatrix& stateMatrixA, const CStateMatrix& stateMatrixB);

    static long GaloisFieldMultiplication(const long lValorA, const long lValorB);

    static std::string LongToHex   (const long     lValor);
    static std::string WordToString(const AESWORD& word  );
};
