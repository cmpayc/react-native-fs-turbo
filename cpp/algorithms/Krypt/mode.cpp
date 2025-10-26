/*
 * MIT License
 *
 * Copyright (c) 2019 SergeyBel
 * Copyright (c) 2023 Jubal Mordecai Velasco
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

#ifndef KRYPT_MODE_OF_ENCRYPTION_CPP
#define KRYPT_MODE_OF_ENCRYPTION_CPP

#include "functions.hpp"
#include "mode.hpp"
#include "padding.hpp"
#include "types.hpp"

namespace Krypt {
    namespace Mode {
        // CBC
        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        CBC<CIPHER_TYPE, PADDING_TYPE>::CBC(const Bytes *key, size_t keyLen) : MODE() {
            Encryption = new CIPHER_TYPE(key, keyLen);
            PaddingScheme = new PADDING_TYPE();
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ByteArray CBC<CIPHER_TYPE, PADDING_TYPE>::encrypt(Bytes *plain, size_t plainLen, Bytes *iv) {
            ByteArray padded = PaddingScheme->AddPadding(plain, plainLen, Encryption->BLOCK_SIZE);
            Bytes *tempIV = new Bytes[Encryption->BLOCK_SIZE];
            Bytes *cipher = new Bytes[padded.length];

            memcpy(tempIV, iv, Encryption->BLOCK_SIZE);

            for (size_t i = 0; i < padded.length; i += Encryption->BLOCK_SIZE) {
                this->blockEncrypt(padded.array + i, cipher + i, tempIV);
            }

            delete[] tempIV;
            return ByteArray(cipher, padded.length);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ByteArray CBC<CIPHER_TYPE, PADDING_TYPE>::decrypt(Bytes *cipher, size_t cipherLen, Bytes *iv) {
            Bytes *recover = new Bytes[cipherLen];
            Bytes *tempIV = new Bytes[Encryption->BLOCK_SIZE];
            memcpy(tempIV, iv, Encryption->BLOCK_SIZE);

            for (size_t i = 0; i < cipherLen; i += Encryption->BLOCK_SIZE) {
                this->blockDecrypt(cipher + i, recover + i, tempIV);
            }

            ByteArray recoverNoPadding = PaddingScheme->RemovePadding(recover, cipherLen, Encryption->BLOCK_SIZE);

            delete[] tempIV;
            delete[] recover;

            return recoverNoPadding;
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        void CBC<CIPHER_TYPE, PADDING_TYPE>::blockEncrypt(Bytes *plain, Bytes *cipher, Bytes *iv) {
            XorAesBlock(iv, plain, iv);
            Encryption->EncryptBlock(iv, cipher);
            memcpy(iv, cipher, 16);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        void CBC<CIPHER_TYPE, PADDING_TYPE>::blockDecrypt(Bytes *cipher, Bytes *recover, Bytes *iv) {
            Encryption->DecryptBlock(cipher, recover);
            XorAesBlock(iv, recover, recover);
            memcpy(iv, cipher, Encryption->BLOCK_SIZE);
        }
    
        // CFB
        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        CFB<CIPHER_TYPE, PADDING_TYPE>::CFB(const Bytes *key, size_t keyLen) : MODE() {
            Encryption = new CIPHER_TYPE(key, keyLen);
            PaddingScheme = new PADDING_TYPE();
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ByteArray CFB<CIPHER_TYPE, PADDING_TYPE>::encrypt(Bytes *plain, size_t plainLen, Bytes *iv) {
            ByteArray padded = PaddingScheme->AddPadding(plain, plainLen, Encryption->BLOCK_SIZE);

            Bytes *tempIV = new Bytes[Encryption->BLOCK_SIZE];
            Bytes *encIV = new Bytes[Encryption->BLOCK_SIZE];
            Bytes *cipher = new Bytes[padded.length];

            memcpy(tempIV, iv, Encryption->BLOCK_SIZE);

            for (size_t i = 0; i < padded.length; i += Encryption->BLOCK_SIZE) {
                blockEncrypt(padded.array + i, cipher + i, tempIV);
            }

            delete[] tempIV;
            delete[] encIV;

            return ByteArray(cipher, padded.length);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ByteArray CFB<CIPHER_TYPE, PADDING_TYPE>::decrypt(Bytes *cipher, size_t cipherLen, Bytes *iv) {
            Bytes *recover = new Bytes[cipherLen];
            Bytes *tempIV = new Bytes[Encryption->BLOCK_SIZE];
            Bytes *encIV = new Bytes[Encryption->BLOCK_SIZE];

            memcpy(tempIV, iv, Encryption->BLOCK_SIZE);

            for (size_t i = 0; i < cipherLen; i += Encryption->BLOCK_SIZE) {
                Encryption->EncryptBlock(tempIV, encIV);
                XorAesBlock(cipher + i, encIV, recover + i);
                memcpy(tempIV, cipher + i, Encryption->BLOCK_SIZE);
            }

            ByteArray recoverNoPadding = PaddingScheme->RemovePadding(recover, cipherLen, Encryption->BLOCK_SIZE);

            delete[] recover;
            delete[] tempIV;
            delete[] encIV;

            return recoverNoPadding;
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        void CFB<CIPHER_TYPE, PADDING_TYPE>::blockEncrypt(Bytes *plain, Bytes *cipher, Bytes *iv) {
            Encryption->EncryptBlock(iv, cipher);
            XorAesBlock(plain, cipher, cipher);
            memcpy(iv, cipher, Encryption->BLOCK_SIZE);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        void CFB<CIPHER_TYPE, PADDING_TYPE>::blockDecrypt(Bytes *cipher, Bytes *recover, Bytes *iv) {
            Encryption->EncryptBlock(iv, recover);
            XorAesBlock(cipher, recover, recover);
            memcpy(iv, cipher, Encryption->BLOCK_SIZE);
        }
    
        // ECB
        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ECB<CIPHER_TYPE, PADDING_TYPE>::ECB(const Bytes *key, size_t keyLen) : MODE() {
            this->Encryption = new CIPHER_TYPE(key, keyLen);
            this->PaddingScheme = new PADDING_TYPE();
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ByteArray ECB<CIPHER_TYPE, PADDING_TYPE>::encrypt(Bytes *plain, size_t plainLen, Bytes *) {
            ByteArray padded = this->PaddingScheme->AddPadding(plain, plainLen, this->Encryption->BLOCK_SIZE);

            Bytes *cipher = new Bytes[padded.length];
            for (size_t i = 0; i < padded.length; i += this->Encryption->BLOCK_SIZE) {
                blockEncrypt(padded.array + i, cipher + i);
            }

            return ByteArray(cipher, padded.length);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        ByteArray ECB<CIPHER_TYPE, PADDING_TYPE>::decrypt(Bytes *cipher, size_t cipherLen, Bytes *) {
            ByteArray recovered;
            recovered.array = new Bytes[cipherLen];
            recovered.length = cipherLen;

            for (size_t i = 0; i < cipherLen; i += this->Encryption->BLOCK_SIZE) {
                blockDecrypt(cipher + i, recovered.array + i);
            }

            return this->PaddingScheme->RemovePadding(recovered.array, recovered.length, this->Encryption->BLOCK_SIZE);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        void ECB<CIPHER_TYPE, PADDING_TYPE>::blockEncrypt(Bytes *plain, Bytes *cipher, Bytes *) {
            this->Encryption->EncryptBlock(plain, cipher);
        }

        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        void ECB<CIPHER_TYPE, PADDING_TYPE>::blockDecrypt(Bytes *cipher, Bytes *recover, Bytes *) {
            this->Encryption->DecryptBlock(cipher, recover);
        }
    } // namespace Mode
} // namespace Krypt

#endif

#endif
