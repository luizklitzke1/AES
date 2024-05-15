#pragma once

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
