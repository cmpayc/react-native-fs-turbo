/*
 * MIT License
 *
 * Copyright (c) 2019 SergeyBel
 * Copyright (c) 2023 Jubal Mordecai Velasco
 * Copyright (c) 2025 Sergei Kazakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef RNFSTURBO_USE_ENCRYPTION

#ifndef KRYPT_AES_CPP
#define KRYPT_AES_CPP

#include "AES.hpp"

namespace Krypt {
    namespace BlockCipher {
        void AES::AddRoundKey(unsigned char state[16], unsigned char *key) {
            state[0 * 4 + 0] = state[0 * 4 + 0] ^ key[0];
            state[0 * 4 + 1] = state[0 * 4 + 1] ^ key[4];
            state[0 * 4 + 2] = state[0 * 4 + 2] ^ key[8];
            state[0 * 4 + 3] = state[0 * 4 + 3] ^ key[12];

            state[1 * 4 + 0] = state[1 * 4 + 0] ^ key[1];
            state[1 * 4 + 1] = state[1 * 4 + 1] ^ key[5];
            state[1 * 4 + 2] = state[1 * 4 + 2] ^ key[9];
            state[1 * 4 + 3] = state[1 * 4 + 3] ^ key[13];

            state[2 * 4 + 0] = state[2 * 4 + 0] ^ key[2];
            state[2 * 4 + 1] = state[2 * 4 + 1] ^ key[6];
            state[2 * 4 + 2] = state[2 * 4 + 2] ^ key[10];
            state[2 * 4 + 3] = state[2 * 4 + 3] ^ key[14];

            state[3 * 4 + 0] = state[3 * 4 + 0] ^ key[3];
            state[3 * 4 + 1] = state[3 * 4 + 1] ^ key[7];
            state[3 * 4 + 2] = state[3 * 4 + 2] ^ key[11];
            state[3 * 4 + 3] = state[3 * 4 + 3] ^ key[15];
        }

        void AES::EncryptBlock(Bytes *plain, Bytes *cipher) {
#ifdef USE_AESNI
            // load the current block & current round key into the registers
            __m128i state = _mm_loadu_si128((__m128i *) &plain[0]);

            // original key
            state = _mm_xor_si128(state, RoundedKeys[0]);

            // perform usual rounds
            for (size_t i = 1; i < Nr - 1; i += 2) {
                state = _mm_aesenc_si128(state, RoundedKeys[i]);
                state = _mm_aesenc_si128(state, RoundedKeys[i + 1]);
            }

            // last round
            state = _mm_aesenc_si128(state, RoundedKeys[Nr - 1]);
            state = _mm_aesenclast_si128(state, RoundedKeys[Nr]);

            // store from register to array
            _mm_storeu_si128((__m128i *) (cipher), state);
#elif defined(USE_ARM_AES) && defined(__aarch64__)
            uint8x16_t state = vld1q_u8(plain);

            // Initial round
            state = vaesmcq_u8(vaeseq_u8(state, RoundedKeys[0]));

            // 8 main rounds
            for (size_t i = 1; i < Nr - 1; i += 2) {
                state = vaesmcq_u8(vaeseq_u8(state, RoundedKeys[i]));
                state = vaesmcq_u8(vaeseq_u8(state, RoundedKeys[i + 1]));
            }

            // last 2 final round
            state = vaeseq_u8(state, RoundedKeys[Nr - 1]);
            state = veorq_u8(state, RoundedKeys[Nr]);

            // store the result to cipher
            vst1q_u8(cipher, state);
#else
            Bytes state[16];

            state[0 * 4 + 0] = plain[0];
            state[0 * 4 + 1] = plain[4];
            state[0 * 4 + 2] = plain[8];
            state[0 * 4 + 3] = plain[12];

            state[1 * 4 + 0] = plain[1];
            state[1 * 4 + 1] = plain[5];
            state[1 * 4 + 2] = plain[9];
            state[1 * 4 + 3] = plain[13];

            state[2 * 4 + 0] = plain[2];
            state[2 * 4 + 1] = plain[6];
            state[2 * 4 + 2] = plain[10];
            state[2 * 4 + 3] = plain[14];

            state[3 * 4 + 0] = plain[3];
            state[3 * 4 + 1] = plain[7];
            state[3 * 4 + 2] = plain[11];
            state[3 * 4 + 3] = plain[15];

            AddRoundKey(state, RoundedKeys);

            for (uint8_t round = 1; round <= Nr - 1; round++) {
                SubBytes(state);
                ShiftRows(state);
                MixColumns(state);
                AddRoundKey(state, RoundedKeys + round * 4 * Nb);
            }

            SubBytes(state);
            ShiftRows(state);
            AddRoundKey(state, RoundedKeys + Nr * 4 * Nb);

            cipher[0] = state[0 * 4 + 0];
            cipher[4] = state[0 * 4 + 1];
            cipher[8] = state[0 * 4 + 2];
            cipher[12] = state[0 * 4 + 3];

            cipher[1] = state[1 * 4 + 0];
            cipher[5] = state[1 * 4 + 1];
            cipher[9] = state[1 * 4 + 2];
            cipher[13] = state[1 * 4 + 3];

            cipher[2] = state[2 * 4 + 0];
            cipher[6] = state[2 * 4 + 1];
            cipher[10] = state[2 * 4 + 2];
            cipher[14] = state[2 * 4 + 3];

            cipher[3] = state[3 * 4 + 0];
            cipher[7] = state[3 * 4 + 1];
            cipher[11] = state[3 * 4 + 2];
            cipher[15] = state[3 * 4 + 3];
#endif
        }

        void AES::DecryptBlock(Bytes *cipher, Bytes *recover) {
#ifdef USE_AESNI
            // load the current block & current round key into the registers
            __m128i state = _mm_loadu_si128((__m128i *) &cipher[0]);

            // first round
            state = _mm_xor_si128(state, DecryptionRoundedKeys[Nr]);

            // usual rounds
            for (size_t i = Nr - 1; i > 1; i -= 2) {
                state = _mm_aesdec_si128(state, DecryptionRoundedKeys[i]);
                state = _mm_aesdec_si128(state, DecryptionRoundedKeys[i - 1]);
            }

            // last round
            state = _mm_aesdec_si128(state, DecryptionRoundedKeys[1]);
            state = _mm_aesdeclast_si128(state, DecryptionRoundedKeys[0]);

            // store from register to array
            _mm_storeu_si128((__m128i *) recover, state);
#elif defined(USE_ARM_AES) && defined(__aarch64__)
            uint8x16_t state = vld1q_u8(cipher);

            // Initial round
            state = vaesimcq_u8(vaesdq_u8(state, DecryptionRoundedKeys[Nr]));

            // 8 main rounds
            for (size_t i = Nr - 1; i > 1; i -= 2) {
                state = vaesimcq_u8(vaesdq_u8(state, DecryptionRoundedKeys[i]));
                state = vaesimcq_u8(vaesdq_u8(state, DecryptionRoundedKeys[i - 1]));
            }

            // final 2 rounds
            state = vaesdq_u8(state, DecryptionRoundedKeys[1]);
            state = veorq_u8(state, DecryptionRoundedKeys[0]);

            // store the result to recover
            vst1q_u8(recover, state);
#else
            Bytes state[16];

            state[0 * 4 + 0] = cipher[0];
            state[0 * 4 + 1] = cipher[4];
            state[0 * 4 + 2] = cipher[8];
            state[0 * 4 + 3] = cipher[12];
            state[1 * 4 + 0] = cipher[1];
            state[1 * 4 + 1] = cipher[5];
            state[1 * 4 + 2] = cipher[9];
            state[1 * 4 + 3] = cipher[13];
            state[2 * 4 + 0] = cipher[2];
            state[2 * 4 + 1] = cipher[6];
            state[2 * 4 + 2] = cipher[10];
            state[2 * 4 + 3] = cipher[14];
            state[3 * 4 + 0] = cipher[3];
            state[3 * 4 + 1] = cipher[7];
            state[3 * 4 + 2] = cipher[11];
            state[3 * 4 + 3] = cipher[15];

            AddRoundKey(state, RoundedKeys + Nr * 4 * Nb);

            for (uint8_t round = Nr - 1; round >= 1; round--) {
                InvSubBytes(state);
                InvShiftRows(state);
                AddRoundKey(state, RoundedKeys + round * 4 * Nb);
                InvMixColumns(state);
            }

            InvSubBytes(state);
            InvShiftRows(state);
            AddRoundKey(state, RoundedKeys);

            recover[0] = state[0 * 4 + 0];
            recover[4] = state[0 * 4 + 1];
            recover[8] = state[0 * 4 + 2];
            recover[12] = state[0 * 4 + 3];
            recover[1] = state[1 * 4 + 0];
            recover[5] = state[1 * 4 + 1];
            recover[9] = state[1 * 4 + 2];
            recover[13] = state[1 * 4 + 3];
            recover[2] = state[2 * 4 + 0];
            recover[6] = state[2 * 4 + 1];
            recover[10] = state[2 * 4 + 2];
            recover[14] = state[2 * 4 + 3];
            recover[3] = state[3 * 4 + 0];
            recover[7] = state[3 * 4 + 1];
            recover[11] = state[3 * 4 + 2];
            recover[15] = state[3 * 4 + 3];

#endif
        }

        void AES::KeyExpansion(const Bytes *key) {
            Bytes *w = new Bytes[4 * Nb * (Nr + 1)];
            Bytes *temp = new Bytes[4];
            Bytes *rcon = new Bytes[4];

            size_t i = 0;
            while (i < 4 * Nk) {
                w[i] = key[i];
                i++;
            }

            i = 4 * Nk;
            while (i < 4 * Nb * (Nr + 1)) {
                temp[0] = w[i - 4 + 0];
                temp[1] = w[i - 4 + 1];
                temp[2] = w[i - 4 + 2];
                temp[3] = w[i - 4 + 3];

                if (i / 4 % Nk == 0) {
                    RotDWord(temp);
                    SubDWordBytes(temp);
                    Rcon(rcon, i / (Nk * 4));
                    XorDWords(temp, rcon, temp);
                } else if (Nk > 6 && i / 4 % Nk == 4) {
                    SubDWordBytes(temp);
                }

                w[i + 0] = w[i - 4 * Nk] ^ temp[0];
                w[i + 1] = w[i + 1 - 4 * Nk] ^ temp[1];
                w[i + 2] = w[i + 2 - 4 * Nk] ^ temp[2];
                w[i + 3] = w[i + 3 - 4 * Nk] ^ temp[3];
                i += 4;
            }

            delete[] rcon;
            delete[] temp;

#ifdef USE_AESNI
            size_t RoundKeySize = Nr + 1;
            RoundedKeys = new __m128i[RoundKeySize];
            DecryptionRoundedKeys = new __m128i[RoundKeySize];

            RoundedKeys[0] = _mm_loadu_si128((__m128i *) &w[0]);
            DecryptionRoundedKeys[0] = RoundedKeys[0];

            for (size_t i = 1; i < RoundKeySize; ++i) {
                RoundedKeys[i] = _mm_loadu_si128((__m128i *) &w[i * 4 * Nb]);
                DecryptionRoundedKeys[i] = _mm_aesimc_si128(RoundedKeys[i]);
            }

            RoundedKeys[Nr] = _mm_loadu_si128((__m128i *) &w[Nr * 4 * Nb]);
            DecryptionRoundedKeys[Nr] = RoundedKeys[Nr];

            delete[] w;
#elif defined(USE_ARM_AES) && defined(__aarch64__)
            size_t RoundKeySize = Nr + 1;
            RoundedKeys = new uint8x16_t[RoundKeySize];
            DecryptionRoundedKeys = new uint8x16_t[RoundKeySize];

            RoundedKeys[0] = vld1q_u8(&w[0]);
            DecryptionRoundedKeys[0] = vld1q_u8(&w[0]);

            for (size_t i = 1; i < RoundKeySize; ++i) {
                RoundedKeys[i] = vld1q_u8(&w[i * 4 * Nb]);
                DecryptionRoundedKeys[i] = vaesimcq_u8(RoundedKeys[i]);
            }

            RoundedKeys[Nr] = vld1q_u8(&w[Nr * 4 * Nb]);
            DecryptionRoundedKeys[Nr] = RoundedKeys[Nr];

            delete[] w;
#else
            RoundedKeys = w;
#endif
        }
    
        void AES::MixColumns(unsigned char state[16]) {
            unsigned char temp_state[4 * 4 + 4];

            memset(temp_state, 0x00, 4 * 4);

            for (size_t i = 0; i < 4; ++i) {
                for (size_t k = 0; k < 4; ++k) {
                    for (size_t j = 0; j < 4; ++j) {
                        if (CMDS[i][k] == 1) {
                            temp_state[i * 4 + j] ^= state[k * 4 + j];
                        } else {
                            temp_state[i * 4 + j] ^= GF_MUL_TABLE[CMDS[i][k]][state[k * 4 + j]];
                        }
                    }
                }
            }

            memcpy(state, temp_state, 4 * 4);
        }

        void AES::InvMixColumns(unsigned char state[16]) {
            unsigned char temp_state[4 * 4 + 4];

            memset(temp_state, 0x00, 4 * 4);

            for (size_t i = 0; i < 4; ++i) {
                for (size_t k = 0; k < 4; ++k) {
                    for (size_t j = 0; j < 4; ++j) {
                        temp_state[i * 4 + j] ^= GF_MUL_TABLE[INV_CMDS[i][k]][state[k * 4 + j]];
                    }
                }
            }

            memcpy(state, temp_state, 4 * 4);
        }
    
        void AES::setKey(const Bytes *ByteArray, size_t keyLen) {
            switch (keyLen) {
                case 16: // AES128
                    this->Nk = 4;
                    this->Nr = 10;
                    break;
                case 24: // AES192
                    this->Nk = 6;
                    this->Nr = 12;
                    break;
                case 32: // AES256
                    this->Nk = 8;
                    this->Nr = 14;
                    break;
                default:
                    throw std::invalid_argument("Incorrect key length");
            }

            KeyExpansion(ByteArray);
        }

        AES::AES(const Bytes *ByteArray, size_t keyLen) : BASE_BLOCKCIPHER(16) {
            setKey(ByteArray, keyLen);
        }

        AES::~AES() {
            if (RoundedKeys != NULL) {
                delete[] RoundedKeys;

#if defined(USE_AESNI) || (defined(USE_ARM_AES) && defined(__aarch64__))
                delete[] DecryptionRoundedKeys;
#endif
            }
        }
    
        void AES::ShiftRows(unsigned char state[16]) {
            // row 2
            unsigned char buffer = state[1 * 4 + 0];
            memmove(&state[4], &state[4] + 1, sizeof(unsigned char) * 3);
            state[1 * 4 + 3] = buffer;

            // row 3
            unsigned char thrid[2];
            memcpy(thrid, &state[8], sizeof(unsigned char) * 2);
            memcpy(&state[8], &state[8] + 2, sizeof(unsigned char) * 2);
            memcpy(&state[8] + 2, thrid, sizeof(unsigned char) * 2);

            // row 4
            buffer = state[3 * 4 + 3];
            memmove(&state[12] + 1, &state[12], sizeof(unsigned char) * 3);
            state[3 * 4 + 0] = buffer;

            /*
            unsigned char temp;

            //Row 2
            temp = state[1 * 4 + 0];
            state[1 * 4 + 0] = state[1 * 4 + 1];
            state[1 * 4 + 1] = state[1 * 4 + 2];
            state[1 * 4 + 2] = state[1 * 4 + 3];
            state[1 * 4 + 3] = temp;

            //Row 3
            temp = state[2 * 4 + 0];
            state[ * 4 + [0] = state[2 * 4 + 2];
            state[2 * 4 + 2] = temp;
            temp = state[2 * 4 + 1];
            state[2 * 4 + 1] = state[2 * 4 + 3];
            state[2 * 4 + 3] = temp;

            //Row 4
            temp = state[3 * 4 + 0];
            state[3 * 4 + 0] = state[3 * 4 + 3];
            state[3 * 4 + 3] = state[3 * 4 + 2];
            state[3 * 4 + 2] = state[3 * 4 + 1];
            state[3 * 4 + 1] = temp;
            */
        }

        void AES::InvShiftRows(unsigned char state[16]) {
            // row 2
            unsigned char buffer = state[1 * 4 + 3];
            memmove(&state[4] + 1, &state[4], sizeof(unsigned char) * 3);
            state[1 * 4 + 0] = buffer;

            // row 3
            unsigned char thrid[2];
            memcpy(thrid, &state[8], sizeof(unsigned char) * 2);
            memcpy(&state[8], &state[8] + 2, sizeof(unsigned char) * 2);
            memcpy(&state[8] + 2, thrid, sizeof(unsigned char) * 2);

            // row 4
            buffer = state[3 * 4 + 0];
            memmove(&state[12], &state[12] + 1, sizeof(unsigned char) * 3);
            state[3 * 4 + 3] = buffer;
        }
    
        void AES::SubBytes(unsigned char state[16]) {
            state[0 * 4 + 0] = sbox[state[0 * 4 + 0]];
            state[0 * 4 + 1] = sbox[state[0 * 4 + 1]];
            state[0 * 4 + 2] = sbox[state[0 * 4 + 2]];
            state[0 * 4 + 3] = sbox[state[0 * 4 + 3]];

            state[1 * 4 + 0] = sbox[state[1 * 4 + 0]];
            state[1 * 4 + 1] = sbox[state[1 * 4 + 1]];
            state[1 * 4 + 2] = sbox[state[1 * 4 + 2]];
            state[1 * 4 + 3] = sbox[state[1 * 4 + 3]];

            state[2 * 4 + 0] = sbox[state[2 * 4 + 0]];
            state[2 * 4 + 1] = sbox[state[2 * 4 + 1]];
            state[2 * 4 + 2] = sbox[state[2 * 4 + 2]];
            state[2 * 4 + 3] = sbox[state[2 * 4 + 3]];

            state[3 * 4 + 0] = sbox[state[3 * 4 + 0]];
            state[3 * 4 + 1] = sbox[state[3 * 4 + 1]];
            state[3 * 4 + 2] = sbox[state[3 * 4 + 2]];
            state[3 * 4 + 3] = sbox[state[3 * 4 + 3]];
        }

        void AES::InvSubBytes(unsigned char state[16]) {
            state[0 * 4 + 0] = inv_sbox[state[0 * 4 + 0]];
            state[0 * 4 + 1] = inv_sbox[state[0 * 4 + 1]];
            state[0 * 4 + 2] = inv_sbox[state[0 * 4 + 2]];
            state[0 * 4 + 3] = inv_sbox[state[0 * 4 + 3]];

            state[1 * 4 + 0] = inv_sbox[state[1 * 4 + 0]];
            state[1 * 4 + 1] = inv_sbox[state[1 * 4 + 1]];
            state[1 * 4 + 2] = inv_sbox[state[1 * 4 + 2]];
            state[1 * 4 + 3] = inv_sbox[state[1 * 4 + 3]];

            state[2 * 4 + 0] = inv_sbox[state[2 * 4 + 0]];
            state[2 * 4 + 1] = inv_sbox[state[2 * 4 + 1]];
            state[2 * 4 + 2] = inv_sbox[state[2 * 4 + 2]];
            state[2 * 4 + 3] = inv_sbox[state[2 * 4 + 3]];

            state[3 * 4 + 0] = inv_sbox[state[3 * 4 + 0]];
            state[3 * 4 + 1] = inv_sbox[state[3 * 4 + 1]];
            state[3 * 4 + 2] = inv_sbox[state[3 * 4 + 2]];
            state[3 * 4 + 3] = inv_sbox[state[3 * 4 + 3]];
        }
    } // namespace BlockCipher
} // namespace Krypt

#endif

#endif
