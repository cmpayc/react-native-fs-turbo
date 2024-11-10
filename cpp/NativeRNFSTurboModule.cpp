//
//  NativeRNFSTurboModule.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#include "NativeRNFSTurboModule.h"
#include "RNFSTurboHostObject.h"

namespace facebook::react {

NativeRNFSTurboModule::NativeRNFSTurboModule(std::shared_ptr<CallInvoker> jsInvoker)
    : NativeRNFSTurboModuleCxxSpec(jsInvoker) {}

NativeRNFSTurboModule::~NativeRNFSTurboModule() {}

jsi::Object NativeRNFSTurboModule::createRNFSTurbo(jsi::Runtime& runtime, RNFSTurboConfig config) {
  auto instance = std::make_shared<RNFSTurboHostObject>(config);
  return jsi::Object::createFromHostObject(runtime, instance);
}

} // namespace facebook::react