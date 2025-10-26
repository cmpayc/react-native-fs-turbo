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

#ifndef KRYPT_BYTEARRAY_HPP
#define KRYPT_BYTEARRAY_HPP

#include <iostream>
#include <utility>
#include "types.hpp"

namespace Krypt {
    class ByteArray {
        public:

        /// this is the Bytes* or unsigned char* that contains the elements of the array
        Bytes *array;

        /// the total element of the in the array
        size_t length;

        /// detach the heap allocated pointer inside of the ByteArray class instance.
        /// @return raw pointer of unsigned char* or Bytes*.
        Bytes *detach();

        Bytes &operator[](size_t i);
        const Bytes &operator[](size_t i) const;

        ByteArray();

        ByteArray(Bytes *heap_obj, size_t length);

        // copy constructor
        ByteArray(const ByteArray &other);

        // move constructor
        ByteArray(ByteArray &&other) noexcept;

        // copy assignment
        ByteArray &operator=(const ByteArray &other);

        // move assingment
        ByteArray &operator=(ByteArray &&other) noexcept;

        ~ByteArray();
    };

    std::ostream &operator<<(std::ostream &outputStream, const ByteArray &instance);

    std::istream &operator>>(std::istream &inputStream, ByteArray &instance);
} // namespace Krypt

#endif

#endif
