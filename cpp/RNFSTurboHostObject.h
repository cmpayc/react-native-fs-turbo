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
#include "filesystem/helpers.h"
#include "NativeRNFSTurboModule.h"
#include "RNFSTurboPlatformHelper.h"

using namespace facebook;

class RNFSTurboHostObject : public jsi::HostObject {
public:
  RNFSTurboHostObject();
  ~RNFSTurboHostObject();

public:
  jsi::Value get(jsi::Runtime&, const jsi::PropNameID& name) override;
  std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime& rt) override;

private:
  RNFSTurboPlatformHelper* platformHelper;
};
