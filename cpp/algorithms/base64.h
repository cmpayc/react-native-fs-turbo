/*
Copyright (c) 2019 Tobias Locker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#ifndef BASE64_HPP_
#define BASE64_HPP_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

#if defined(__cpp_lib_bit_cast)
#include <bit>  // For std::bit_cast.
#endif

namespace base64 {

std::string to_base64(std::string_view data);

std::string from_base64(std::string_view data);

}
#endif