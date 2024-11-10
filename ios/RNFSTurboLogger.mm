//
//  RNFSTurboLogger.mm
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#import "RNFSTurboLogger.h"
#import <Foundation/Foundation.h>

void RNFSTurboLogger::log(const std::string& tag, const std::string& message) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
  NSLog(@"[%s]: %s", tag.c_str(), message.c_str());
#pragma clang diagnostic pop
}
