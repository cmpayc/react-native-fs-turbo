//
//  JNIOnLoad.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 20.10.25.
//

#include "RNFSTurboModule.h"
#include <fbjni/fbjni.h>
#include <jni.h>

using namespace cmpayc::rnfsturbo;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return facebook::jni::initialize(vm, [] {
    RNFSTurboModule::registerNatives();
  });
}
