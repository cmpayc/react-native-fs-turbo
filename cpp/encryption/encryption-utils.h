//
//  helpers.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 29.04.25.
//

#pragma once
#ifdef RNFSTURBO_USE_ENCRYPTION
#include <jsi/jsi.h>
#include "../algorithms/Krypt/AES.hpp"
#include "../algorithms/Krypt/mode.hpp"

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
);

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
);

}

#endif
