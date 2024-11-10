//
//  helpers.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

#include "helpers.h"

std::string readFile(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open() || !inputFile.good()) {
    throw "Can not open file";
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

std::vector<unsigned char> readFileUint8(const char* filePath, int offset, int length) {
  std::ifstream inputFile(filePath, std::ios::binary);
  if (!inputFile.is_open() || !inputFile.good()) {
    throw "Can not open file";
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

void writeFile(const char* filePath, const char* content, bool isAppend) {
  std::ofstream outputFile(filePath, isAppend ? std::ios::ate|std::ios::app : std::ios::binary);
  if (!outputFile.is_open() || !outputFile.good()) {
    throw "Can not open file";
  }

  outputFile << content;

  outputFile.close();
}

void writeFileUint8(const char* filePath, const uint8_t *contentUint8, size_t length, bool isAppend) {
  std::ofstream outputFile(filePath, isAppend ? std::ios::ate|std::ios::app|std::ios::binary : std::ios::binary);
  if (!outputFile.is_open() || !outputFile.good()) {
    throw "Can not open file";
  }

  outputFile.write((char*)contentUint8, length);

  outputFile.close();
}


void writeWithOffset(const char* filePath, std::string content, int offset) {
  std::string fileData = readFile(filePath, 0, 0);
  if (offset > fileData.size()) {
    throw "Offset is greater then file size";
  }
  std::ofstream outputFile(filePath, std::ios::trunc | std::ios::binary);
  if (!outputFile.is_open() || !outputFile.good()) {
    throw "Can not open file";
  }
  
  fileData.replace(offset, content.size(), content);
  
  outputFile << fileData;

  outputFile.close();
}