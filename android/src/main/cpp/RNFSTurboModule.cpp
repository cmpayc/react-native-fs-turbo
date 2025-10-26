//
//  RNFSTurboModule.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 19.10.25.
//

#include "RNFSTurboModule.h"
#include "RNFSTurboInstall.h"
#include "RNFSTurboLogger.h"

#include <exception>

namespace cmpayc::rnfsturbo {

jni::local_ref<RNFSTurboModule::jhybriddata> RNFSTurboModule::initHybrid(jni::alias_ref<RNFSTurboModule::jhybridobject>) {
  return makeCxxInstance();
}

bool RNFSTurboModule::createRNFSTurbo(jlong runtimePointer, jni::alias_ref<react::CallInvokerHolder::javaobject> callInvokerHolder) {
  auto runtime = reinterpret_cast<jsi::Runtime*>(runtimePointer);
  if (runtime == nullptr) {
    throw std::invalid_argument("jsi::Runtime not found");
  }

  if (callInvokerHolder == nullptr) {
    throw std::invalid_argument("CallInvoker not found");
  }
  auto callInvoker = callInvokerHolder->cthis()->getCallInvoker();
  if (callInvoker == nullptr) {
    throw std::invalid_argument("CallInvoker not found");
  }

  return cmpayc::rnfsturbo::install(*runtime, callInvoker);
}

void RNFSTurboModule::registerNatives() {
  registerHybrid({
    makeNativeMethod("initHybrid", RNFSTurboModule::initHybrid),
    makeNativeMethod("createRNFSTurbo", RNFSTurboModule::createRNFSTurbo),
  });
}

} // namespace cmpayc::rnfsturbo
