//
//  RNFSTurboOnLoad.mm
//  react-native-fa-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#import "NativeRNFSTurboModule.h"
#import <Foundation/Foundation.h>
#import <ReactCommon/CxxTurboModuleUtils.h>

@interface RNFSTurboOnLoad : NSObject
@end

@implementation RNFSTurboOnLoad

+ (void)load {
  facebook::react::registerCxxModuleToGlobalModuleMap(
      std::string(facebook::react::NativeRNFSTurboModule::kModuleName),
      [&](std::shared_ptr<facebook::react::CallInvoker> jsInvoker) {
        return std::make_shared<facebook::react::NativeRNFSTurboModule>(jsInvoker);
      });
}

@end
