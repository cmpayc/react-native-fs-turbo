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

#ifndef KRYPT_BLOCKCIPHER_HPP
#define KRYPT_BLOCKCIPHER_HPP

#include <cstring>
#include <exception>
#include <iostream>
#include "types.hpp"

namespace Krypt {
    namespace BlockCipher {
        class BASE_BLOCKCIPHER {
            public:

            size_t BLOCK_SIZE;

            BASE_BLOCKCIPHER(size_t blockSize) : BLOCK_SIZE(blockSize) {
            }

            /// encrypts a fixed 16 byte block from `src` into `dest`.
            /// @param src a pointer to an array of unsigned char*/Bytes* in heap this is
            /// the array you want to encrypt.
            /// @param dest another pointer to an array of unsigned char*/Bytes* in heap
            /// this is the array where the encrypted block will be stored.
            virtual void EncryptBlock(Bytes *, Bytes *) = 0;

            /// decrypts a fixed 16 byte block from `src` into `dest`.
            /// @param src a pointer to an array of unsigned char*/Bytes* in heap this is
            /// the array you want to decrypt.
            /// @param dest another pointer to an array of unsigned char*/Bytes* in heap
            /// this is the array where the decrypted block will be stored.
            virtual void DecryptBlock(Bytes *, Bytes *) = 0;

            virtual ~BASE_BLOCKCIPHER() = default;
        };
    } // namespace BlockCipher
} // namespace Krypt

#endif

#endif
