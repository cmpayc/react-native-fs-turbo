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

std::string readFile(const char* filePath, int offset, int length);

std::vector<uint8_t> readFileUint8(const char* filePath, int offset, int length);

void writeFile(const char* filePath, const char* content, bool isAppend);

void writeFileUint8(const char* filePath, const uint8_t *contentUint8, size_t length, bool isAppend);

void writeWithOffset(const char* filePath, std::string content, int offset);

std::string cleanPath(std::string filePath);
