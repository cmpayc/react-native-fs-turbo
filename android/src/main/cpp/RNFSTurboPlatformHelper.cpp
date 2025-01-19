//
//  RNFSTurboPlatformHelper.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 16.09.24.
//

#include <jni.h>
#include "RNFSTurboPlatformHelper.h"
#include "RNFSTurboLogger.h"

void JavaHashMapToStlStringStringMap(JNIEnv *env, jobject hashMap, std::map<std::string, std::string>& mapOut) {
  // Get the Map's entry Set.
  jclass mapClass = env->FindClass("java/util/Map");
  if (mapClass == nullptr) {
    env->DeleteLocalRef(hashMap);
    return;
  }
  jmethodID entrySet =
    env->GetMethodID(mapClass, "entrySet", "()Ljava/util/Set;");
  if (entrySet == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    return;
  }
  jobject set = env->CallObjectMethod(hashMap, entrySet);
  if (set == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    return;
  }
  // Obtain an iterator over the Set
  jclass setClass = env->FindClass("java/util/Set");
  if (setClass == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    return;
  }
  jmethodID iterator =
    env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
  if (iterator == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    env->DeleteLocalRef(setClass);
    return;
  }
  jobject iter = env->CallObjectMethod(set, iterator);
  if (iter == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    env->DeleteLocalRef(setClass);
    return;
  }
  // Get the Iterator method IDs
  jclass iteratorClass = env->FindClass("java/util/Iterator");
  if (iteratorClass == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    env->DeleteLocalRef(setClass);
    env->DeleteLocalRef(iter);
    return;
  }
  jmethodID hasNext = env->GetMethodID(iteratorClass, "hasNext", "()Z");
  jmethodID next =
    env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
  if (hasNext == nullptr || next == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    env->DeleteLocalRef(setClass);
    env->DeleteLocalRef(iter);
    env->DeleteLocalRef(iteratorClass);
    return;
  }
  // Get the Entry class method IDs
  jclass entryClass = env->FindClass("java/util/Map$Entry");
  if (entryClass == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    env->DeleteLocalRef(setClass);
    env->DeleteLocalRef(iter);
    env->DeleteLocalRef(iteratorClass);
    return;
  }
  jmethodID getKey =
    env->GetMethodID(entryClass, "getKey", "()Ljava/lang/Object;");
  jmethodID getValue =
    env->GetMethodID(entryClass, "getValue", "()Ljava/lang/Object;");
  if (getKey == nullptr || getValue == nullptr) {
    env->DeleteLocalRef(hashMap);
    env->DeleteLocalRef(mapClass);
    env->DeleteLocalRef(set);
    env->DeleteLocalRef(setClass);
    env->DeleteLocalRef(iter);
    env->DeleteLocalRef(iteratorClass);
    env->DeleteLocalRef(entryClass);
    return;
  }
  // Iterate over the entry Set
  while (env->CallBooleanMethod(iter, hasNext)) {
    jobject entry = env->CallObjectMethod(iter, next);
    jstring key = (jstring) env->CallObjectMethod(entry, getKey);
    jstring value = (jstring) env->CallObjectMethod(entry, getValue);
    const char* keyStr = env->GetStringUTFChars(key, nullptr);
    if (!keyStr) {  // Out of memory
      return;
    }
    const char* valueStr = env->GetStringUTFChars(value, nullptr);
    if (!valueStr) {  // Out of memory
      env->ReleaseStringUTFChars(key, keyStr);
      return;
    }

    mapOut.insert(std::make_pair(std::string(keyStr), std::string(valueStr)));

    env->DeleteLocalRef(entry);
    env->ReleaseStringUTFChars(key, keyStr);
    env->DeleteLocalRef(key);
    env->ReleaseStringUTFChars(value, valueStr);
    env->DeleteLocalRef(value);
  }
  env->DeleteLocalRef(hashMap);
  env->DeleteLocalRef(mapClass);
  env->DeleteLocalRef(set);
  env->DeleteLocalRef(setClass);
  env->DeleteLocalRef(iter);
  env->DeleteLocalRef(iteratorClass);
  env->DeleteLocalRef(entryClass);
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_downloadCompleteCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  int statusCode,
  double bytesWritten
) {
  std::map<int, DownloadCallbacks>::iterator it = RNFSTurboPlatformHelper::downloadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::downloadCallbacks.end()) {
    RNFSTurboPlatformHelper::downloadCallbacks[jobId].completeCallback(
      jobId,
      statusCode,
      static_cast<float>(bytesWritten)
    );
    RNFSTurboPlatformHelper::downloadCallbacks.erase(it);
  }
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_downloadErrorCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  jstring errorStr
) {
  std::map<int, DownloadCallbacks>::iterator it = RNFSTurboPlatformHelper::downloadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::downloadCallbacks.end()) {
    const char *error = env->GetStringUTFChars(errorStr, nullptr);
    RNFSTurboPlatformHelper::downloadCallbacks[jobId].errorCallback(jobId, error);
    RNFSTurboPlatformHelper::downloadCallbacks.erase(it);
    env->ReleaseStringUTFChars(errorStr, error);
  }
  env->DeleteLocalRef(errorStr);
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_downloadBeginCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  int statusCode,
  double contentLength,
  jobject headersObj
) {
  std::map<int, DownloadCallbacks>::iterator it = RNFSTurboPlatformHelper::downloadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::downloadCallbacks.end()) {
    std::map<std::string, std::string> headers;
    JavaHashMapToStlStringStringMap(env, headersObj, headers);

    RNFSTurboPlatformHelper::downloadCallbacks[jobId].beginCallback(
      jobId,
      statusCode,
      static_cast<float>(contentLength),
      headers
    );
  }
  env->DeleteLocalRef(headersObj);
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_downloadProgressCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  double contentLength,
  double bytesWritten
) {
  std::map<int, DownloadCallbacks>::iterator it = RNFSTurboPlatformHelper::downloadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::downloadCallbacks.end()) {
    RNFSTurboPlatformHelper::downloadCallbacks[jobId].progressCallback(
      jobId,
      static_cast<float>(contentLength),
      static_cast<float>(bytesWritten)
    );
  }
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_uploadCompleteCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  int statusCode,
  jobject headersObj,
  jstring bodyStr
) {
  std::map<int, UploadCallbacks>::iterator it = RNFSTurboPlatformHelper::uploadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::uploadCallbacks.end()) {
    std::map<std::string, std::string> headers;
    JavaHashMapToStlStringStringMap(env, headersObj, headers);
    const char *body = env->GetStringUTFChars(bodyStr, nullptr);
    RNFSTurboPlatformHelper::uploadCallbacks[jobId].completeCallback(jobId, statusCode, headers, body);
    RNFSTurboPlatformHelper::uploadCallbacks.erase(it);
    env->ReleaseStringUTFChars(bodyStr, body);
  }
  env->DeleteLocalRef(headersObj);
  env->DeleteLocalRef(bodyStr);
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_uploadBeginCallback(
  JNIEnv *env,
  jclass cls,
  int jobId
) {
  std::map<int, UploadCallbacks>::iterator it = RNFSTurboPlatformHelper::uploadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::uploadCallbacks.end()) {
    RNFSTurboPlatformHelper::uploadCallbacks[jobId].beginCallback(jobId);
  }
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_uploadProgressCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  double totalBytesExpectedToSend,
  double totalBytesSent
) {
  std::map<int, UploadCallbacks>::iterator it = RNFSTurboPlatformHelper::uploadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::uploadCallbacks.end()) {
    RNFSTurboPlatformHelper::uploadCallbacks[jobId].progressCallback(
      jobId,
      static_cast<float>(totalBytesExpectedToSend),
      static_cast<float>(totalBytesSent)
    );
  }
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_uploadErrorCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  jstring errorStr
) {
  std::map<int, UploadCallbacks>::iterator it = RNFSTurboPlatformHelper::uploadCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::uploadCallbacks.end()) {
    const char *error = env->GetStringUTFChars(errorStr, nullptr);
    RNFSTurboPlatformHelper::uploadCallbacks[jobId].errorCallback(jobId, error);
    RNFSTurboPlatformHelper::uploadCallbacks.erase(it);
    env->ReleaseStringUTFChars(errorStr, error);
  }
  env->DeleteLocalRef(errorStr);
}

