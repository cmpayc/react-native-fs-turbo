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

#ifndef KRYPT_FUNCTIONS_HPP
#define KRYPT_FUNCTIONS_HPP

#include <iostream>
#include "types.hpp"

namespace Krypt {
    Bytes xtime(Bytes b); // multiply on x
    void SubDWordBytes(Bytes *a);
    void RotDWord(Bytes *a);
    void XorDWords(Bytes *a, Bytes *b, Bytes *dest);
    void XorBlocks(unsigned char *a, unsigned char *b, unsigned char *c, unsigned int len);
    void XorAesBlock(unsigned char *a, unsigned char *b, unsigned char *result);

    void Rcon(Bytes *a, int n);
    void printHexArray(unsigned char a[], size_t n);
    void printHexVector(const std::vector<unsigned char> &a);

    std::vector<unsigned char> ArrayToVector(std::unique_ptr<unsigned char[]> a, unsigned char len);
    std::unique_ptr<unsigned char[]> VectorToArray(const std::vector<unsigned char>& a);
} // namespace Krypt

#endif

#endif
