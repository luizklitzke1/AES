#pragma once

#include <vector>
#include <string>
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

    void AdicionaValor(const long lValor);

    std::string ToString();

    AESWORD& operator[](std::size_t index);
    const AESWORD& operator[](std::size_t index) const;

public:
    std::array<AESWORD, WORDS_PER_STATE> m_Matrix;

private:
    ULONG m_ulWord          = 0;
    ULONG m_ulPoximaPosicao = 0;
};