extern "C"
JNIEXPORT void JNICALL Java_com_cmpayc_rnfsturbo_RNFSTurboPlatformHelper_scanCallback(
  JNIEnv *env,
  jclass cls,
  int jobId,
  jstring pathStr
) {
  std::map<int, RNFSTurboScanCallback>::iterator it = RNFSTurboPlatformHelper::scanCallbacks.find(jobId);
  if (it != RNFSTurboPlatformHelper::scanCallbacks.end()) {
    const char *path = env->GetStringUTFChars(pathStr, nullptr);
    RNFSTurboPlatformHelper::scanCallbacks[jobId](
      jobId,
      path
    );
    env->ReleaseStringUTFChars(pathStr, path);
    RNFSTurboPlatformHelper::scanCallbacks.erase(it);
  }
}

RNFSTurboPlatformHelper::RNFSTurboPlatformHelper(JNIEnv *env) {
  jniEnv = env;
  jclass jniCls = env->FindClass("com/cmpayc/rnfsturbo/RNFSTurboPlatformHelper");
  jmethodID initObject = jniEnv->GetMethodID(jniCls, "<init>", "(Landroid/content/Context;)V");
  jclass contextCls = env->FindClass("com/cmpayc/rnfsturbo/RNFSTurboPlatformContextModule");
  jmethodID mid = env->GetStaticMethodID(contextCls, "getContext", "()Landroid/content/Context;");
  jobject context = env->CallStaticObjectMethod(contextCls, mid);
  jobject obj = jniEnv->NewObject(jniCls, initObject, context);
  jniObj = jniEnv->NewGlobalRef(obj);
  jniEnv->DeleteLocalRef(jniCls);
  jniEnv->DeleteLocalRef(context);
  jniEnv->DeleteLocalRef(obj);
}

