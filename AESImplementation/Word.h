#pragma once
#include <array>

#include "AESConstants.h"

class CWord
{
public:
    CWord();
    CWord(WORD_ARRAY aValues);

    void Rotate();
    CWord SubWord();

    WORD_ARRAY m_aValues;

public:
    long& operator[](std::size_t index);
    const long& operator[](std::size_t index) const;
};

