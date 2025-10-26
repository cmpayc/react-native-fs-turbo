//
//  RNFSTurboHostObject.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#pragma once

#include <dirent.h>
#include <filesystem>
#include <jsi/jsi.h>
#include <map>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <utime.h>
#include <vector>
#include <ReactCommon/CallInvoker.h>
#ifdef __ANDROID__
#include <fbjni/fbjni.h>
#include <jni.h>
#endif
#include "algorithms/base64.h"
#include "algorithms/md5.h"
#include "algorithms/sha1.h"
#include "algorithms/sha224.h"
#include "algorithms/sha256.h"
#include "algorithms/sha384.h"
#include "algorithms/sha512.h"
#ifdef RNFSTURBO_USE_ENCRYPTION
#include "algorithms/Krypt/aes-config.hpp"
#include "encryption/encryption-utils.h"
#endif
#include "filesystem/filesystem-utils.h"
#include "RNFSTurboPlatformHelper.h"

namespace cmpayc::rnfsturbo {

using namespace facebook;

class RNFSTurboHostObject : public jsi::HostObject {
public:
  RNFSTurboHostObject(std::shared_ptr<react::CallInvoker> jsInvoker);
  ~RNFSTurboHostObject();

public:
  jsi::Value get(jsi::Runtime&, const jsi::PropNameID& name) override;
  std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime& rt) override;
  std::shared_ptr<react::CallInvoker> _jsInvoker;

private:
  RNFSTurboPlatformHelper* platformHelper;
};

}