std::vector<ReadDirItem> RNFSTurboPlatformHelper::readDirAssets(const char* dirPath) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  std::vector<ReadDirItem> files;
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "readDirAssets",
    "(Ljava/lang/String;)[Ljava/lang/String;"
  );
  jobjectArray filesObject = (jobjectArray)jniEnv->CallObjectMethod(
    jniObj,
    mid,
    jniEnv->NewStringUTF(dirPath)
  );
  if (jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    throw "Dir not exists or access denied";
  }
  int length = (int)jniEnv->GetArrayLength(filesObject);
  for (int i = 0; i < length; i += 4) {
    jstring nameStr = (jstring)jniEnv->GetObjectArrayElement(filesObject, i);
    jstring pathStr = (jstring)jniEnv->GetObjectArrayElement(filesObject, i + 1);
    jstring sizeStr = (jstring)jniEnv->GetObjectArrayElement(filesObject, i + 2);
    jstring isDirectoryStr = (jstring)jniEnv->GetObjectArrayElement(filesObject, i + 3);

    const char* name = jniEnv->GetStringUTFChars(nameStr, nullptr);
    const char* path = jniEnv->GetStringUTFChars(pathStr, nullptr);
    const char* size = jniEnv->GetStringUTFChars(sizeStr, nullptr);
    const char* isDirectoryChr = jniEnv->GetStringUTFChars(isDirectoryStr, nullptr);
    float floatSize = std::stof(size);
    bool isDirectory = std::strcmp(isDirectoryChr, "1") == 0;
    ReadDirItem dirItem{name, path, floatSize, isDirectory};
    files.push_back(dirItem);
    jniEnv->ReleaseStringUTFChars(nameStr, name);
    jniEnv->DeleteLocalRef(nameStr);
    jniEnv->ReleaseStringUTFChars(pathStr, path);
    jniEnv->DeleteLocalRef(pathStr);
    jniEnv->ReleaseStringUTFChars(sizeStr, size);
    jniEnv->DeleteLocalRef(sizeStr);
    jniEnv->ReleaseStringUTFChars(isDirectoryStr, isDirectoryChr);
    jniEnv->DeleteLocalRef(isDirectoryStr);
  }
  jniEnv->DeleteLocalRef(jniCls);
  jniEnv->DeleteLocalRef(filesObject);

  return files;
}

