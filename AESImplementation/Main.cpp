#pragma once

#include <iostream>
#include "stdio.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

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

#pragma region Input Inicial
    std::cout << "[CRIPTOGRAFIA AES]" << endl;
    std::cout << "Alunos: Arthur B. Pinotti, Kaue Reblin, Luiz G. Klitzke" << endl;

    //Permitir que o usuário informe um arquivo a ser criptografado. O programa deverá suportar qualquer arquivo (texto ou binário) e de qualquer tamanho;
    string aTextoSimples;
    {
        std::cout << "Informe o caminho do arquivo para ser criptografado: ";
        string sFilePath;
        cin >> sFilePath;

        ifstream inputFile;
        inputFile.open(sFilePath);

        if (inputFile.fail())
        {
            std::cout << "Erro ao abrir o arquivo: " << sFilePath << endl;
            return 1;
        }

        while (!inputFile.eof())
        {
            string sAux;
            getline(inputFile, sAux);
            aTextoSimples.append(sAux);
        }

        inputFile.close();
        if (inputFile.fail())
        {
            std::cout << "Erro ao fechar o arquivo: " << sFilePath << endl;
            return 1;
        }
    }

    //Permitir que o usuário possa informar o nome do arquivo de destino a ser gerado;
    string sNomeArquivoResult;
    FILE* fpResult = nullptr;
    {
        std::cout << "Informe o caminho do arquivo a ser gerado (Caso já existir, será destruído): ";
        cin >> sNomeArquivoResult;

        //Abre um arquivo vazio para gravação. Se o arquivo determinado existir, seus conteúdos são destruídos.
        errno_t err = fopen_s(&fpResult, sNomeArquivoResult.c_str(), "w");
        if (err != 0 || fpResult == nullptr)
        {
            std::cout << "Erro ao abrir o arquivo de resultado: " << err << endl;
            return 1;
        }
    }
    
    //Permitir que o usuário forneça a chave de criptografia.
    //Deve ser um campo texto em que possa ser fornecido os valores dos bytes da chave em formato decimal, separando - os por vírgula
    vector<long> aKey;
    {
        std::cout << "Informe a chave de criptografia de 128 bits: ";

        string sKeyInput;
        cin.ignore();
        getline(cin, sKeyInput);

        stringstream sKey(sKeyInput);
        string sSegment;
        vector<string> aKeySegments;
        while (getline(sKey, sSegment, ','))
        {
            sSegment.erase(std::remove_if(sSegment.begin(), sSegment.end(), ::isspace), sSegment.end());
            aKeySegments.push_back(sSegment);
        }

        aKey.reserve(aKeySegments.size());

        for (string& sKeySegment : aKeySegments)
        {
            if (CAESUtils::IsNumber(sKeySegment) == false)
            {
                std::cout << "Valor não númerico informado para um segmento da chave: " << sKeySegment << endl;
                return 1;
            }

            long lValor = stol(sKeySegment);
            if (lValor > 255)
            {
                std::cout << "Valor superior a 255 informado para um segmento da chave: " << sKeySegment << endl;
                return 1;
            }

            aKey.push_back(lValor);
        }

        if (aKey.size() != 16)
        {
            std::cout << "A chave informada não é de 128bits!" << endl;
            return 1;
        }
    }

    FILE* fpLog = nullptr;
    bool bGeraLog = true;
    //Possibilitar logar os passos
    {
        std::cout << "Gerar log do processo? (Y - Sim): ";
        char cLog;
        cin >> cLog;

        bGeraLog = toupper(cLog) == 'Y';
    }

    if (bGeraLog)
    {
        errno_t err = fopen_s(&fpLog, "log.txt", "a");
        if (err != 0 || fpLog == nullptr)
        {
            std::cout << "Erro ao abrir o arquivo de log: "  << err << endl;
            return 1;
        }

        fprintf(fpLog, "Executado em: %s\n", CAESUtils::GetTimeString().c_str());
    }
#pragma endregion

