//
//  RNFSTurboModule.java
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 20.10.25.
//

package com.cmpayc.rnfsturbo;

import android.os.Environment;
import android.content.Context;
import androidx.annotation.Keep;
import java.io.File;

import com.facebook.jni.HybridData;
import com.facebook.proguard.annotations.DoNotStrip;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.common.annotations.FrameworkAPI;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

@DoNotStrip
@Keep
@FrameworkAPI
public class RNFSTurboModule extends RNFSTurboSpec {
  private final HybridData mHybridData;
  private final ReactApplicationContext context;

  public static Context externalContext;
  public static final String NAME = "RNFSTurboModule";

  static {
    // Make sure RNFSTurbo's C++ library is loaded
    JNIOnLoad.initializeNativeRNFSTurboModule();
  }

  public RNFSTurboModule(ReactApplicationContext context) {
    super(context);
    this.context = context;
    externalContext = context;
    this.mHybridData = initHybrid();
  }

  public static Context getContext() {
    return externalContext;
  }

  @Override
  public String getName() {
    return NAME;
  }

  @Override
  public boolean createRNFSTurbo() {
    try {
      // 1. Get jsi::Runtime pointer
      if (this.context.getJavaScriptContextHolder() == null) {
        return false;
      }
      long jsContext = this.context.getJavaScriptContextHolder().get();

      // 2. Get CallInvokerHolder
      CallInvokerHolder callInvokerHolderBase = this.context.getJSCallInvokerHolder();
      if (!(callInvokerHolderBase instanceof CallInvokerHolderImpl)) {
        return false;
      }
      CallInvokerHolderImpl callInvokerHolder = (CallInvokerHolderImpl) callInvokerHolderBase;

      // 3. Create RNFSTurbo instance
      return createRNFSTurbo(jsContext, callInvokerHolder);
    } catch (Throwable e) {
      return false;
    }
  }

  @Override
  public String getMainBundlePath() {
    return "";
  }

  @Override
  public String getCachesDirectoryPath() {
    return this.context.getCacheDir().getAbsolutePath();
  }

  @Override
  public String getDocumentDirectoryPath() {
    return this.context.getFilesDir().getAbsolutePath();
  }

  @Override
  public String getTemporaryDirectoryPath() {
    return this.context.getCacheDir().getAbsolutePath();
  }

  @Override
  public String getLibraryDirectoryPath() {
    return "";
  }

  @Override
  public String getExternalDirectoryPath() {
    File externalDirectory = this.context.getExternalFilesDir(null);
    if (externalDirectory != null) {
      return externalDirectory.getAbsolutePath();
    }
    return "";
  }

  @Override
  public String getExternalStorageDirectoryPath() {
    File externalStorageDirectory = Environment.getExternalStorageDirectory();
    if (externalStorageDirectory != null) {
      return externalStorageDirectory.getAbsolutePath();
    }
    return "";
  }

  @Override
  public String getExternalCachesDirectoryPath() {
    File externalCachesDirectory = this.context.getExternalCacheDir();
    if (externalCachesDirectory != null) {
      return externalCachesDirectory.getAbsolutePath();
    }
    return "";
  }

  @Override
  public String getDownloadDirectoryPath() {
    return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();
  }

  @Override
  public String getPicturesDirectoryPath() {
    return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getAbsolutePath();
  }

  @Override
  public String getRoamingDirectoryPath() {
    return "";
  }

  private native HybridData initHybrid();

  private native boolean createRNFSTurbo(
    long jsRuntimePointer,
    CallInvokerHolderImpl callInvokerHolder
  );
}
