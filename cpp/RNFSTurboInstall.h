//
//  RNFSTurboInstall.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 20.10.25
//

#pragma once

#include <jsi/jsi.h>
#include <memory>
#include <ReactCommon/CallInvoker.h>

namespace cmpayc::rnfsturbo {

using namespace facebook;

bool install(jsi::Runtime& runtime, std::shared_ptr<react::CallInvoker>& callInvoker);

} // namespace cmpayc::rnfsturbo