std::string RNFSTurboPlatformHelper::readFileAssetsOrRes(const char* filePath, bool isRes) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  std::vector<ReadDirItem> files;
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "readFileAssetsOrRes",
    "(Ljava/lang/String;Z)[B"
  );
  jbyteArray fileBytesArr = (jbyteArray) jniEnv->CallObjectMethod(
    jniObj,
    mid,
    jniEnv->NewStringUTF(filePath),
    isRes
  );
  if (jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    throw isRes ? "Failed to open asset" : "Failed to open res";
  }
  jbyte* fileJBytes = jniEnv->GetByteArrayElements(fileBytesArr, nullptr);
  std::string fileData = (char*) fileJBytes;
  jniEnv->ReleaseByteArrayElements(fileBytesArr, fileJBytes, 0);

  jniEnv->DeleteLocalRef(jniCls);
  jniEnv->DeleteLocalRef(fileBytesArr);

  return fileData;
}

void RNFSTurboPlatformHelper::copyFileAssetsOrRes(const char *filePath, const char *destPath, bool isRes) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  std::vector<ReadDirItem> files;
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "copyFileAssetsOrRes",
    "(Ljava/lang/String;Ljava/lang/String;Z)V"
  );
  jniEnv->CallVoidMethod(
    jniObj,
    mid,
    jniEnv->NewStringUTF(filePath),
    jniEnv->NewStringUTF(destPath),
    isRes
  );
  jniEnv->DeleteLocalRef(jniCls);
  if (jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    throw isRes ? "Failed to copy res" : "Failed to copy asset";
  }
}

bool RNFSTurboPlatformHelper::existsAssetsOrRes(const char *filePath, bool isRes) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  std::vector<ReadDirItem> files;
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "existsAssetsOrRes",
    "(Ljava/lang/String;Z)Z"
  );
  jboolean isExists = jniEnv->CallBooleanMethod(
    jniObj,
    mid,
    jniEnv->NewStringUTF(filePath),
    isRes
  );
  jniEnv->DeleteLocalRef(jniCls);
  if (jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    throw isRes ? "Failed to open asset" : "Failed to open res";
  }

  return (bool)isExists;
}

void RNFSTurboPlatformHelper::downloadFile(
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
  std::optional<RNFSTurboBeginDownloadCallback> beginCallback,
  std::optional<RNFSTurboProgressDownloadCallback> progressCallback,
  std::optional<RNFSTurboResumableDownloadCallback> resumableCallback
) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "downloadFile",
    "(ILjava/lang/String;Ljava/lang/String;Ljava/util/HashMap;IFIIZZ)V"
  );
  jclass mapClass = jniEnv->FindClass("java/util/HashMap");
  jmethodID initHashMap = jniEnv->GetMethodID(mapClass, "<init>", "()V");
  jobject headersMap = jniEnv->NewObject(mapClass, initHashMap);
  jmethodID putMethod = jniEnv->GetMethodID(
    mapClass,
    "put",
    "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"
  );
  for (auto const& [key, val] : headers) {
    jniEnv->CallObjectMethod(
      headersMap,
      putMethod,
      jniEnv->NewStringUTF(key.c_str()),
      jniEnv->NewStringUTF(val.c_str())
    );
  }

  bool hasBeginCallback = beginCallback.has_value();
  bool hasProgressCallback = progressCallback.has_value();
  RNFSTurboPlatformHelper::downloadCallbacks[jobId].completeCallback = completeCallback;
  RNFSTurboPlatformHelper::downloadCallbacks[jobId].errorCallback = errorCallback;
  if (hasBeginCallback) {
    RNFSTurboPlatformHelper::downloadCallbacks[jobId].beginCallback = beginCallback.value();
  }
  if (hasProgressCallback) {
    RNFSTurboPlatformHelper::downloadCallbacks[jobId].progressCallback = progressCallback.value();
  }

  jniEnv->CallVoidMethod(
    jniObj,
    mid,
    jobId,
    jniEnv->NewStringUTF(fromUrl),
    jniEnv->NewStringUTF(toFile),
    headersMap,
    progressInterval,
    progressDivider,
    connectionTimeout,
    readTimeout,
    hasBeginCallback,
    hasProgressCallback
  );
  jniEnv->DeleteLocalRef(jniCls);
  jniEnv->DeleteLocalRef(mapClass);
  jniEnv->DeleteLocalRef(headersMap);
}

