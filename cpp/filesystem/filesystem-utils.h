//
//  helpers.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

#pragma once

#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <variant>

namespace cmpayc::rnfsturbo {

using FileBuffer = std::variant<
    std::vector<uint8_t>,
    std::vector<uint16_t>,
    std::vector<uint32_t>
>;

template <typename T>
T fromBigEndian(T value) {
  static_assert(std::is_integral<T>::value, "T must be integral");

  if constexpr (sizeof(T) == 2) {
    return (value << 8) | (value >> 8);
  } else if constexpr (sizeof(T) == 4) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0xFF000000) >> 24);
  } else if constexpr (sizeof(T) == 8) {
    return ((value & 0x00000000000000FFULL) << 56) |
           ((value & 0x000000000000FF00ULL) << 40) |
           ((value & 0x0000000000FF0000ULL) << 24) |
           ((value & 0x00000000FF000000ULL) << 8)  |
           ((value & 0x000000FF00000000ULL) >> 8)  |
           ((value & 0x0000FF0000000000ULL) >> 24) |
           ((value & 0x00FF000000000000ULL) >> 40) |
           ((value & 0xFF00000000000000ULL) >> 56);
  } else {
    return value;
  }
}

std::string readFile(const char* filePath, int offset, int length);

template <typename T>
std::vector<T> readFileUint(const char* filePath, int offset, int length);

FileBuffer readFileUintUniversal(const std::string& encoding, const char* filePath, int offset, int length);

std::vector<float> readFileFloat32(const char* filePath, int offset, int length);

void writeFile(const char* filePath, std::string_view content, bool isAppend);

template <typename T>
void writeFileUint(const char* filePath, const T *contentUint, size_t length, bool isAppend);

void writeFileUintUniversal(
  const std::string& encoding,
  const char* filePath,
  const uint8_t *contentUint8,
  const uint16_t *contentUint16,
  const uint32_t *contentUint32,
  size_t length,
  bool isAppend
);

void writeFileFloat32(const char* filePath, const float *contentFloat32, size_t length, bool isAppend);

void writeWithOffset(const char* filePath, std::string_view content, int offset);

std::string cleanPath(std::string filePath);

}
