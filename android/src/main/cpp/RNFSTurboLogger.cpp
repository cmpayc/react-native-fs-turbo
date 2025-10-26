//
//  RNFSTurboLogger.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#import "RNFSTurboLogger.h"
#include <android/log.h>

namespace cmpayc::rnfsturbo {

void RNFSTurboLogger::log(const std::string& tag, const std::string& message) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
  __android_log_print(ANDROID_LOG_INFO, tag.c_str(), message.c_str());
#pragma clang diagnostic pop
}

}
