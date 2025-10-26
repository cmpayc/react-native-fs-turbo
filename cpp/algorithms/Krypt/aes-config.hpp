/*
 * MIT License
 *
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

#ifndef AES_CONFIG_HPP
#define AES_CONFIG_HPP

// WIP

#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
#include <immintrin.h>
#include <xmmintrin.h>
#define AES_IMPLEMENT_INTEL_AESNI
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM)
#if defined(__GNUC__)
    #include <stdint.h>
#endif

#if defined(__ARM_NEON) || defined(_MSC_VER)
    #include <arm_neon.h>
#endif

/* GCC and LLVM Clang, but not Apple Clang */
#if defined(__GNUC__) && !defined(__apple_build_version__)
    #if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
        #include <arm_acle.h>
    #endif
#endif
#define AES_IMPLEMENT_ARM_NEON
#else
#define AES_IMPLEMENT_PORTABLE_CPLUSPLUS
#endif

#if defined(USE_AES_ARM_NEON)
#undef AES_IMPLEMENT_INTEL_AESNI
#undef AES_IMPLEMENT_PORTABLE_CPLUSPLUS
#elif defined(USE_AES_INTEL_AESNI)
#undef AES_IMPLEMENT_ARM_NEON
#undef AES_IMPLEMENT_PORTABLE_CPLUSPLUS
#else
#undef AES_IMPLEMENT_ARM_NEON
#undef AES_IMPLEMENT_INTEL_AESNI
#endif

#endif

#endif
