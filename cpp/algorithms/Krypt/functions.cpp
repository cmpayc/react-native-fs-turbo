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

#ifndef KRYPT_FUNCTIONS_CPP
#define KRYPT_FUNCTIONS_CPP

#include "functions.hpp"
#include "types.hpp"

// #define USE_AESNI

#if defined(USE_AESNI)
#include <immintrin.h>
#else

#endif

// here the DWORDS is formed from 4 continous elements from a starting index of an unsinged char* array or Bytes*.
// a DWORD or double-word occupies 32-bits in a memory.
// the term DWORD is usually used to represent bits inside the eax,edx,...r15d parts of an x86_64 cpu register which can
// contain 32 bits inside.

namespace Krypt {
    Bytes xtime(Bytes b) // multiply on x
    {
        return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
    }

    void SubDWordBytes(Bytes *a) {
        a[0] = sbox[a[0]];
        a[1] = sbox[a[1]];
        a[2] = sbox[a[2]];
        a[3] = sbox[a[3]];
    }

    void RotDWord(Bytes *a) {
        Bytes c = a[0];
        a[0] = a[1];
        a[1] = a[2];
        a[2] = a[3];
        a[3] = c;
    }

    void XorDWords(Bytes *a, Bytes *b, Bytes *dest) {
        dest[0] = a[0] ^ b[0];
        dest[1] = a[1] ^ b[1];
        dest[2] = a[2] ^ b[2];
        dest[3] = a[3] ^ b[3];
    }

    void XorBlocks(unsigned char *a, unsigned char *b, unsigned char *dest, unsigned int len) {
        for (unsigned int i = 0; i < len; i++) {
            dest[i] = a[i] ^ b[i];
        }
    }

    void XorAesBlock(unsigned char *a, unsigned char *b, unsigned char *result) {
        #if defined(USE_AESNI)
        __m128i vxor = _mm_xor_si128(
            _mm_load_si128((__m128i *) a),
            _mm_load_si128((__m128i *) b)
        );

        _mm_storeu_si128((__m128i *) (result), vxor);
        #else
        uint64_t
            *A = reinterpret_cast<uint64_t *>(a),
            *B = reinterpret_cast<uint64_t *>(b),
            *C = reinterpret_cast<uint64_t *>(result);

        C[0] = A[0] ^ B[0];
        C[1] = A[1] ^ B[1];
        #endif
    }

    void Rcon(Bytes *a, int n) {
        int i;
        Bytes c = 1;
        for (i = 0; i < n - 1; i++) {
            c = xtime(c);
        }

        a[0] = c;
        a[1] = a[2] = a[3] = 0;
    }

    void printHexArray(unsigned char a[], size_t n) {
        for (size_t i = 0; i < n; i++) {
            printf("%02x ", a[i]);
        }
        std::cout << "\n";
    }

    void printHexVector(const std::vector<unsigned char> &a) {
        for (size_t i = 0; i < a.size(); i++) {
            printf("%02x ", a[i]);
        }
    }

    std::vector<unsigned char> ArrayToVector(std::unique_ptr<unsigned char[]> a, unsigned char len) {
        return std::vector<unsigned char>(a.get(), a.get() + len);
    }

    std::unique_ptr<unsigned char[]> VectorToArray(const std::vector<unsigned char>& a) {
        auto cpy = std::make_unique<unsigned char[]>(a.size());
        memcpy(cpy.get(), a.data(), a.size());
        return cpy;
    }
} // namespace Krypt

#endif

#endif
