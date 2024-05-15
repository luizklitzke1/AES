#include "Word.h"
#include <stdexcept>

CWord::CWord()
{
    m_aValues = { 0 };
}

CWord::CWord(WORD_ARRAY aValues)
{
    m_aValues = aValues;
}

void CWord::Rotate()
{
    WORD_ARRAY tempRotatedWord = { m_aValues[1], m_aValues[2], m_aValues[3], m_aValues[0] };
    m_aValues = tempRotatedWord;
}

long& CWord::operator[](std::size_t index)
{
    if (index >= WORD_LENGTH)
        throw std::out_of_range("Index out of range");

    return m_aValues[index];
}

const long& CWord::operator[](std::size_t index) const
{
    if (index >= WORD_LENGTH)
        throw std::out_of_range("Index out of range");

    return m_aValues[index];
}