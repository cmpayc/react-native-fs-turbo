//
//  RNFSTurboModule.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 20.10.25.
//

#pragma once

#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <jsi/jsi.h>

namespace cmpayc::rnfsturbo {

using namespace facebook;

class RNFSTurboModule final : public jni::HybridClass<RNFSTurboModule> {
public:
  static auto constexpr kJavaDescriptor = "Lcom/cmpayc/rnfsturbo/RNFSTurboModule;";

private:
  explicit RNFSTurboModule() = default;

private:
  static jni::local_ref<RNFSTurboModule::jhybriddata> initHybrid(jni::alias_ref<jhybridobject> javaThis);

  bool createRNFSTurbo(jlong runtimePointer, jni::alias_ref<react::CallInvokerHolder::javaobject> callInvokerHolder);

private:
  static auto constexpr TAG = "RNFSTurboModule";
  using HybridBase::HybridBase;
  friend HybridBase;

public:
  static void registerNatives();
};

} // namespace cmpayc::rnfsturbo
