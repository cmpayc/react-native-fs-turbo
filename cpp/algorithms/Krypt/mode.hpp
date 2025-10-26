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

#pragma once

#ifdef RNFSTURBO_USE_ENCRYPTION

#ifndef KRYPT_MODE_OF_ENCRYPTION_HPP
#define KRYPT_MODE_OF_ENCRYPTION_HPP

#include <iostream>
#include "blockcipher.hpp"
#include "bytearray.hpp"
#include "padding.hpp"

namespace Krypt {
    namespace Mode {
        class MODE {
            public:

            BlockCipher::BASE_BLOCKCIPHER *Encryption;
            Padding::NoPadding *PaddingScheme;

            MODE() : Encryption(nullptr), PaddingScheme(nullptr) {}

            virtual ByteArray encrypt(Bytes *, size_t, Bytes *) = 0;
            virtual ByteArray decrypt(Bytes *, size_t, Bytes *) = 0;

            virtual void blockEncrypt(Bytes *, Bytes *, Bytes *) = 0;
            virtual void blockDecrypt(Bytes *, Bytes *, Bytes *) = 0;

            virtual ~MODE() {
                delete Encryption;
                delete PaddingScheme;
            }
        };

        /// Electronic Code Blocking.
        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        class ECB : public MODE {
            public:

            ECB(const Bytes *key, size_t keyLen);
            /// encrypts the whole plain array using ECB mode of encryption with a chosen BlockCipher and Padding.
            /// @param plain pointer to a Bytes*/unsigned char* array, this is the array you want to encrypt.
            /// @param plainLen this is the size of the *plain array.
            /// @param iv for ECB passing an iv will not do anything so there is no need to provide this function an iv
            /// array.
            ByteArray encrypt(Bytes *plain, size_t plainLen, Bytes *iv = nullptr) override;
            /// decrypts the whole cipher array using ECB mode of decryption with a chosen BlockCipher and Padding.
            /// @param cipher pointer to a Bytes*/unsigned char* array, this is the array you want to decrypt.
            /// @param cipherLen this is the size of the *cipher array.
            /// @param iv for ECB passing an iv will not do anything so there is no need to provide this function an iv
            /// array.
            ByteArray decrypt(Bytes *cipher, size_t cipherLen, Bytes *iv = nullptr) override;

            void blockEncrypt(Bytes *plain, Bytes *cipher, Bytes *iv = nullptr) override;
            void blockDecrypt(Bytes *plain, Bytes *cipher, Bytes *iv = nullptr) override;
        };

        /// Cipher Block Chaining.
        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        class CBC : public MODE {
            public:

            CBC(const Bytes *key, size_t keyLen);
            /// encrypts the whole plain array using CBC mode of encryption with a chosen BlockCipher and Padding.
            /// @param plain pointer to a Bytes*/unsigned char* array, this is the array you want to encrypt.
            /// @param plainLen this is the size of the *plain array.
            /// @param iv the initial array use for the XOR'ing operations during the CBC encryption.
            ByteArray encrypt(Bytes *plain, size_t plainLen, Bytes *iv) override;
            /// decrypts the whole cipher array using CBC mode of decryption with a chosen BlockCipher and Padding.
            /// @param cipher pointer to a Bytes*/unsigned char* array, this is the array you want to decrypt.
            /// @param cipherLen this is the size of the *cipher array.
            /// @param iv the initial array use for the XOR'ing operations during the CBC decryption.
            ByteArray decrypt(Bytes *cipher, size_t cipherLen, Bytes *iv) override;

            void blockEncrypt(Bytes *plain, Bytes *cipher, Bytes *iv) override;
            void blockDecrypt(Bytes *plain, Bytes *cipher, Bytes *iv) override;
        };

        /// Cipher Feedback.
        template <typename CIPHER_TYPE, typename PADDING_TYPE>
        class CFB : public MODE {
            public:

            CFB(const Bytes *key, size_t keyLen);
            /// encrypts the whole plain array using CFB mode of encryption with a chosen BlockCipher and Padding.
            /// @param plain pointer to a Bytes*/unsigned char* array, this is the array you want to encrypt.
            /// @param plainLen this is the size of the *plain array.
            /// @param iv the initial array use for the XOR'ing operations during the CFB encryption.
            ByteArray encrypt(Bytes *plain, size_t plainLen, Bytes *iv) override;
            /// decrypts the whole cipher array using CFB mode of decryption with a chosen BlockCipher and Padding.
            /// @param cipher pointer to a Bytes*/unsigned char* array, this is the array you want to decrypt.
            /// @param cipherLen this is the size of the *cipher array.
            /// @param iv the initial array use for the XOR'ing operations during the CFB decryption.
            ByteArray decrypt(Bytes *cipher, size_t cipherLen, Bytes *iv) override;

            void blockEncrypt(Bytes *plain, Bytes *cipher, Bytes *iv) override;
            void blockDecrypt(Bytes *plain, Bytes *cipher, Bytes *iv) override;
        };
    } // namespace Mode
} // namespace Krypt

#include "mode.cpp"

#endif

#endif
