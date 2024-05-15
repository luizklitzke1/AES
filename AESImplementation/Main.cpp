#pragma once

#include <iostream>
#include "stdio.h"

#include <vector>
#include <string>
#include <sstream>

#include "AESConstants.h"
#include "AESUtils.h"
#include "StateMatrix.h"

using namespace std;

int main()
{
    stringstream sKey("65, 66 , 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80");
    
    string sSegment;
    vector<string> aKeySegments;
    while (getline(sKey, sSegment, ',')){ aKeySegments.push_back(sSegment);}

    vector<long> aKey;
    aKey.reserve(aKeySegments.size());

    for (const string sKeySegment : aKeySegments)
    {
        if (IsNumber(sKeySegment) == false)
        {
            cout << "Valor não número informado para um segmento da chave." << endl;
            return 1;
        }

        long lValue = stol(sKeySegment);
        if (lValue > 255)
        {
            cout << "Valor superior a 255 informado para um segmento da chave." << endl;
            return 1;
        }

        aKey.push_back(lValue);
    }

#pragma region Expansão de Chave

    CStateMatrix aKeySchedule[ROUND_KEY_AMOUNT];

    //Copia chave para primeira round key
    CStateMatrix& firstKey = aKeySchedule[0];
    for (size_t idxWord = 0; idxWord < WORDS_PER_STATE; ++idxWord)
        firstKey.SetWordFromArray(aKey, idxWord);

    //Gerar demais round key
    for (size_t idxRoundKey = 1; idxRoundKey < ROUND_KEY_AMOUNT; ++idxRoundKey)
    {
        CStateMatrix roundKey;

#pragma region Gerar primeira Word da RoundKey
        //1 - Copia última palavra da round key anterior
        WORD firstWord = aKeySchedule[idxRoundKey - 1].m_Matrix[WORDS_PER_STATE - 1];
        
        //2 - Rotacionar os bytes
        firstWord = RotWord(firstWord);

        //3 - Substituição de palavra com a SBOX
        firstWord = SubBytes(firstWord);

        //4 - Geração da RoundConstant
        const WORD roundConstant = { ROUND_CONSTANTS[idxRoundKey - 1] , 0x00, 0x00, 0x00 };

        //5 - XOR das etapas (3) e (4) 
        firstWord = XORWords(firstWord, roundConstant);
#pragma endregion

    }

#pragma endregion

   
}