void RNFSTurboPlatformHelper::stopDownload(int jobId) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "stopDownload",
    "(I)V"
  );

  jniEnv->CallVoidMethod(
    jniObj,
    mid,
    jobId
  );
  jniEnv->DeleteLocalRef(jniCls);
}

void RNFSTurboPlatformHelper::uploadFiles(
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
  std::optional<RNFSTurboBeginUploadCallback> beginCallback,
  std::optional<RNFSTurboProgressUploadCallback> progressCallback
) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "uploadFile",
    "(ILjava/lang/String;Z[Ljava/lang/String;ILjava/util/HashMap;Ljava/util/HashMap;Ljava/lang/String;ZZ)V"
  );
  jobjectArray filesArr = jniEnv->NewObjectArray(
    filesNum * 4,
    jniEnv->FindClass("java/lang/String"),
    jniEnv->NewStringUTF("")
  );
  for (int i = 0; i < filesNum * 4; i += 4) {
    jniEnv->SetObjectArrayElement(
      filesArr,
      i,
      jniEnv->NewStringUTF(files[i].name.c_str())
    );
    jniEnv->SetObjectArrayElement(
      filesArr,
      i + 1,
      jniEnv->NewStringUTF(files[i].filename.c_str())
    );
    jniEnv->SetObjectArrayElement(
      filesArr,
      i + 2,
      jniEnv->NewStringUTF(files[i].filepath.c_str())
    );
    jniEnv->SetObjectArrayElement(
      filesArr,
      i + 3,
      jniEnv->NewStringUTF(files[i].filetype.c_str())
    );
  }

  jclass mapClass = jniEnv->FindClass("java/util/HashMap");
  jmethodID initHashMap = jniEnv->GetMethodID(mapClass, "<init>", "()V");
  jobject headersMap = jniEnv->NewObject(mapClass, initHashMap);
  jmethodID putMethod = jniEnv->GetMethodID(
    mapClass,
    "put",
    "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"
  );
  for (auto const& [key, val] : headers) {
    jniEnv->CallObjectMethod(
      headersMap,
      putMethod,
      jniEnv->NewStringUTF(key.c_str()),
      jniEnv->NewStringUTF(val.c_str())
    );
  }
  jobject fieldsMap = jniEnv->NewObject(mapClass, initHashMap);
  for (auto const& [key, val] : fields) {
    jniEnv->CallObjectMethod(
      fieldsMap,
      putMethod,
      jniEnv->NewStringUTF(key.c_str()),
      jniEnv->NewStringUTF(val.c_str())
    );
  }
  bool hasBeginCallback = beginCallback.has_value();
  bool hasProgressCallback = progressCallback.has_value();
  RNFSTurboPlatformHelper::uploadCallbacks[jobId].completeCallback = completeCallback;
  RNFSTurboPlatformHelper::uploadCallbacks[jobId].errorCallback = errorCallback;
  if (hasBeginCallback) {
    RNFSTurboPlatformHelper::uploadCallbacks[jobId].beginCallback = beginCallback.value();
  }
  if (hasProgressCallback) {
    RNFSTurboPlatformHelper::uploadCallbacks[jobId].progressCallback = progressCallback.value();
  }

  jniEnv->CallVoidMethod(
    jniObj,
    mid,
    jobId,
    jniEnv->NewStringUTF(toUrl),
    binaryStreamOnly,
    filesArr,
    filesNum,
    headersMap,
    fieldsMap,
    jniEnv->NewStringUTF(method),
    hasBeginCallback,
    hasProgressCallback
  );

  jniEnv->DeleteLocalRef(filesArr);
  jniEnv->DeleteLocalRef(mapClass);
  jniEnv->DeleteLocalRef(headersMap);
  jniEnv->DeleteLocalRef(fieldsMap);
  jniEnv->DeleteLocalRef(jniCls);
}

