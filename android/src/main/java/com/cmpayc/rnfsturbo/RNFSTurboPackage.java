//
//  RNFSTurboPackage.java
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

package com.cmpayc.rnfsturbo;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.OptIn;
import java.util.HashMap;
import java.util.Map;

import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.common.annotations.FrameworkAPI;
import com.facebook.react.module.model.ReactModuleInfo;
import com.facebook.react.module.model.ReactModuleInfoProvider;
import com.facebook.react.TurboReactPackage;

@OptIn(markerClass = FrameworkAPI.class)
public class RNFSTurboPackage extends TurboReactPackage {
  @Nullable
  @Override
  public NativeModule getModule(String name, @NonNull ReactApplicationContext reactContext) {
    if (name.equals(RNFSTurboModule.NAME)) {
      return new RNFSTurboModule(reactContext);
    } else {
      return null;
    }
  }

  @Override
  public ReactModuleInfoProvider getReactModuleInfoProvider() {
    return () -> {
      final Map<String, ReactModuleInfo> moduleInfos = new HashMap<>();
      moduleInfos.put(
        RNFSTurboModule.NAME,
        new ReactModuleInfo(
          RNFSTurboModule.NAME,
          RNFSTurboModule.NAME,
          false, // canOverrideExistingModule
          false, // needsEagerInit
          false, // hasConstants
          false, // isCxxModule
          true // isTurboModule
        )
      );
      return moduleInfos;
    };
  }
}
