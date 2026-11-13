//
//  helpers.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

#include "filesystem-utils.h"

namespace cmpayc::rnfsturbo {

static size_t prepareRead(std::ifstream& inputFile, int offset, int length) {
  inputFile.seekg(0, std::ios::end);
  std::streamoff fileSize = inputFile.tellg();
  std::streamoff start = offset > 0 ? offset : 0;
  if (fileSize <= 0 || start >= fileSize) {
    return 0;
  }
  std::streamoff available = fileSize - start;
  inputFile.seekg(start);
  return static_cast<size_t>(length > 0 && length < available ? length : available);
}

std::string readFile(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open()) {
    throw strerror(errno);
  }
  size_t size = prepareRead(inputFile, offset, length);
  std::string buffer(size, '\0');
  if (size > 0) {
    inputFile.read(&buffer[0], size);
    buffer.resize(static_cast<size_t>(inputFile.gcount()));
  }

  inputFile.close();

  return buffer;
}

template <typename T>
std::vector<T> readFileUint(const char* filePath, int offset, int length) {
  static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open()) {
    throw strerror(errno);
  }

  size_t size = prepareRead(inputFile, offset, length);
  std::vector<T> vBuffer(size / sizeof(T));
  if (!vBuffer.empty()) {
    inputFile.read(reinterpret_cast<char*>(vBuffer.data()), vBuffer.size() * sizeof(T));
    vBuffer.resize(static_cast<size_t>(inputFile.gcount()) / sizeof(T));
  }

  // Only convert for uint16_t or uint32_t
  if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>) {
      for (auto& x : vBuffer) {
          x = fromBigEndian(x);
      }
  }

  inputFile.close();
  return vBuffer;
}

FileBuffer readFileUintUniversal(const std::string& encoding, const char* filePath, int offset, int length) {
  if (encoding == "uint8") {
    return readFileUint<uint8_t>(filePath, offset, length);
  } else if (encoding == "uint16") {
    return readFileUint<uint16_t>(filePath, offset, length);
  } else if (encoding == "uint32") {
    return readFileUint<uint32_t>(filePath, offset, length);
  } else {
    throw std::invalid_argument("Unsupported encoding: " + encoding);
  }
}

std::vector<float> readFileFloat32(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open()) {
    throw strerror(errno);
  }

  size_t size = prepareRead(inputFile, offset, length);
  std::vector<float> vBuffer(size / sizeof(float));
  if (!vBuffer.empty()) {
    inputFile.read(reinterpret_cast<char*>(vBuffer.data()), vBuffer.size() * sizeof(float));
    vBuffer.resize(static_cast<size_t>(inputFile.gcount()) / sizeof(float));
  }

  inputFile.close();

  return vBuffer;
}

void writeFile(const char* filePath, std::string_view content, bool isAppend) {
  std::ofstream outputFile(filePath, isAppend ? std::ios::ate|std::ios::app : std::ios::binary);
  if (!outputFile.is_open()) {
    throw strerror(errno);
  }

  outputFile << content;

  outputFile.close();
}

template <typename T>
void writeFileUint(const char* filePath, const T *contentUint, size_t length, bool isAppend) {
  std::ofstream outputFile(filePath, isAppend ? std::ios::ate|std::ios::app|std::ios::binary : std::ios::binary);
  if (!outputFile.is_open()) {
    throw strerror(errno);
  }

  outputFile.write((char*)contentUint, length);

  outputFile.close();
}

void writeFileUintUniversal(
  const std::string& encoding,
  const char* filePath,
  const uint8_t *contentUint8,
  const uint16_t *contentUint16,
  const uint32_t *contentUint32,
  size_t length,
  bool isAppend
) {
  if (encoding == "uint8") {
    return writeFileUint<uint8_t>(filePath, contentUint8, length, isAppend);
  } else if (encoding == "uint16") {
    return writeFileUint<uint16_t>(filePath, contentUint16, length, isAppend);
  } else if (encoding == "uint32") {
    return writeFileUint<uint32_t>(filePath, contentUint32, length, isAppend);
  } else {
    throw std::invalid_argument("Unsupported encoding: " + encoding);
  }
}

void writeFileFloat32(const char* filePath, const float *contentFloat32, size_t length, bool isAppend) {
  std::ofstream outputFile(filePath, isAppend ? std::ios::ate|std::ios::app|std::ios::binary : std::ios::binary);
  if (!outputFile.is_open()) {
    throw strerror(errno);
  }

  outputFile.write((char*)contentFloat32, length);

  outputFile.close();
}

void writeWithOffset(const char* filePath, std::string_view content, int offset) {
  std::string fileData = readFile(filePath, 0, 0);
  if (offset > fileData.size()) {
    throw "Offset is greater then file size";
  }
  std::ofstream outputFile(filePath, std::ios::trunc | std::ios::binary);
  if (!outputFile.is_open()) {
    throw strerror(errno);
  }
  
  fileData.replace(offset, content.size(), content);
  
  outputFile << fileData;

  outputFile.close();
}

std::string cleanPath(std::string filePath) {
  return std::regex_replace(
    filePath,
    std::regex("^file://"),
    ""
  );
}

}
