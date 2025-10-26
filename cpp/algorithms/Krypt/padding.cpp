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

#ifndef KRYPT_PADDING_CPP
#define KRYPT_PADDING_CPP

#import "padding.hpp"

namespace Krypt {
    namespace Padding {
        // ANSI_X9_23
        ByteArray ANSI_X9_23::AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
            size_t paddings = BLOCKSIZE - (len % BLOCKSIZE);
            size_t paddedLen = paddings + len;
            Bytes *paddedBlock = new Bytes[paddedLen];

            memcpy(paddedBlock, src, len);
            memset(paddedBlock + len, 0x00, paddings);
            paddedBlock[paddedLen - 1] = static_cast<Bytes>(paddings);

            return ByteArray(paddedBlock, paddedLen);
        }

        ByteArray ANSI_X9_23::RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
#ifndef PADDING_CHECK_DISABLE
            if (len < BLOCKSIZE || len % BLOCKSIZE != 0) {
                std::cerr << "\nA padded `src` should have a `len` greater than and divisible by the `BLOCKSIZE`\n";
                throw InvalidPaddedLength("ANSI_X9_23: src's `len` indicates that it was not padded or is corrupted");
            }
#endif

            size_t paddings = src[len - 1];
            size_t noPaddingLength = len - paddings;

#ifndef PADDING_CHECK_DISABLE
            for (size_t i = 1; i < paddings; ++i) {
                if (src[len - 1 - i] != 0x00) {
                    throw InvalidPadding("ANSI_X9_23: does not match the padding scheme used in `src`");
                }
            }
#endif

            Bytes *NoPadding = new Bytes[noPaddingLength];
            memcpy(NoPadding, src, noPaddingLength);

            return ByteArray(NoPadding, noPaddingLength);
        }
    
        // ISO_IEC_7816_4
        ByteArray ISO_IEC_7816_4::AddPadding(Bytes *src, size_t originalSrcLen, size_t BLOCKSIZE) {
            size_t paddings = BLOCKSIZE - (originalSrcLen % BLOCKSIZE);
            size_t paddedLen = paddings + originalSrcLen;
            Bytes *paddedBlock = new Bytes[paddedLen];

            memcpy(paddedBlock, src, originalSrcLen);
            memset(paddedBlock + originalSrcLen, 0x00, paddings);
            paddedBlock[originalSrcLen] = 0x80;

            return ByteArray(paddedBlock, paddedLen);
        }

        ByteArray ISO_IEC_7816_4::RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
#ifndef PADDING_CHECK_DISABLE
            if (len < BLOCKSIZE || len % BLOCKSIZE != 0) {
                std::cerr << "\nA padded `src` should have a `len` greater than and divisible by the `BLOCKSIZE`\n";
                throw InvalidPaddedLength("ISO_IEC_7816_4: src's `len` indicates that it was not padded or is corrupted"
                );
            }
#endif

            size_t i;

#ifndef PADDING_CHECK_DISABLE
            for (i = 1; i < BLOCKSIZE; ++i) {
                if (src[len - i] == 0x80) {
                    break;
                }

                if (src[len - i] != 0x00) {
                    throw InvalidPadding("ISO_IEC_7816_4: does not match the padding scheme used in `src`");
                }
            }
#endif

            size_t noPaddingLength = len - i;
            Bytes *NoPadding = new Bytes[noPaddingLength];
            memcpy(NoPadding, src, noPaddingLength);

            return ByteArray(NoPadding, noPaddingLength);
        }
    
        // NoPadding
        const char *InvalidPadding::what() const throw() {
            return msg;
        }
        const char *InvalidPaddedLength::what() const throw() {
            return msg;
        }

        ByteArray NoPadding::AddPadding(Bytes *src, size_t len, size_t) {
            Bytes *Copy = new Bytes[len];
            memcpy(Copy, src, len);
            return ByteArray(Copy, len);
        }

        ByteArray NoPadding::RemovePadding(Bytes *src, size_t len, size_t) {
            Bytes *Copy = new Bytes[len];
            memcpy(Copy, src, len);
            return ByteArray(Copy, len);
        }
    
        // PKCS_5_7
        ByteArray PKCS_5_7::AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
            size_t paddings = BLOCKSIZE - (len % BLOCKSIZE);
            size_t paddedLen = paddings + len;
            Bytes *paddedBlock = new Bytes[paddedLen];

            memcpy(paddedBlock, src, len);
            memset(paddedBlock + len, static_cast<Bytes>(paddings), paddings);

            return ByteArray(paddedBlock, paddedLen);
        }

        ByteArray PKCS_5_7::RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
#ifndef PADDING_CHECK_DISABLE
            if (len < BLOCKSIZE || len % BLOCKSIZE != 0) {
                std::cerr << "\nA padded `src` should have a `len` greater than and divisible by the `BLOCKSIZE`\n";
                throw InvalidPaddedLength("PKCS_5_7: src's `len` indicates that it was not padded or is corrupted");
            }
#endif

            size_t paddings = src[len - 1];
            size_t noPaddingLength = len - paddings;

#ifndef PADDING_CHECK_DISABLE
            Bytes checkchar = static_cast<Bytes>(paddings);
            for (size_t i = 1; i < paddings; ++i) {
                if (src[len - 1 - i] != checkchar) {
                    throw InvalidPadding("PKCS_5_7: does not match the padding scheme used in `src`");
                }
            }
#endif

            Bytes *NoPadding = new Bytes[noPaddingLength];
            memcpy(NoPadding, src, noPaddingLength);

            return ByteArray(NoPadding, noPaddingLength);
        }
    
        // ZeroNulls
        ByteArray ZeroNulls::AddPadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
            size_t paddings = BLOCKSIZE - (len % BLOCKSIZE);
            size_t paddedLen = paddings + len;
            Bytes *paddedBlock = new Bytes[paddedLen];

            memcpy(paddedBlock, src, len);
            memset(paddedBlock + len, 0x00, paddings);

            return ByteArray(paddedBlock, paddedLen);
        }

        ByteArray ZeroNulls::RemovePadding(Bytes *src, size_t len, size_t BLOCKSIZE) {
#ifndef PADDING_CHECK_DISABLE
            if (len < BLOCKSIZE || len % BLOCKSIZE != 0) {
                std::cerr << "\nA padded `src` should have a `len` greater than and divisible by the `BLOCKSIZE`\n";
                throw InvalidPaddedLength("ZeroNulls: src's `len` indicates that it was not padded or is corrupted");
            }
#endif

#ifndef PADDING_CHECK_DISABLE
            if (src[len - 1] != 0x00) {
                throw InvalidPadding("ZeroNulls: does not match the padding scheme used in `src`");
            }
#endif

            size_t paddings = 0, noPaddingLength = 0;
            for (size_t i = 0; i < BLOCKSIZE; ++i)
                if (src[len - 1 - i] == 0x00) {
                    paddings++;
                } else {
                    break;
                }

            noPaddingLength = len - paddings;
            Bytes *NoPadding = new Bytes[noPaddingLength];
            memcpy(NoPadding, src, noPaddingLength);

            return ByteArray(NoPadding, noPaddingLength);
        }
    } // namespace Padding
} // namespace Krypt

#endif

#endif
