#pragma once

#include <array>

using ULONG = unsigned long;

const long ROUND_CONSTANTS[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

const long WORD_LENGTH     = 4;
const long WORDS_PER_STATE = 4;

using AESWORD = std::array<long, WORD_LENGTH>;

const long ROUND_KEY_AMOUNT = 11;
const long ROUND_KEY_SIZE   = 4;

const long S_BOX_DIM   = 16;
const long E_TABLE_DIM = 16;
const long L_TABLE_DIM = 16;

const long BLOCK_SIZE = WORD_LENGTH * WORDS_PER_STATE;
