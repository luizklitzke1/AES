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
    bool bGenerateLog = true;

    stringstream sKey("65, 66 , 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80");
    
    string sSegment;
    vector<string> aKeySegments;
    while (getline(sKey, sSegment, ',')){ aKeySegments.push_back(sSegment);}

    vector<long> aKey;
    aKey.reserve(aKeySegments.size());

    for (const string sKeySegment : aKeySegments)
    {
        if (CAESUtils::IsNumber(sKeySegment) == false)
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
        if (bGenerateLog) cout << "\n**** RoundKey = " << idxRoundKey << " ****" << endl;

        CStateMatrix roundKey;

#pragma region Gerar primeira Word da RoundKey
        //1 - Copia última palavra da round key anterior
        WORD& firstWord = aKeySchedule[idxRoundKey - 1][WORDS_PER_STATE - 1];
        if (bGenerateLog) cout << "1) Cópia da última palavra da roundkey anterior: " + CAESUtils::WordToString(firstWord) << endl;
        
        //2 - Rotacionar os bytes
        firstWord = CAESUtils::RotWord(firstWord);
        if (bGenerateLog) cout << "2) Rotacionar os bytes desta palavra (RotWord): " + CAESUtils::WordToString(firstWord) << endl;

        //3 - Substituição de palavra com a SBOX
        firstWord = CAESUtils::SubWord(firstWord);
        if (bGenerateLog) cout << "3) Substituir os bytes da palavra (SubWord): " + CAESUtils::WordToString(firstWord) << endl;

        //4 - Geração da RoundConstant
        const WORD roundConstant = { ROUND_CONSTANTS[idxRoundKey - 1] , 0x00, 0x00, 0x00 };
        if (bGenerateLog) cout << "4) Gerar a RoundConstant: " + CAESUtils::WordToString(roundConstant) << endl;

        //5 - XOR das etapas (3) e (4) 
        firstWord = CAESUtils::XORWords(firstWord, roundConstant);
        if (bGenerateLog) cout << "5) XOR de (3) com (4): " + CAESUtils::WordToString(firstWord) << endl;

        //6 - XOR de (5) com a 1ª palavra da roundkey anterior
        firstWord = CAESUtils::XORWords(firstWord, aKeySchedule[idxRoundKey - 1][0]);
        if (bGenerateLog) cout << "6) XOR 1a. palavra da roundkey anterior com (5): " + CAESUtils::WordToString(firstWord) << endl;

#pragma endregion

#pragma region Gerar demais words da RoundKey
        for (size_t idxWord = 1; idxWord < ROUND_KEY_SIZE; ++idxWord)
        {
            //XOR com a palavra imediatamente anterior e a palavra de posição equivalente na round key anterior.
            roundKey[idxWord] = CAESUtils::XORWords(roundKey[idxWord - 1], aKeySchedule[idxRoundKey - 1][idxWord]);
        }
#pragma endregion

    }

#pragma endregion

   
}