void RNFSTurboPlatformHelper::stopUpload(int jobId) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "stopUpload",
    "(I)V"
  );

  jniEnv->CallVoidMethod(
    jniObj,
    mid,
    jobId
  );
  jniEnv->DeleteLocalRef(jniCls);
}

FSInfo RNFSTurboPlatformHelper::getFSInfo() {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  jmethodID mid = jniEnv->GetMethodID(jniCls, "getFSInfo", "()[J");
  jobject infoArr = jniEnv->CallObjectMethod(jniObj, mid);
  jlongArray *infoLongArr = reinterpret_cast<jlongArray*>(&infoArr);
  jlong* infoData = jniEnv->GetLongArrayElements(*infoLongArr, NULL);

  FSInfo fsInfo = {
    static_cast<unsigned long long>(infoData[0]),
    static_cast<unsigned long long>(infoData[1]),
    static_cast<unsigned long long>(infoData[2]),
    static_cast<unsigned long long>(infoData[3])
  };

  jniEnv->ReleaseLongArrayElements(*infoLongArr, infoData, 0);
  jniEnv->DeleteLocalRef(jniCls);
  jniEnv->DeleteLocalRef(infoArr);

  return fsInfo;
}

void RNFSTurboPlatformHelper::scanFile(int jobId, const char *path, RNFSTurboScanCallback scanCallback) {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  std::vector<std::string> items;
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "scanFile",
    "(ILjava/lang/String;)V"
  );
  RNFSTurboPlatformHelper::scanCallbacks[jobId] = scanCallback;
  jniEnv->CallVoidMethod(
    jniObj,
    mid,
    jobId,
    jniEnv->NewStringUTF(path)
  );
  jniEnv->DeleteLocalRef(jniCls);
  if (jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    std::map<int, RNFSTurboScanCallback>::iterator it = RNFSTurboPlatformHelper::scanCallbacks.find(jobId);
    if (it != RNFSTurboPlatformHelper::scanCallbacks.end()) {
      RNFSTurboPlatformHelper::scanCallbacks.erase(it);
    }
    throw "Scan error";
  }
}

std::vector<std::string> RNFSTurboPlatformHelper::getAllExternalFilesDirs() {
  jclass jniCls = jniEnv->GetObjectClass(jniObj);
  std::vector<std::string> items;
  jmethodID mid = jniEnv->GetMethodID(
    jniCls,
    "getAllExternalFilesDirs",
    "()[Ljava/lang/String;"
  );
  jobjectArray filesObject = (jobjectArray) jniEnv->CallObjectMethod(
    jniObj,
    mid
  );
  if (jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    throw "Get all external files dirs error";
  }
  int length = (int) jniEnv->GetArrayLength(filesObject);
  for (int i = 0; i < length; i++) {
    jstring itemStr = (jstring)jniEnv->GetObjectArrayElement(filesObject, i);

    const char* item = jniEnv->GetStringUTFChars(itemStr, nullptr);
    items.push_back(item);
    jniEnv->ReleaseStringUTFChars(itemStr, item);
  }
  jniEnv->DeleteLocalRef(filesObject);
  jniEnv->DeleteLocalRef(jniCls);

  return items;
}
