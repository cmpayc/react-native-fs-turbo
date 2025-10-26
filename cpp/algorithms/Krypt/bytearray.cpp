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

#ifndef KRYPT_BYTEARRAY_CPP
#define KRYPT_BYTEARRAY_CPP

#include "bytearray.hpp"

namespace Krypt {
    // returns the pointer array, leaving the .array() equal to NULL, and .length() equal to zero
    Bytes *ByteArray::detach() {
        Bytes *ptr = array;
        array = NULL;
        length = 0;
        return ptr;
    }

    const Bytes &ByteArray::operator[](size_t i) const {
#ifndef INDEX_CHECK_DISABLE
        if (length == 0) {
            throw std::underflow_error(
                "Krypt::ByteArray[] :"
                "       accessing an empty byte array"
            );
        }

        if (i >= length) {
            throw std::overflow_error(
                "Krypt::ByteArray[] :"
                "       index given for operator[] is greater-than ByteArray.length()-1"
            );
        }
#endif
        return array[i];
    }

    Bytes &ByteArray::operator[](size_t i) {
#ifndef INDEX_CHECK_DISABLE
        if (length == 0) {
            throw std::underflow_error(
                "Krypt::ByteArray[] :"
                "       accessing an empty byte array"
            );
        }

        if (i >= length) {
            throw std::overflow_error(
                "Krypt::ByteArray[] :"
                "       index given for operator[] is greater-than ByteArray.length()-1"
            );
        }
#endif
        return array[i];
    }

    ByteArray::ByteArray() {
        array = NULL;
        length = 0;
    }

    ByteArray::ByteArray(Bytes *heap_obj, size_t length) {
        array = heap_obj;
        this->length = length;
    }

    // copy constructor
    ByteArray::ByteArray(const ByteArray &other) : array(new Bytes[other.length]), length(other.length) {
        memcpy(array, other.array, other.length);
    }

    // move constructor
    ByteArray::ByteArray(ByteArray &&other) noexcept : array(other.array), length(other.length) {
        other.array = NULL;
        other.length = 0;
    }

    // copy assignment
    ByteArray &ByteArray::operator=(const ByteArray &other) {
        if (this != &other) {
            if (array != NULL) {
                memset((Bytes *) array, 0x00, length);
                delete[] array;
            }

            array = new Bytes[other.length];
            length = other.length;
            memcpy(array, other.array, other.length);
        }
        return *this;
    }

    // move assingment
    ByteArray &ByteArray::operator=(ByteArray &&other) noexcept {
        if (this != &other) {
            if (array != NULL) {
                memset((Bytes *) array, 0x00, length);
                delete[] array;
            }

            array = other.array;
            length = other.length;

            other.array = NULL;
            other.length = 0;
        }
        return *this;
    }

    ByteArray::~ByteArray() {
        if (array != NULL) {
            memset((Bytes *) array, 0x00, length);
            delete[] array;
        }
        length = 0;
    }

    std::ostream &operator<<(std::ostream &outputStream, const ByteArray &instance) {
        for (size_t i = 0; i < instance.length; ++i) {
            outputStream << instance.array[i];
        }
        return outputStream;
    }

    std::istream &operator>>(std::istream &inputStream, ByteArray &instance) {
        for (size_t i = 0; i < instance.length; ++i) {
            inputStream >> instance.array[i];
        }
        return inputStream;
    }
} // namespace  Krypt

#endif

#endif
