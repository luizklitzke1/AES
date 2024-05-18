#pragma once
#include <stdexcept>
#include <iostream>

#include "StateMatrix.h"
#include "AESUtils.h"

using namespace std;

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
    std::array<AESWORD, WORDS_PER_STATE> matrixAux;
    //Prestar atenção que aqui a visualização de index fica trocada, porque consideramos uma word uma coluna no visual, (X, Y) no slide aqui é (Y, X) :)
    matrixAux[0] = { m_Matrix[0][0], m_Matrix[1][1], m_Matrix[2][2], m_Matrix[3][3] };
    matrixAux[1] = { m_Matrix[1][0], m_Matrix[2][1], m_Matrix[3][2], m_Matrix[0][3] };
    matrixAux[2] = { m_Matrix[2][0], m_Matrix[3][1], m_Matrix[0][2], m_Matrix[1][3] };
    matrixAux[3] = { m_Matrix[3][0], m_Matrix[0][1], m_Matrix[1][2], m_Matrix[2][3] };

    m_Matrix = matrixAux;
}

void CStateMatrix::MixColumns()
{
    long multiplicationMatrix[4][4] = { { 2, 3, 1, 1 },
                                        { 1, 2, 3, 1 },
                                        { 1, 1, 2, 3 },
                                        { 3, 1, 1, 2 }};

    //Matriz que armazena o resultado final dessa função
    CStateMatrix resultMatrix;

    for (size_t idxWord = 0; idxWord < WORDS_PER_STATE; ++idxWord)
    {
        const long lCol = idxWord;

        for (size_t idxValue = 0; idxValue < WORD_LENGTH; ++idxValue)
        {
            //cout << "b" << idxValue + 1 + lCol * WORD_LENGTH << " = ";

            const long lRow = idxValue;

            long lFinalValue = 0;

            for (size_t idxMult = 0; idxMult < WORD_LENGTH; ++idxMult)
            {
                long lCurrentMultValue = CAESUtils::GaloisFieldMultiplication(m_Matrix[lCol][idxMult], multiplicationMatrix[lRow][idxMult]);
                //out << " (r" << lCol * WORD_LENGTH + idxMult + 1 << " [ " << CAESUtils::LongToHex(m_Matrix[lCol][idxMult]) << "] * " << multiplicationMatrix[lRow][idxMult] << ")[" << lCurrentMultValue << "] ";

                if (idxMult == 0)
                    lFinalValue = lCurrentMultValue;
                else
                    lFinalValue = lFinalValue ^ lCurrentMultValue;
            }

            resultMatrix[idxWord][idxValue] = lFinalValue;
        }
    }

    m_Matrix = resultMatrix.m_Matrix;
}

void CStateMatrix::AdicionaValor(const long lValor)
{
    m_Matrix[m_ulWord][m_ulPoximaPosicao] = lValor;

    ++m_ulPoximaPosicao;
    if (m_ulPoximaPosicao == 4)
    {
        m_ulPoximaPosicao = 0;
        ++m_ulWord;
    }
}

std::string CStateMatrix::ToString()
{
    std::string sRetorno;

    sRetorno = CAESUtils::LongToHex(m_Matrix[0][0]) + " " + CAESUtils::LongToHex(m_Matrix[1][0]) + " " + CAESUtils::LongToHex(m_Matrix[2][0]) + " " + CAESUtils::LongToHex(m_Matrix[3][0]) + "\n" +
               CAESUtils::LongToHex(m_Matrix[0][1]) + " " + CAESUtils::LongToHex(m_Matrix[1][1]) + " " + CAESUtils::LongToHex(m_Matrix[2][1]) + " " + CAESUtils::LongToHex(m_Matrix[3][1]) + "\n" +
               CAESUtils::LongToHex(m_Matrix[0][2]) + " " + CAESUtils::LongToHex(m_Matrix[1][2]) + " " + CAESUtils::LongToHex(m_Matrix[2][2]) + " " + CAESUtils::LongToHex(m_Matrix[3][2]) + "\n" +
               CAESUtils::LongToHex(m_Matrix[0][3]) + " " + CAESUtils::LongToHex(m_Matrix[1][3]) + " " + CAESUtils::LongToHex(m_Matrix[2][3]) + " " + CAESUtils::LongToHex(m_Matrix[3][3]) + "\n";

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
