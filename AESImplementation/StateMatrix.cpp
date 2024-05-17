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
    long multiplicationMatrix[4][4] = { { 2, 1, 1, 3 },
                                        { 3, 2, 1, 1 },
                                        { 1, 3, 2, 1 },
                                        { 1, 1, 3, 2 }};

    //Matriz que armazena o resultado final dessa função
    CStateMatrix resultMatrix;

    for (size_t idxWord = 0; idxWord < WORDS_PER_STATE; ++idxWord)
    {
        const long lCol = idxWord;

        for (size_t idxValue = 0; idxValue < WORD_LENGTH; ++idxValue)
        {
            const long lRow = idxValue;

            long lFinalValue = 0;

            for (size_t idxMult = 0; idxMult < WORD_LENGTH; ++idxMult)
            {
                long lCurrentMultValue = CAESUtils::GaloisFieldMultiplication(m_Matrix[lCol][idxMult], multiplicationMatrix[lRow][idxMult]);

                if (idxMult == 0)
                    lFinalValue = lCurrentMultValue;
                else
                    lFinalValue ^ lCurrentMultValue;
            }
        }
    }

}

std::string CStateMatrix::ToString()
{
    std::string sRetorno;

    sRetorno = CAESUtils::LongToHex(m_Matrix[0][0]) + " " + CAESUtils::LongToHex(m_Matrix[1][0]) + CAESUtils::LongToHex(m_Matrix[2][0]) + " " + CAESUtils::LongToHex(m_Matrix[3][0]) + "\n" +
               CAESUtils::LongToHex(m_Matrix[0][1]) + " " + CAESUtils::LongToHex(m_Matrix[1][1]) + CAESUtils::LongToHex(m_Matrix[2][1]) + " " + CAESUtils::LongToHex(m_Matrix[3][1]) + "\n" +
               CAESUtils::LongToHex(m_Matrix[0][2]) + " " + CAESUtils::LongToHex(m_Matrix[1][2]) + CAESUtils::LongToHex(m_Matrix[2][2]) + " " + CAESUtils::LongToHex(m_Matrix[3][2]) + "\n" +
               CAESUtils::LongToHex(m_Matrix[0][3]) + " " + CAESUtils::LongToHex(m_Matrix[1][3]) + CAESUtils::LongToHex(m_Matrix[2][3]) + " " + CAESUtils::LongToHex(m_Matrix[3][3]) + "\n";

    return sRetorno;
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
