//
//  helpers.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//
#ifdef RNFSTURBO_USE_ENCRYPTION
#include "encryption-utils.h"
#include "RNFSTurboLogger.h"

namespace cmpayc::rnfsturbo {

using namespace facebook;
using namespace Krypt;

std::unique_ptr<Mode::MODE> createCipherMode(
  jsi::Runtime& runtime,
  const std::string& propName,
  const std::string& mode,
  const std::string& padding,
  const Bytes* key,
  size_t keyLen
) {
  if (mode == "ecb") {
    if (padding == "ansi_x9.23") {
      return std::make_unique<Mode::ECB<BlockCipher::AES, Padding::ANSI_X9_23>>(key, keyLen);
    } else if (padding == "iso/iec_7816-4") {
      return std::make_unique<Mode::ECB<BlockCipher::AES, Padding::ISO_IEC_7816_4>>(key, keyLen);
    } else if (padding == "zero") {
      return std::make_unique<Mode::ECB<BlockCipher::AES, Padding::ZeroNulls>>(key, keyLen);
    } else if (padding == "no") {
      return std::make_unique<Mode::ECB<BlockCipher::AES, Padding::NoPadding>>(key, keyLen);
    }
    return std::make_unique<Mode::ECB<BlockCipher::AES, Padding::PKCS_5_7>>(key, keyLen);
  } else if (mode == "cbc") {
    if (padding == "ansi_x9.23") {
      return std::make_unique<Mode::CBC<BlockCipher::AES, Padding::ANSI_X9_23>>(key, keyLen);
    } else if (padding == "iso/iec_7816-4") {
      return std::make_unique<Mode::CBC<BlockCipher::AES, Padding::ISO_IEC_7816_4>>(key, keyLen);
    } else if (padding == "zero") {
      return std::make_unique<Mode::CBC<BlockCipher::AES, Padding::ZeroNulls>>(key, keyLen);
    } else if (padding == "no") {
      return std::make_unique<Mode::CBC<BlockCipher::AES, Padding::NoPadding>>(key, keyLen);
    }
    return std::make_unique<Mode::CBC<BlockCipher::AES, Padding::PKCS_5_7>>(key, keyLen);
  } else if (mode == "cfb") {
    if (padding == "ansi_x9.23") {
      return std::make_unique<Mode::CFB<BlockCipher::AES, Padding::ANSI_X9_23>>(key, keyLen);
    } else if (padding == "iso/iec_7816-4") {
      return std::make_unique<Mode::CFB<BlockCipher::AES, Padding::ISO_IEC_7816_4>>(key, keyLen);
    } else if (padding == "zero") {
      return std::make_unique<Mode::CFB<BlockCipher::AES, Padding::ZeroNulls>>(key, keyLen);
    } else if (padding == "no") {
      return std::make_unique<Mode::CBC<BlockCipher::AES, Padding::NoPadding>>(key, keyLen);
    }
    return std::make_unique<Mode::CFB<BlockCipher::AES, Padding::PKCS_5_7>>(key, keyLen);
  }
  throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Unsupported cipher mode"));
}

void processEncryptionOptions(
  jsi::Runtime& runtime,
  const std::string& propName,
  jsi::Object& optionsObject,
  bool &encrypted,
  int &passphraseLength,
  std::vector<unsigned char> &passphrase,
  int &ivLength,
  std::vector<unsigned char> &iv,
  std::string &mode,
  std::string &padding
) {
  if (optionsObject.hasProperty(runtime, "encrypted")) {
    auto encryptedOption = optionsObject.getProperty(runtime, "encrypted");
    if (encryptedOption.isBool()) {
      encrypted = encryptedOption.asBool();
    }
  }
  if (optionsObject.hasProperty(runtime, "passphrase")) {
    auto passphraseOption = optionsObject.getProperty(runtime, "passphrase");
    if (passphraseOption.isString()) {
      std::string passphraseString = passphraseOption.asString(runtime).utf8(runtime);
      passphraseLength = passphraseString.size();
      passphrase.assign(passphraseString.begin(), passphraseString.end());
    } else if (passphraseOption.isObject()) {
      jsi::Array passphraseArr = passphraseOption.asObject(runtime).asArray(runtime);
      if (!passphraseArr.isArray(runtime)) {
        throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Passphrase has to be of type number[]"));
      }
      passphraseLength = passphraseArr.size(runtime);
      for (int i = 0; i < passphraseLength; i++) {
        passphrase.push_back(static_cast<unsigned char>(
          passphraseArr.getValueAtIndex(runtime, i).asNumber()
        ));
      }
    } else {
      throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Passphrase has to be of type string or number[]"));
    }
  }
  if (encrypted && propName != "readFile" && propName != "writeFile") {
    throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Encryption is only available for the readFile and writeFile methods"));
  }
  if (encrypted && passphraseLength != 16 && passphraseLength != 25 && passphraseLength != 32) {
    throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Passphrase length should be 16, 24 or 32"));
  }
  if (optionsObject.hasProperty(runtime, "iv")) {
    auto ivOption = optionsObject.getProperty(runtime, "iv");
    if (ivOption.isString()) {
      std::string ivString = ivOption.asString(runtime).utf8(runtime);
      ivLength = ivString.size();
      iv.assign(ivString.begin(), ivString.end());
    } else if (ivOption.isObject()) {
      jsi::Array ivArr = ivOption.asObject(runtime).asArray(runtime);
      if (!ivArr.isArray(runtime)) {
        throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "IV has to be of type number[]"));
      }
      ivLength = ivArr.size(runtime);
      for (int i = 0; i < ivLength; i++) {
        iv.push_back(static_cast<unsigned char>(
          ivArr.getValueAtIndex(runtime, i).asNumber()
        ));
      }
      
    } else {
      throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "IV has to be of type string or number[]"));
    }
    if (ivLength != 16) {
      throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "IV length should be equal 16"));
    }
  }
  if (optionsObject.hasProperty(runtime, "mode")) {
    auto modeOption = optionsObject.getProperty(runtime, "mode");
    if (modeOption.isString()) {
      mode = modeOption.asString(runtime).utf8(runtime);
    }
    if (mode != "ecb" && mode != "cbc" && mode != "cfb") {
      throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Only ecb, cbc and cfb modes allowed"));
    }
    if ((mode == "cbc" || mode == "cfb") && ivLength == 0) {
      throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "IV required for cbc and cfb"));
    }
  }
  if (optionsObject.hasProperty(runtime, "padding")) {
    auto paddingOption = optionsObject.getProperty(runtime, "padding");
    if (paddingOption.isString()) {
      padding = paddingOption.asString(runtime).utf8(runtime);
    }
    if (
      padding != "ansi_x9.23"
      && padding != "iso/iec_7816-4"
      && padding != "pkcs5/pkcs7"
      && padding != "zero"
      && padding != "no"
    ) {
      throw jsi::JSError(runtime, RNFSTurboLogger::sprintf("%s: %s", propName.c_str(), "Only ansi_x9.23, iso/iec_7816-4, pkcs5/pkcs7, zero and no paddings allowed"));
    }
  }
}

}

#endif
