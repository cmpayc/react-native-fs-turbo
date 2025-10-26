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

#ifndef KRYPT_PADDING_HPP
#define KRYPT_PADDING_HPP

#include <cstring>
#include <exception>
#include <iostream>
#include <utility>
#include "bytearray.hpp"

namespace Krypt {
    namespace Padding {
        class InvalidPadding : public std::exception {
            public:

            InvalidPadding(const char *info) : msg(info) {
            }
            const char *msg;
            const char *what() const throw();
        };

        class InvalidPaddedLength : public std::exception {
            public:

            InvalidPaddedLength(const char *paddingSchemName) : msg(paddingSchemName) {
            }
            const char *msg;
            const char *what() const throw();
        };

        /// default & base class for padding - pad the src with zeros.
        class NoPadding {
            public:

            /** does nothing obviously. **/
            virtual ByteArray AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE);

            /** does nothing obviously. **/
            virtual ByteArray RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE);

            virtual ~NoPadding() = default;
        };

        /// default & base class for padding - pad the src with zeros.
        class ZeroNulls : public NoPadding {
            public:

            /** Pad the last block with zeros [reallocates memory].
             * @return padded ByteArray generated from src.
             * **/
            ByteArray AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE);

            /** Removes the last 16 byte zeros [reallocates memory].
             * @return padded ByteArray generated from src.
             * **/
            ByteArray RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE);

            ~ZeroNulls() = default;
        };

        class ANSI_X9_23 : public NoPadding {
            public:

            /** Pad the `src` with zeros first, then sets the last byte value to the count of paddings added
             * [reallocates memory].
             * @return padded ByteArray generated from src.
             * **/
            ByteArray AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE) override;

            /** Removes the number of bytes [reallocates memory].
             * @returns padded ByteArray generated from src.
             * **/
            ByteArray RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) override;

            ~ANSI_X9_23() {
            }
        };

        class ISO_IEC_7816_4 : public NoPadding {
            public:

            /** Adds one `0x80` byte value, then pad the next remaining spaces with zeros [reallocates memory].
             * @return padded ByteArray generated from src.
             * **/
            ByteArray AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE) override;

            /** removes padding [reallocates memory].
             * - figures out the padding size by checking the sequence of zeros from the least significant to the most
             * significant byte until it hits `0x80` byte value.
             * @return padded ByteArray generated from src.
             * **/
            ByteArray RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) override;

            ~ISO_IEC_7816_4() {
            }
        };

        class PKCS_5_7 : public NoPadding {
            public:

            /** Pad the `src` with the value of the padding count itself [reallocates memory].
             * @return the new length of the padded `src`.
             * **/
            ByteArray AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE) override;

            /** removes padding [reallocates memory].
             * - figures out the padding size by getting the value of the last byte.
             * @return the new length of the un-padded `src`.
             * **/
            ByteArray RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) override;

            ~PKCS_5_7() {
            }
        };
    } // namespace Padding
} // namespace Krypt

#endif

#endif
