//
//  helpers.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

#include "helpers.h"

std::string readFile(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open()) {
    throw strerror(errno);
  }
  inputFile.seekg(0, std::ios::end);
  size_t size = inputFile.tellg();
  if (length != 0) {
    if (length > size - offset) {
      size = size - offset;
    } else {
      size = length;
    }
  }
  std::string buffer(size, ' ');
  if (offset > 0) {
    inputFile.seekg(offset);
  } else {
    inputFile.seekg(0);
  }
  inputFile.read(&buffer[0], size);

  inputFile.close();

  return buffer;
}

std::vector<uint8_t> readFileUint8(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open()) {
    throw strerror(errno);
  }
  if (offset > 0 || length > 0) {
    std::vector<uint8_t> vBuffer;
    inputFile.seekg(0, std::ios::end);
    size_t size = inputFile.tellg();
    if (length != 0) {
      if (length > size - offset) {
        size = size - offset;
      } else {
        size = length;
      }
    }
    if (offset > 0) {
      inputFile.seekg(offset);
    } else {
      inputFile.seekg(0);
    }
    vBuffer.resize(static_cast<size_t>(size));
    inputFile.read(reinterpret_cast<char*> (&vBuffer[0]), size);

    inputFile.close();
    return vBuffer;
  }

  std::vector<uint8_t> vBuffer((std::istreambuf_iterator<char>(inputFile)), (std::istreambuf_iterator<char>()));

  inputFile.close();

  return vBuffer;
}

std::vector<float> readFileFloat32(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open()) {
    throw strerror(errno);
  }
  if (offset > 0 || length > 0) {
    std::vector<float> vBuffer;
    inputFile.seekg(0, std::ios::end);
    size_t size = inputFile.tellg();
    if (length != 0) {
      if (length > size - offset) {
        size = size - offset;
      } else {
        size = length;
      }
    }
    if (offset > 0) {
      inputFile.seekg(offset);
    } else {
      inputFile.seekg(0);
    }
    vBuffer.resize(std::floor(size / sizeof(float)));
    inputFile.read(reinterpret_cast<char*> (&vBuffer[0]), size);

    inputFile.close();
    return vBuffer;
  }
  
  std::vector<float> vBuffer;
  inputFile.seekg(0, std::ios::end);
  size_t size = inputFile.tellg();
  if (size >= 4) {
    inputFile.seekg(0);
    vBuffer.resize(std::floor(size / sizeof(float)));
    inputFile.read(reinterpret_cast<char*> (&vBuffer[0]), size);
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

void writeFileUint8(const char* filePath, const uint8_t *contentUint8, size_t length, bool isAppend) {
  std::ofstream outputFile(filePath, isAppend ? std::ios::ate|std::ios::app|std::ios::binary : std::ios::binary);
  if (!outputFile.is_open()) {
    throw strerror(errno);
  }

  outputFile.write((char*)contentUint8, length);

  outputFile.close();
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
