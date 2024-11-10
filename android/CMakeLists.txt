cmake_minimum_required(VERSION 3.9.0)
project(RNFSTurbo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

# Compile sources
add_library(
    react-native-fs-turbo
    SHARED
    src/main/cpp/RNFSTurboLogger.cpp
    src/main/cpp/RNFSTurboPlatformHelper.cpp
    ../cpp/RNFSTurboHostObject.cpp
    ../cpp/NativeRNFSTurboModule.cpp
    ../cpp/algorithms/base64.cpp
    ../cpp/algorithms/md5.cpp
    ../cpp/algorithms/sha1.cpp
    ../cpp/algorithms/sha224.cpp
    ../cpp/algorithms/sha256.cpp
    ../cpp/algorithms/sha384.cpp
    ../cpp/algorithms/sha512.cpp
    ../cpp/filesystem/helpers.cpp
)

# Add headers search paths
target_include_directories(react-native-fs-turbo PUBLIC ../cpp)

# Add android/log dependency
find_library(log-lib log)

target_link_libraries(
    react-native-fs-turbo
    ${log-lib}                  # <-- Logcat logger
    android                     # <-- Android JNI core
    react_codegen_RNFSTurboSpec    # <-- Generated Specs from CodeGen
)
