//
//  NativeRNFSTurboModule.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#pragma once

#if __has_include(<React-Codegen/RNFSTurboSpecJSI.h>)
// CocoaPods include (iOS)
#include <React-Codegen/RNFSTurboSpecJSI.h>
#elif __has_include(<RNFSTurboSpecJSI.h>)
// CMake include on Android
#include <RNFSTurboSpecJSI.h>
#else
#error Cannot find react-native-fs-turbo spec! Try cleaning your cache and re-running CodeGen!
#endif

namespace facebook::react {

// The TurboModule itself
class NativeRNFSTurboModule : public NativeRNFSTurboModuleCxxSpec<NativeRNFSTurboModule> {
public:
  NativeRNFSTurboModule(std::shared_ptr<CallInvoker> jsInvoker);
  ~NativeRNFSTurboModule();

  jsi::Object createRNFSTurbo(jsi::Runtime& runtime);
};

} // namespace facebook::react