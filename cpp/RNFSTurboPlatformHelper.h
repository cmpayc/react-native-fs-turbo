//
//  RNFSTurboPlatformHelper.h
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <thread>

typedef std::function<void (int jobId, int statusCode, float bytesWritten)> RNFSTurboCompleteDownloadCallback;

typedef std::function<void (int jobId, const char* errorMessage)> RNFSTurboErrorCallback;

typedef std::function<void (int jobId, int statusCode, float contentLength, std::map<std::string, std::string> headers)> RNFSTurboBeginDownloadCallback;

typedef std::function<void (int jobId, float contentLength, float bytesWritten)> RNFSTurboProgressDownloadCallback;

typedef std::function<void (int jobId)> RNFSTurboResumableDownloadCallback;

typedef std::function<void (int jobId, int statusCode, std::map<std::string, std::string> headers, const char* body)> RNFSTurboCompleteUploadCallback;

typedef std::function<void (int jobId)> RNFSTurboBeginUploadCallback;

typedef std::function<void (int jobId, float totalBytesExpectedToSend, float totalBytesSent)> RNFSTurboProgressUploadCallback;

typedef std::function<void (int jobId, std::string path)> RNFSTurboScanCallback;

struct ReadDirItem {
  std::string name;
  std::string path;
  float size;
  bool isDirectory;
};

struct UploadFileItem {
  std::string name;
  std::string filename;
  std::string filepath;
  std::string filetype;
};

struct FSInfo {
  unsigned long long totalSpace;
  unsigned long long freeSpace;
  std::optional<unsigned long long> totalSpaceEx;
  std::optional<unsigned long long> freeSpaceEx;
};

struct DownloadCallbacks {
  RNFSTurboCompleteDownloadCallback completeCallback;
  RNFSTurboErrorCallback errorCallback;
  RNFSTurboBeginDownloadCallback beginCallback;
  RNFSTurboProgressDownloadCallback progressCallback;
  RNFSTurboResumableDownloadCallback resumableCallback;
  std::chrono::time_point<std::chrono::steady_clock> lastProgressCall{ std::chrono::steady_clock::now() };
};

struct UploadCallbacks {
  RNFSTurboCompleteUploadCallback completeCallback;
  RNFSTurboErrorCallback errorCallback;
  RNFSTurboBeginUploadCallback beginCallback;
  RNFSTurboProgressUploadCallback progressCallback;
  std::chrono::time_point<std::chrono::steady_clock> lastProgressCall{ std::chrono::steady_clock::now() };
};

class RNFSTurboPlatformHelper {
public:
  static inline int jobId{0};
#ifdef __ANDROID__
public:
  RNFSTurboPlatformHelper(JNIEnv *env);
  static inline std::map<int, DownloadCallbacks> downloadCallbacks;
  static inline std::map<int, UploadCallbacks> uploadCallbacks;
  static inline std::map<int, RNFSTurboScanCallback> scanCallbacks;
private:
  JNIEnv *jniEnv;
  jobject jniObj;
#endif
#ifdef __APPLE__
public:
  RNFSTurboPlatformHelper();
  static inline NSMutableDictionary* downloaders;
  static inline NSMutableDictionary* uuids;
  static inline NSMutableDictionary* uploaders;
#endif
public:
  std::vector<ReadDirItem> readDirAssets(const char* dirPath);

  std::string readFileAssetsOrRes(const char* filePath, bool isRes);

  void copyFileAssetsOrRes(const char* filePath, const char* destPath, bool isRes);

  bool existsAssetsOrRes(const char* filePath, bool isRes);

  void copyAssetsFileIOS(const char* imageUri, const char* destPath, int width, int height, float scale, float compression, const char* resizeMode);

  void copyAssetsVideoIOS(const char* videoUri, const char* destPath);

  void downloadFile(
    int jobId,
    const char* fromUrl,
    const char* toFile,
    std::map<std::string, std::string> headers,
    bool background,
    bool discretionary,
    bool cacheable,
    int progressInterval,
    float progressDivider,
    int connectionTimeout,
    int readTimeout,
    int backgroundTimeout,
    RNFSTurboCompleteDownloadCallback completeCallback,
    RNFSTurboErrorCallback errorCallback,
    std::optional<RNFSTurboBeginDownloadCallback> beginCallback = std::nullopt,
    std::optional<RNFSTurboProgressDownloadCallback> progressCallback = std::nullopt,
    std::optional<RNFSTurboResumableDownloadCallback> resumableCallback = std::nullopt
  );

  void stopDownload(int jobId);

  void resumeDownload(int jobId);

  bool isResumable(int jobId);
  
  void completeHandlerIOS(int jobId);
  
  void uploadFiles(
    int jobId,
    const char* toUrl,
    bool binaryStreamOnly,
    UploadFileItem files[],
    int filesNum,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> fields,
    const char* method,
    RNFSTurboCompleteUploadCallback completeCallback,
    RNFSTurboErrorCallback errorCallback,
    std::optional<RNFSTurboBeginUploadCallback> beginCallback = std::nullopt,
    std::optional<RNFSTurboProgressUploadCallback> progressCallback = std::nullopt
  );
  
  void stopUpload(int jobId);
  
  FSInfo getFSInfo();

  void scanFile(int jobId, const char* path, RNFSTurboScanCallback scanCallback);

  std::vector<std::string> getAllExternalFilesDirs();
  
  const char* pathForGroup(const char* groupIdentifier);
  
  void setResourceValue(const char* path, const char* optionType, const char* optionValue);
};
