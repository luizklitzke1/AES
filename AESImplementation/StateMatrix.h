#pragma once

#include <vector>

#include "AESConstants.h"

//Utilizada tanto para o estado da matriz como para as keys
class CStateMatrix
{
public:
    void SetWordFromArray(const std::vector<long>& aValues, const ULONG ulWordIdx);

public:
    void SubBytes  ();
    void ShiftRows ();
    void MixColumns();
    
    CStateMatrix XOR(const CStateMatrix& stateMatrix) const;

    std::array<WORD, WORDS_PER_STATE> m_Matrix;
};