#pragma region Expansão de Chave

    CStateMatrix aKeySchedule[ROUND_KEY_AMOUNT];

    //Copia chave para primeira round key
    CStateMatrix& primeiraKey = aKeySchedule[0];
    for (ULONG idxWord = 0; idxWord < WORDS_PER_STATE; ++idxWord)
        primeiraKey.SetWordFromArray(aKey, idxWord);

    //Gerar demais round key
    for (ULONG idxRoundKey = 1; idxRoundKey < ROUND_KEY_AMOUNT; ++idxRoundKey)
    {
        if (bGeraLog) fprintf(fpLog, "\n**** RoundKey = %d  ****\n", idxRoundKey);

        CStateMatrix& roundKey = aKeySchedule[idxRoundKey];

#pragma region Gerar primeira Word da RoundKey
        //1 - Copia última palavra da round key anterior
        AESWORD primeiraWord = aKeySchedule[idxRoundKey - 1][WORDS_PER_STATE - 1];
        if (bGeraLog) fprintf(fpLog, "1) Cópia da última palavra da roundkey anterior: %s\n", CAESUtils::WordToString(primeiraWord).c_str());
        
        //2 - Rotacionar os bytes
        primeiraWord = CAESUtils::RotWord(primeiraWord);
        if (bGeraLog) fprintf(fpLog, "2) Rotacionar os bytes desta palavra (RotWord): %s\n", CAESUtils::WordToString(primeiraWord).c_str());

        //3 - Substituição de palavra com a SBOX
        primeiraWord = CAESUtils::SubWord(primeiraWord);
        if (bGeraLog) fprintf(fpLog, "3) Substituir os bytes da palavra (SubWord): %s\n", CAESUtils::WordToString(primeiraWord).c_str());

        //4 - Geração da RoundConstant
        const AESWORD roundConstant = { ROUND_CONSTANTS[idxRoundKey - 1] , 0x00, 0x00, 0x00 };
        if (bGeraLog) fprintf(fpLog, "4) Gerar a RoundConstant: %s\n", CAESUtils::WordToString(roundConstant).c_str());

        //5 - XOR das etapas (3) e (4) 
        primeiraWord = CAESUtils::XORWords(primeiraWord, roundConstant);
        if (bGeraLog) fprintf(fpLog, "5) XOR de (3) com (4): %s\n", CAESUtils::WordToString(primeiraWord).c_str());

        //6 - XOR de (5) com a 1ª palavra da roundkey anterior
        primeiraWord = CAESUtils::XORWords(primeiraWord, aKeySchedule[idxRoundKey - 1][0]);
        if (bGeraLog) fprintf(fpLog, "6) XOR 1a. palavra da roundkey anterior com (5): %s\n", CAESUtils::WordToString(primeiraWord).c_str());

        roundKey[0] = primeiraWord;

#pragma endregion

#pragma region Gerar demais words da RoundKey
        for (ULONG idxWord = 1; idxWord < ROUND_KEY_SIZE; ++idxWord)
        {
            //XOR com a palavra imediatamente anterior e a palavra de posição equivalente na round key anterior.
            roundKey[idxWord] = CAESUtils::XORWords(roundKey[idxWord - 1], aKeySchedule[idxRoundKey - 1][idxWord]);

            if (bGeraLog) fprintf(fpLog, "W %d  = W %d XOR W%d\n", ROUND_KEY_SIZE * idxRoundKey + idxWord, ROUND_KEY_SIZE * idxRoundKey + idxWord - 1, ROUND_KEY_SIZE * (idxRoundKey - 1) + idxWord);
        }
#pragma endregion

        if (bGeraLog) fprintf(fpLog, "\n%s\n", roundKey.ToString().c_str());
    }

#pragma endregion

#pragma region Criptografar dados
    const ULONG ulRestoDivisao = aTextoSimples.size() % BLOCK_SIZE;
    const ULONG ulPaddingSize  = ulRestoDivisao == 0 ? BLOCK_SIZE : BLOCK_SIZE - ulRestoDivisao;

    aTextoSimples.insert(aTextoSimples.end(), ulPaddingSize, ulPaddingSize);

    const ULONG ulQuantidadeBlocos = aTextoSimples.size() / BLOCK_SIZE;

    string sTextoCifrado;

    ULONG ulContadorChar = 0;
    for (ULONG idxBloco = 0; idxBloco < ulQuantidadeBlocos; ++idxBloco)
    {
        CStateMatrix matrizEstadoBloco;

        for (ULONG idxAdicao = 0; idxAdicao < BLOCK_SIZE; ++idxAdicao, ++ulContadorChar)
            matrizEstadoBloco.AdicionaValor(aTextoSimples[ulContadorChar]);

        if (bGeraLog) fprintf(fpLog, "**** Texto simples **** \n%s\n", matrizEstadoBloco.ToString().c_str());

        // Etapa 1 – XOR(textoSimples, RoundKey(0))
        matrizEstadoBloco = CAESUtils::XORStates(matrizEstadoBloco, aKeySchedule[0]);
        if (bGeraLog) fprintf(fpLog, "**** AddRoundKey-Round 0 ****\n%s\n", matrizEstadoBloco.ToString().c_str());
        
        for (ULONG idxRodada = 1; idxRodada < ROUND_KEY_AMOUNT; ++idxRodada)
        {
            //Etapa 2 - SubBytes
            matrizEstadoBloco.SubBytes();
            if (bGeraLog) fprintf(fpLog, "**** SubBytes-Round %d ****\n%s\n", idxRodada, matrizEstadoBloco.ToString().c_str());

            //Etapa 3 - ShiftLinhas
            matrizEstadoBloco.ShiftLinhas();
            if (bGeraLog) fprintf(fpLog, "**** ShiftLinhas-Round %d ****\n%s\n", idxRodada, matrizEstadoBloco.ToString().c_str());

            //Etapa 4 - MixColumns
            if (idxRodada != ROUND_KEY_AMOUNT - 1)
            {
                matrizEstadoBloco.MixColumns();
                if (bGeraLog) fprintf(fpLog, "**** MixColumns-Round %d ****\n%s\n", idxRodada, matrizEstadoBloco.ToString().c_str());
            }

            //Etapa 5 - AddRoundKey
            matrizEstadoBloco = CAESUtils::XORStates(matrizEstadoBloco, aKeySchedule[idxRodada]);
            if (bGeraLog) fprintf(fpLog, "**** AddRoundKey-Round %d ****\n%s\n", idxRodada, matrizEstadoBloco.ToString().c_str());
        }

        if (bGeraLog) fprintf(fpLog, "**** Texto cifrado Bloco = %d **** \n%s\n", idxBloco, matrizEstadoBloco.ToString().c_str());;

        sTextoCifrado.append(matrizEstadoBloco.ToCharArray());
    }

    fprintf(fpResult, sTextoCifrado.c_str());

    if (fclose(fpResult))
    {
        std::cout << "Erro ao fechar o arquivo de resultado." << endl;
        return 1;
    }

    if (fpLog != nullptr && fclose(fpLog))
    {
        std::cout << "Erro ao fechar o arquivo de log." << endl;
        return 1;
    }
#pragma endregion
}