cmake_minimum_required(VERSION 3.9.0)
project(RNFSTurbo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

set(
    ANDROID_SOURCES
    src/main/cpp/RNFSTurboLogger.cpp
    src/main/cpp/RNFSTurboPlatformHelper.cpp
    src/main/cpp/JNIOnLoad.cpp
    src/main/cpp/RNFSTurboModule.cpp
)
set(
    SHARED_SOURCES
    ../cpp/RNFSTurboHostObject.cpp
    ../cpp/RNFSTurboInstall.cpp
    ../cpp/algorithms/base64.cpp
    ../cpp/algorithms/md5.cpp
    ../cpp/algorithms/sha1.cpp
    ../cpp/algorithms/sha224.cpp
    ../cpp/algorithms/sha256.cpp
    ../cpp/algorithms/sha384.cpp
    ../cpp/algorithms/sha512.cpp
    ../cpp/algorithms/base64.cpp
    ../cpp/filesystem/filesystem-utils.cpp
)

if(RNFSTURBO_USE_ENCRYPTION)
    set(CMAKE_CXX_FLAGS "-DUSE_ARM_AES")
    add_definitions(-DRNFSTURBO_USE_ENCRYPTION)

    list(
        APPEND
        SHARED_SOURCES
        ../cpp/algorithms/Krypt/bytearray.cpp
        ../cpp/algorithms/Krypt/functions.cpp
        ../cpp/algorithms/Krypt/AES.cpp
        ../cpp/algorithms/Krypt/mode.cpp
        ../cpp/algorithms/Krypt/padding.cpp
        ../cpp/encryption/encryption-utils.cpp
    )
endif()

# Compile sources
add_library(
    RNFSTurbo
    SHARED
    ${ANDROID_SOURCES}
    ${SHARED_SOURCES}
)

# Add headers search paths
target_include_directories(
    RNFSTurbo
    PUBLIC
    src/main/cpp
    ../cpp
)

# Add android/log dependency
find_library(log-lib log)

# Add react dependencies
find_package(fbjni)
find_package(ReactAndroid)

# Link all libraries together
target_link_libraries(
    RNFSTurbo
    ${log-lib}                  # <-- Logcat logger
    android                     # <-- Android JNI core
    fbjni::fbjni                # <-- Facebook C++ JNI helpers
    ReactAndroid::jsi           # <-- RN: JSI
)

# Link react-native (different prefab between RN 0.75 and RN 0.76)
if(ReactAndroid_VERSION_MINOR GREATER_EQUAL 76)
    target_link_libraries(
        RNFSTurbo
        ReactAndroid::reactnative                 # <-- RN: Native Modules umbrella prefab
    )
else()
    target_link_libraries(
        RNFSTurbo
        ReactAndroid::react_nativemodule_core     # <-- RN: TurboModules Core
        ReactAndroid::turbomodulejsijni           # <-- RN: TurboModules utils (e.g. CallInvokerHolder)
    )
endif()
