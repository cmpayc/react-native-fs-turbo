
//
//  RNFSTurboPlatformContextModule.java
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

package com.cmpayc.rnfsturbo;

import android.content.Context;
import android.os.Environment;

import java.io.File;

import com.facebook.react.bridge.ReactApplicationContext;

public class RNFSTurboPlatformContextModule extends NativeRNFSTurboPlatformContextModuleSpec {
  private final ReactApplicationContext context;

  public static Context externalContext;

  public RNFSTurboPlatformContextModule(ReactApplicationContext reactContext) {
    super(reactContext);
    context = reactContext;
    externalContext = reactContext;
  }

  public static Context getContext() {
    return externalContext;
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
}
