//
//  RNFSTurboPackage.java
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

package com.cmpayc.rnfsturbo;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.module.model.ReactModuleInfo;
import com.facebook.react.module.model.ReactModuleInfoProvider;
import com.facebook.react.TurboReactPackage;

import java.util.HashMap;
import java.util.Map;

public class RNFSTurboPackage extends TurboReactPackage {
  @Nullable
  @Override
  public NativeModule getModule(String name, @NonNull ReactApplicationContext reactContext) {
    if (name.equals(RNFSTurboPlatformContextModule.NAME)) {
      return new RNFSTurboPlatformContextModule(reactContext);
    } else {
      return null;
    }
  }

  @Override
  public ReactModuleInfoProvider getReactModuleInfoProvider() {
    return () -> {
      final Map<String, ReactModuleInfo> moduleInfos = new HashMap<>();
      moduleInfos.put(
        RNFSTurboPlatformContextModule.NAME,
        new ReactModuleInfo(
          RNFSTurboPlatformContextModule.NAME,
          RNFSTurboPlatformContextModule.NAME,
          false, // canOverrideExistingModule
          false, // needsEagerInit
          true, // hasConstants
          false, // isCxxModule
          true // isTurboModule
        )
      );
      return moduleInfos;
    };
  }
}
