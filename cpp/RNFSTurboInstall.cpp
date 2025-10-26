//
//  RNFSTurboInstall.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 20.10.25
//

#include "RNFSTurboInstall.h"
#include "RNFSTurboHostObject.h"

namespace cmpayc::rnfsturbo {

bool install(jsi::Runtime& runtime, std::shared_ptr<react::CallInvoker>& callInvoker) {
  if (runtime.global().hasProperty(runtime, "RNFSTurboProxy")) [[unlikely]] {
    return true;
  }

  std::shared_ptr<RNFSTurboHostObject> instance = std::make_shared<RNFSTurboHostObject>(callInvoker);

  jsi::Object result = jsi::Object::createFromHostObject(runtime, instance);

  runtime.global().setProperty(runtime, "RNFSTurboProxy", std::move(result));

  return true;
}

} // namespace cmpayc::rnfsturbo
