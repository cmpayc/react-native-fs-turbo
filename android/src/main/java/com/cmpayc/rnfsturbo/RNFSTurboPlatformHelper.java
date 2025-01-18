/*
The MIT License (MIT)

Copyright (c) 2015 Johannes Lumpe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Modified by Sergei Kazakov on 31.10.24.
*/

package com.cmpayc.rnfsturbo;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Environment;
import android.os.StatFs;
import android.util.SparseArray;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class RNFSTurboPlatformHelper {

  public static String tag = "RNFSTurboPlatformHelper";

  public static SparseArray<RNFSTurboDownloader> downloaders = new SparseArray<>();
  public static SparseArray<RNFSTurboUploader> uploaders = new SparseArray<>();

  public static native void downloadCompleteCallback(int jobId, int statusCode, double bytesWritten);

  public static native void downloadErrorCallback(int jobId, String error);

  public static native void downloadBeginCallback(int jobId, int statusCode, double contentLength, HashMap<String, String> headers);

  public static native void downloadProgressCallback(int jobId, double contentLength, double bytesWritten);

  public static native void uploadCompleteCallback(int jobId, int statusCode, HashMap<String, String> headers, String body);

  public static native void uploadErrorCallback(int jobId, String error);

  public static native void uploadBeginCallback(int jobId);

  public static native void uploadProgressCallback(int jobId, double totalBytesExpectedToSend, double totalBytesSent);

  public static native void scanCallback(int jobId, String path);

  private Context context;

  public RNFSTurboPlatformHelper(Context ctx) {
    context = ctx;
  }

  public String[] readDirAssets(String dirPath) {
    ArrayList<String> filesList = new ArrayList<String>();
    try {
      AssetManager assetManager = context.getAssets();
      String[] list = assetManager.list(dirPath);

      for (String childFile : list) {
        filesList.add(childFile);
        String path = dirPath.isEmpty() ? childFile : String.format("%s/%s", dirPath, childFile); // don't allow / at the start when directory is ""
        filesList.add(path);
        int length = 0;
        boolean isDirectory = true;
        try {
          AssetFileDescriptor assetFileDescriptor = assetManager.openFd(path);
          if (assetFileDescriptor != null) {
            length = (int) assetFileDescriptor.getLength();
            assetFileDescriptor.close();
            isDirectory = false;
          }
        } catch (IOException ex) {
          isDirectory = !ex.getMessage().contains("compressed");
        }
        filesList.add(String.valueOf(length));
        filesList.add(isDirectory ? "1" : "0");
      }
    } catch (IOException e) {
      String[] files = new String[1];
      files[0] = "-1";
      return files;
    }
    return filesList.toArray(new String[0]);
  }

  public byte[] readFileAssetsOrRes(String filePath, boolean isRes) throws Exception {
    InputStream stream = null;
    try {
      if (isRes) {
        int res = getResIdentifier(filePath);
        stream = context.getResources().openRawResource(res);
        if (stream == null) {
          throw new Exception("Failed to open file");
        }
      } else {
        AssetManager assetManager = context.getAssets();
        stream = assetManager.open(filePath, 0);
        if (stream == null) {
          throw new Exception("Failed to open file");
        }
      }

      byte[] buffer = new byte[stream.available()];
      stream.read(buffer);

      return buffer;
    } catch (Exception ex) {
      ex.printStackTrace();
      throw new Exception("Failed to open file");
    } finally {
      if (stream != null) {
        try {
          stream.close();
        } catch (IOException ignored) {
        }
      }
    }
  }

  public void copyFileAssetsOrRes(String filePath, String destPath, boolean isRes) throws Exception {
    InputStream stream = null;
    try {
      if (isRes) {
        int res = getResIdentifier(filePath);
        stream = context.getResources().openRawResource(res);
      } else {
        AssetManager assetManager = context.getAssets();
        stream = assetManager.open(filePath);
      }
      copyInputStream(stream, filePath, destPath);
    } catch (Exception ex) {
      // Default error message is just asset name, so make a more helpful error here.
      throw new Exception(ex.getLocalizedMessage());
    } finally {
      if (stream != null) {
        try {
          stream.close();
        } catch (IOException ignored) {
        }
      }
    }
  }

  public boolean existsAssetsOrRes(String filePath, boolean isRes) throws Exception {
    InputStream stream = null;
    try {
      if (isRes) {
        int res = getResIdentifier(filePath);
        if (res > 0) {
          return true;
        } else {
          return false;
        }
      } else {
        AssetManager assetManager = context.getAssets();
        try {
          String[] list = assetManager.list(filePath);
          if (list != null && list.length > 0) {
            return true;
          }
        } catch (Exception ignored) {
          //.. probably not a directory then
        }
        // Attempt to open file (win = exists)
        stream = assetManager.open(filePath);
        return true;
      }
    } catch (Exception ex) {
      return false;
    } finally {
      if (stream != null) {
        try {
          stream.close();
        } catch (IOException ignored) {
        }
      }
    }
  }

  public void downloadFile(
    int jobId,
    String fromUrl,
    String toFile,
    HashMap<String, String> headers,
    int progressInterval,
    float progressDivider,
    int connectionTimeout,
    int readTimeout,
    boolean hasBeginCallback,
    boolean hasProgressCallback
  ) {
    try {
      File file = new File(toFile);
      URL url = new URL(fromUrl);

      RNFSTurboDownloadParams params = new RNFSTurboDownloadParams();

      params.src = url;
      params.dest = file;
      params.headers = headers;
      params.progressInterval = progressInterval;
      params.progressDivider = progressDivider;
      params.readTimeout = readTimeout;
      params.connectionTimeout = connectionTimeout;

      params.onTaskCompleted = new RNFSTurboDownloadParams.OnTaskCompleted() {
        public void onTaskCompleted(RNFSTurboDownloadResult res) {
          RNFSTurboDownloader downloader = downloaders.get(jobId);
          if (downloader != null) {
            if (res.exception == null) {
              downloadCompleteCallback(jobId, res.statusCode, (double)res.bytesWritten);
            } else {
              downloadErrorCallback(jobId, res.exception.toString());
            }
            downloaders.remove(jobId);
          }
        }
      };

      if (hasBeginCallback) {
        params.onDownloadBegin = new RNFSTurboDownloadParams.OnDownloadBegin() {
          @Override
          public void onDownloadBegin(int statusCode, long contentLength, Map<String, String> headers) {
            RNFSTurboDownloader downloader = downloaders.get(jobId);
            if (downloader != null) {
              downloadBeginCallback(jobId, statusCode, (double)contentLength, new HashMap<String, String>(headers));
            }
          }
        };
      }

      if (hasProgressCallback) {
        params.onDownloadProgress = new RNFSTurboDownloadParams.OnDownloadProgress() {
          public void onDownloadProgress(long contentLength, long bytesWritten) {
            RNFSTurboDownloader downloader = downloaders.get(jobId);
            if (downloader != null) {
              downloadProgressCallback(jobId, (double)contentLength, (double)bytesWritten);
            }
          }
        };
      }

      RNFSTurboDownloader downloader = new RNFSTurboDownloader();

      downloader.execute(params);

      downloaders.put(jobId, downloader);
    } catch (Exception ex) {
      ex.printStackTrace();
      downloadErrorCallback(jobId, ex.toString());
    }
  }

  public void stopDownload(int jobId) {
    RNFSTurboDownloader downloader = downloaders.get(jobId);

    if (downloader != null) {
      downloader.stop();
      downloaders.remove(jobId);
    }
  }

  public void uploadFile(
    int jobId,
    String toUrl,
    boolean binaryStreamOnly,
    String[] files,
    int filesNum,
    HashMap<String, String> headers,
    HashMap<String, String> fields,
    String method,
    boolean hasBeginCallback,
    boolean hasProgressCallback
  ) {
    try {
      URL url = new URL(toUrl);
      RNFSTurboUploadParams params = new RNFSTurboUploadParams();
      params.src = url;
      params.files = files;
      params.filesNum = filesNum;
      params.headers = headers;
      params.method = method;
      params.fields = fields;
      params.binaryStreamOnly = binaryStreamOnly;
      params.onUploadComplete = new RNFSTurboUploadParams.onUploadComplete() {
        public void onUploadComplete(RNFSTurboUploadResult res) {
          if (res.exception == null) {
            uploadCompleteCallback(jobId, res.statusCode, new HashMap<String, String>(res.headers), res.body);
          } else {
            uploadErrorCallback(jobId, res.exception.toString());
          }
          uploaders.remove(jobId);
        }
      };

      if (hasBeginCallback) {
        params.onUploadBegin = new RNFSTurboUploadParams.onUploadBegin() {
          public void onUploadBegin() {
            uploadBeginCallback(jobId);
          }
        };
      }

      if (hasProgressCallback) {
        params.onUploadProgress = new RNFSTurboUploadParams.onUploadProgress() {
          public void onUploadProgress(double totalBytesExpectedToSend, double totalBytesSent) {
            uploadProgressCallback(jobId, totalBytesExpectedToSend, totalBytesSent);
          }
        };
      }

      RNFSTurboUploader uploader = new RNFSTurboUploader();

      uploader.execute(params);

      uploaders.put(jobId, uploader);
    } catch (Exception ex) {
      ex.printStackTrace();
      uploadErrorCallback(jobId, ex.toString());
    }
  }

  public void stopUpload(int jobId) {
    RNFSTurboUploader uploader = uploaders.get(jobId);

    if (uploader != null) {
      uploader.stop();
      uploaders.remove(jobId);
    }
  }

  public long[] getFSInfo() {
    File path = Environment.getDataDirectory();
    StatFs stat = new StatFs(path.getPath());
    StatFs statEx = new StatFs(Environment.getExternalStorageDirectory().getPath());
    long[] info = new long[4];
    if (android.os.Build.VERSION.SDK_INT >= 18) {
      info[0] = stat.getTotalBytes();
      info[1] = stat.getFreeBytes();
      info[2] = statEx.getTotalBytes();
      info[3] = statEx.getFreeBytes();
    } else {
      long blockSize = stat.getBlockSize();
      info[0] = blockSize * stat.getBlockCount();
      info[1] = blockSize * stat.getAvailableBlocks();
    }

    return info;
  }

  public void scanFile(int jobId, String path) {
    MediaScannerConnection.scanFile(context,
      new String[]{path},
      null,
      new MediaScannerConnection.MediaScannerConnectionClient() {
        @Override
        public void onMediaScannerConnected() {}
        @Override
        public void onScanCompleted(String path, Uri uri) {
          scanCallback(jobId, uri != null ? uri.toString() : "");
        }
      }
    );
  }

  public String[] getAllExternalFilesDirs() {
    File[] allExternalFilesDirs = context.getExternalFilesDirs(null);
    ArrayList<String> filesList = new ArrayList<String>();
    for (File f : allExternalFilesDirs) {
      if (f != null) {
        filesList.add(f.getAbsolutePath());
      }
    }
    return filesList.toArray(new String[0]);
  }

  private int getResIdentifier(String filename) {
    String suffix = filename.substring(filename.lastIndexOf(".") + 1);
    String name = filename.substring(0, filename.lastIndexOf("."));
    Boolean isImage = suffix.equals("png") || suffix.equals("jpg") || suffix.equals("jpeg") || suffix.equals("bmp") || suffix.equals("gif") || suffix.equals("webp") || suffix.equals("psd") || suffix.equals("svg") || suffix.equals("tiff");
    return context.getResources().getIdentifier(name, isImage ? "drawable" : "raw", context.getPackageName());
  }

  private Uri getFileUri(String filepath, boolean isDirectoryAllowed) throws RNFSTurboIORejectionException {
    Uri uri = Uri.parse(filepath);
    if (uri.getScheme() == null) {
      // No prefix, assuming that provided path is absolute path to file
      File file = new File(filepath);
      if (!isDirectoryAllowed && file.isDirectory()) {
        throw new RNFSTurboIORejectionException("EISDIR", "EISDIR: illegal operation on a directory, read '" + filepath + "'");
      }
      uri = Uri.parse("file://" + filepath);
    }
    return uri;
  }

  private String getWriteAccessByAPILevel() {
    return android.os.Build.VERSION.SDK_INT <= android.os.Build.VERSION_CODES.P ? "w" : "rwt";
  }

  private OutputStream getOutputStream(String filepath, boolean append) throws RNFSTurboIORejectionException {
    Uri uri = getFileUri(filepath, false);
    OutputStream stream;
    try {
      stream = context.getContentResolver().openOutputStream(uri, append ? "wa" : getWriteAccessByAPILevel());
    } catch (FileNotFoundException ex) {
      throw new RNFSTurboIORejectionException("ENOENT", "ENOENT: " + ex.getMessage() + ", open '" + filepath + "'");
    }
    if (stream == null) {
      throw new RNFSTurboIORejectionException("ENOENT", "ENOENT: could not open an output stream for '" + filepath + "'");
    }
    return stream;
  }

  /**
   * Internal method for copying that works with any InputStream
   *
   * @param in          InputStream from assets or file
   * @param source      source path (only used for logging errors)
   * @param destination destination path
   */
  private void copyInputStream(InputStream in, String source, String destination) throws Exception {
    OutputStream out = null;
    try {
      out = getOutputStream(destination, false);

      byte[] buffer = new byte[1024 * 10]; // 10k buffer
      int read;
      while ((read = in.read(buffer)) != -1) {
        out.write(buffer, 0, read);
      }
    } catch (Exception ex) {
      throw new Exception(String.format("Failed to copy '%s' to %s (%s)", source, destination, ex.getLocalizedMessage()));
    } finally {
      if (in != null) {
        try {
          in.close();
        } catch (IOException ignored) {
        }
      }
      if (out != null) {
        try {
          out.close();
        } catch (IOException ignored) {
        }
      }
    }
  }
}
