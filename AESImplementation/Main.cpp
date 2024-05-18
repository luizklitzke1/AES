#pragma once

#include <iostream>
#include "stdio.h"

#include <vector>
#include <string>
#include <sstream>

#include "AESConstants.h"
#include "AESUtils.h"
#include "StateMatrix.h"

#include <conio.h>
#include "windows.h"

using namespace std;

int main()
{
    SetConsoleCP      (1252);
    SetConsoleOutputCP(1252);

    bool bGeraLog = true;

    stringstream sKey("65, 66 , 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80");
    
    string sSegment;
    vector<string> aKeySegments;
    while (getline(sKey, sSegment, ','))
    { 
        sSegment.erase(std::remove_if(sSegment.begin(), sSegment.end(), ::isspace), sSegment.end());
        aKeySegments.push_back(sSegment);
    }

    vector<long> aKey;
    aKey.reserve(aKeySegments.size());

    for (string& sKeySegment : aKeySegments)
    {
        if (CAESUtils::IsNumber(sKeySegment) == false)
        {
            cout << "Valor não númerico informado para um segmento da chave: " << sKeySegment << endl;
            return 1;
        }

        long lValor = stol(sKeySegment);
        if (lValor > 255)
        {
            cout << "Valor superior a 255 informado para um segmento da chave: " << sKeySegment << endl;
            return 1;
        }

        aKey.push_back(lValor);
    }

#pragma region Expansão de Chave

    CStateMatrix aKeySchedule[ROUND_KEY_AMOUNT];

    //Copia chave para primeira round key
    CStateMatrix& primeiraKey = aKeySchedule[0];
    for (ULONG idxWord = 0; idxWord < WORDS_PER_STATE; ++idxWord)
        primeiraKey.SetWordFromArray(aKey, idxWord);

    //Gerar demais round key
    for (ULONG idxRoundKey = 1; idxRoundKey < ROUND_KEY_AMOUNT; ++idxRoundKey)
    {
        if (bGeraLog) cout << "\n**** RoundKey = " << idxRoundKey << " ****" << endl;

        CStateMatrix& roundKey = aKeySchedule[idxRoundKey];

#pragma region Gerar primeira Word da RoundKey
        //1 - Copia última palavra da round key anterior
        AESWORD primeiraWord = aKeySchedule[idxRoundKey - 1][WORDS_PER_STATE - 1];
        if (bGeraLog) cout << "1) Cópia da última palavra da roundkey anterior: " + CAESUtils::WordToString(primeiraWord) << endl;
        
        //2 - Rotacionar os bytes
        primeiraWord = CAESUtils::RotWord(primeiraWord);
        if (bGeraLog) cout << "2) Rotacionar os bytes desta palavra (RotWord): " + CAESUtils::WordToString(primeiraWord) << endl;

        //3 - Substituição de palavra com a SBOX
        primeiraWord = CAESUtils::SubWord(primeiraWord);
        if (bGeraLog) cout << "3) Substituir os bytes da palavra (SubWord): " + CAESUtils::WordToString(primeiraWord) << endl;

        //4 - Geração da RoundConstant
        const AESWORD roundConstant = { ROUND_CONSTANTS[idxRoundKey - 1] , 0x00, 0x00, 0x00 };
        if (bGeraLog) cout << "4) Gerar a RoundConstant: " + CAESUtils::WordToString(roundConstant) << endl;

        //5 - XOR das etapas (3) e (4) 
        primeiraWord = CAESUtils::XORWords(primeiraWord, roundConstant);
        if (bGeraLog) cout << "5) XOR de (3) com (4): " + CAESUtils::WordToString(primeiraWord) << endl;

        //6 - XOR de (5) com a 1ª palavra da roundkey anterior
        primeiraWord = CAESUtils::XORWords(primeiraWord, aKeySchedule[idxRoundKey - 1][0]);
        if (bGeraLog) cout << "6) XOR 1a. palavra da roundkey anterior com (5): " + CAESUtils::WordToString(primeiraWord) << endl;

        roundKey[0] = primeiraWord;

#pragma endregion

#pragma region Gerar demais words da RoundKey
        for (ULONG idxWord = 1; idxWord < ROUND_KEY_SIZE; ++idxWord)
        {
            //XOR com a palavra imediatamente anterior e a palavra de posição equivalente na round key anterior.
            roundKey[idxWord] = CAESUtils::XORWords(roundKey[idxWord - 1], aKeySchedule[idxRoundKey - 1][idxWord]);

            if (bGeraLog) cout << "W" << ROUND_KEY_SIZE * idxRoundKey + idxWord << " = W" << ROUND_KEY_SIZE * idxRoundKey + idxWord - 1 << " XOR W" << ROUND_KEY_SIZE * (idxRoundKey - 1) + idxWord << endl;
        }
#pragma endregion

        if (bGeraLog) cout << endl << roundKey.ToString() << endl;
    }

#pragma endregion

#pragma region Criptografar dados
    vector<long> aTextoSimples = { 0x44, 0x45, 0x53, 0x45, 0x4e, 0x56, 0x4f, 0x4c, 0x56, 0x49, 0x4d, 0x45, 0x4e, 0x54, 0x4f, 0x21 };

    const ULONG ulRestoDivisao = aTextoSimples.size() % BLOCK_SIZE;
    const ULONG ulPaddingSize  = ulRestoDivisao == 0 ? BLOCK_SIZE : BLOCK_SIZE - ulRestoDivisao;

    aTextoSimples.insert(aTextoSimples.end(), ulPaddingSize, ulPaddingSize);

    const ULONG ulQuantidadeBlocos = aTextoSimples.size() / BLOCK_SIZE;

    ULONG ulContadorChar = 0;
    for (ULONG idxBloco = 0; idxBloco < ulQuantidadeBlocos; ++idxBloco)
    {
        CStateMatrix matrizEstadoBloco;

        for (ULONG idxAdicao = 0; idxAdicao < BLOCK_SIZE; ++idxAdicao, ++ulContadorChar)
            matrizEstadoBloco.AdicionaValor(aTextoSimples[ulContadorChar]);

        if (bGeraLog) cout << "**** Texto simples **** \n" + matrizEstadoBloco.ToString() << endl;

        // Etapa 1 – XOR(textoSimples, RoundKey(0))
        matrizEstadoBloco = CAESUtils::XORStates(matrizEstadoBloco, aKeySchedule[0]);
        if (bGeraLog) cout << "**** AddRoundKey-Round 0 ****\n" + matrizEstadoBloco.ToString() << endl;
        
        for (ULONG idxRodada = 1; idxRodada < ROUND_KEY_AMOUNT; ++idxRodada)
        {
            //Etapa 2 - SubBytes
            matrizEstadoBloco.SubBytes();
            if (bGeraLog) cout << "**** SubBytes-Round " << idxRodada << " ****\n" + matrizEstadoBloco.ToString() << endl;

            //Etapa 3 - ShiftLinhas
            matrizEstadoBloco.ShiftLinhas();
            if (bGeraLog) cout << "**** ShiftLinhas-Round " << idxRodada << " ****\n" + matrizEstadoBloco.ToString() << endl;

            //Etapa 4 - MixColumns
            if (idxRodada != ROUND_KEY_AMOUNT - 1)
            {
                matrizEstadoBloco.MixColumns();
                if (bGeraLog) cout << "**** MixColumns-Round " << idxRodada << " ****\n" + matrizEstadoBloco.ToString() << endl;
            }

            //Etapa 5 - AddRoundKey
            matrizEstadoBloco = CAESUtils::XORStates(matrizEstadoBloco, aKeySchedule[idxRodada]);
            if (bGeraLog) cout << "**** AddRoundKey-Round " << idxRodada << " ****\n" + matrizEstadoBloco.ToString() << endl;
        }

        if (bGeraLog) cout << "**** Texto cifrado ****\n" + matrizEstadoBloco.ToString() << endl;
    }
#pragma endregion
}