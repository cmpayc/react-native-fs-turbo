//
//  RNFSTurboLogger.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#pragma once

#include <string>

class RNFSTurboLogger {
private:
  RNFSTurboLogger() = delete;

private:
  template <typename... Args>
  static std::string string_format(const std::string& format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0) {
      throw std::runtime_error("Failed to format string!");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }

public:
  static void log(const std::string& tag, const std::string& message);

  static std::string sprintf(const std::string& message) {
    return message;
  }

  template <typename... Args>
  inline static void log(const std::string& tag, const std::string& formatString, Args&&... args) {
    std::string formattedString = string_format(formatString, std::forward<Args>(args)...);
    log(tag, formattedString);
  }

  template <typename... Args>
  inline static std::string sprintf(const std::string& formatString, Args&&... args) {
    std::string formattedString = string_format(formatString, std::forward<Args>(args)...);
    return sprintf(formattedString);
  }
};
