#pragma once
#include <stdexcept>

#include "StateMatrix.h"
#include "AESUtils.h"

void CStateMatrix::SetWordFromArray(const std::vector<long>& aValues, const ULONG ulWordIdx)
{
    const long lBeginIdx = ulWordIdx * WORD_LENGTH;

    std::copy(aValues.begin() + lBeginIdx,
              aValues.begin() + lBeginIdx + WORD_LENGTH,
              m_Matrix[ulWordIdx].begin());
}

void CStateMatrix::SubBytes()
{
    for (size_t idxWord = 0; idxWord < WORDS_PER_STATE; ++idxWord)
        m_Matrix[idxWord] = CAESUtils::SubWord(m_Matrix[idxWord]);
}

void CStateMatrix::ShiftRows()
{
    std::array<AESWORD, WORDS_PER_STATE> matrixAux; //Por algum motivo, não funciona declarar todos logo na definição :/
    matrixAux[0] = { m_Matrix[0][0], m_Matrix[0][1], m_Matrix[0][2], m_Matrix[0][3] };
    matrixAux[1] = { m_Matrix[1][1], m_Matrix[1][2], m_Matrix[1][3], m_Matrix[1][0] };
    matrixAux[2] = { m_Matrix[2][2], m_Matrix[2][3], m_Matrix[2][0], m_Matrix[2][1] };
    matrixAux[3] = { m_Matrix[3][3], m_Matrix[3][0], m_Matrix[3][1], m_Matrix[3][2] };

    m_Matrix = matrixAux;
}

void CStateMatrix::MixColumns()
{
}

AESWORD& CStateMatrix::operator[](std::size_t index)
{
    if (index >= WORDS_PER_STATE)
        throw std::out_of_range("Index out of range");

    return m_Matrix[index];
}

const AESWORD& CStateMatrix::operator[](std::size_t index) const
{
    if (index >= WORDS_PER_STATE)
        throw std::out_of_range("Index out of range");

    return m_Matrix[index];
}
