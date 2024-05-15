#pragma once
#include <stdexcept>

#include "StateMatrix.h"
#include "AESUtils.h"

void CStateMatrix::SetWordFromArray(const std::vector<long>& aValues, const ULONG ulWordIdx)
{
    const long lBeginIdx = ulWordIdx * WORD_LENGTH;

    std::copy(aValues.begin() + lBeginIdx,
              aValues.begin() + lBeginIdx + WORD_LENGTH,
              m_Matrix[lBeginIdx].begin());
}

void CStateMatrix::ShiftRows()
{
}

void CStateMatrix::MixColumns()
{
}

CStateMatrix XOR(const CStateMatrix& stateMatrix)
{
    //TODO
    CStateMatrix todo;
    return todo;
}

WORD& CStateMatrix::operator[](std::size_t index)
{
    if (index >= WORDS_PER_STATE)
        throw std::out_of_range("Index out of range");

    return m_Matrix[index];
}

const WORD& CStateMatrix::operator[](std::size_t index) const
{
    if (index >= WORDS_PER_STATE)
        throw std::out_of_range("Index out of range");

    return m_Matrix[index];
}
