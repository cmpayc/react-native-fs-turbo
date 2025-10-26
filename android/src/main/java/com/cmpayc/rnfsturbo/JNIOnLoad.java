//
//  JNIOnLoad.java
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 20.10.25.
//

package com.cmpayc.rnfsturbo;

import android.util.Log;

public class JNIOnLoad {
  private static final String TAG = "RNFSTurboModule";

  private static boolean isInitialized = false;

  public synchronized static void initializeNativeRNFSTurboModule() {
    if (isInitialized) return;
    try {
      System.loadLibrary("RNFSTurbo");
      isInitialized = true;
    } catch (Throwable e) {
      Log.e(TAG, "Failed to load RNFSTurbo C++ library", e);
      throw e;
    }
  }
}
