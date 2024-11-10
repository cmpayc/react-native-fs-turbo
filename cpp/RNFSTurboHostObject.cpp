//
//  RNFSTurboHostObject.cpp
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#include "RNFSTurboHostObject.h"
#include "RNFSTurboLogger.h"

using namespace facebook;
namespace fs = std::filesystem;
 
RNFSTurboHostObject::RNFSTurboHostObject(const facebook::react::RNFSTurboConfig& config) {
  RNFSTurboLogger::log("RNFSTurbo", "Initializing RNFSTurbo");
#ifdef __ANDROID__
  JNIEnv *env = facebook::jni::Environment::current();
  platformHelper = new RNFSTurboPlatformHelper(env);
#endif
#ifdef __APPLE__
  platformHelper = new RNFSTurboPlatformHelper();
#endif

}

RNFSTurboHostObject::~RNFSTurboHostObject() {
  delete platformHelper;
  platformHelper = nullptr;
}

std::vector<jsi::PropNameID> RNFSTurboHostObject::getPropertyNames(jsi::Runtime& rt) {
  return jsi::PropNameID::names(rt, "readDir", "readDirAssets", "readdir", "stat", "readFile", "read", "readFileAssets", "readFileRes", "writeFile", "appendFile", "write", "moveFile", "copyFolder", "copyFile", "copyFileAssets", "copyFileRes", "copyAssetsFileIOS", "copyAssetsVideoIOS", "unlink" "exists", "existsAssets", "existsRes", "hash", "touch", "mkdir", "downloadFile", "stopDownload", "resumeDownload", "isResumable", "completeHandlerIOS", "uploadFiles", "stopUpload", "getFSInfo", "scanFile", "getAllExternalFilesDirs", "pathForGroup");
}

jsi::Value RNFSTurboHostObject::get(jsi::Runtime& runtime, const jsi::PropNameID& propNameId) {
  std::string propName = propNameId.utf8(runtime);

  RNFSTurboLogger::log("RNFSTurbo", "call func %s...", propName.c_str());

  if (propName == "stat") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count > 2) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        bool isNewFormat{false};
        if (count > 1 && arguments[1].isBool()) {
          isNewFormat = arguments[1].asBool();
        }

        struct stat t_stat;
        int res = stat(filePath.c_str(), &t_stat);
        if (res < 0) {
            throw jsi::JSError(runtime, "File not exists");
        }

        jsi::Object obj = jsi::Object(runtime);
        obj.setProperty(runtime, "path", jsi::String::createFromUtf8(runtime, filePath));
        obj.setProperty(runtime, "ctime", jsi::Value(static_cast<float>(t_stat.st_ctime)));
        obj.setProperty(runtime, "mtime", jsi::Value(static_cast<float>(t_stat.st_mtime)));
        obj.setProperty(runtime, "size", jsi::Value(static_cast<float>(t_stat.st_size)));
        obj.setProperty(runtime, "mode", jsi::Value(static_cast<int>(t_stat.st_mode)));
        obj.setProperty(runtime, "originalFilepath", jsi::String::createFromUtf8(runtime, filePath));
        if (isNewFormat) {
          obj.setProperty(runtime, "isDirectory", jsi::Value(static_cast<bool>(t_stat.st_mode & S_IFDIR)));
        } else {
          obj.setProperty(runtime, "isDirectory", jsi::Function::createFromHostFunction(
            runtime,
            jsi::PropNameID::forAscii(runtime, "isDirectory"),
            0,
            [t_stat](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) {
              return jsi::Value(static_cast<bool>(t_stat.st_mode & S_IFDIR));
            }
          ));
        }
        if (isNewFormat) {
          obj.setProperty(runtime, "isFile", jsi::Value(static_cast<bool>(t_stat.st_mode & S_IFREG)));
        } else {
          obj.setProperty(runtime, "isFile", jsi::Function::createFromHostFunction(
            runtime,
            jsi::PropNameID::forAscii(runtime, "isFile"),
            0,
            [t_stat](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) {
              return jsi::Value(static_cast<bool>(t_stat.st_mode & S_IFREG));
            }
          ));
        }
        
        return obj;
      }
    );
  }
  if (propName == "readDir" || propName == "readDirAssets" || propName == "readdir") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this, propName](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifndef __ANDROID__
        if (propName == "readDirAssets") {
          throw jsi::JSError(runtime, "readDirAssets command only for Android");
        }
#endif
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count > 2) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }
        bool isAndroidAssets = propName == "readDirAssets";

        std::string dirPath = arguments[0].asString(runtime).utf8(runtime);
        bool isNewFormat{false};
        if (count > 1 && arguments[1].isBool()) {
          isNewFormat = arguments[1].asBool();
        }
        
        struct dirent *dent = nullptr;
        struct stat t_stat;
        int len{0};
        DIR *dir = nullptr;
        std::vector<ReadDirItem> assetsDirItems;
        
        if (isAndroidAssets) {
#ifdef __ANDROID__
          try {
            assetsDirItems = platformHelper->readDirAssets(dirPath.c_str());
            len = assetsDirItems.size();
          } catch (const char *error_message) {
            throw jsi::JSError(runtime, error_message);
          }
#endif
        } else {
          dir = opendir(dirPath.c_str());
          if (!dir){
            throw jsi::JSError(runtime, "Dir not exists or access denied");
          }
          
          while ((dent = readdir(dir)) != NULL){
            if (std::strcmp(dent->d_name, ".") != 0 && std::strcmp(dent->d_name, "..") != 0) {
              len++;
            }
          }
          if (len < 1) {
            closedir(dir);
            jsi::Array arr = jsi::Array(runtime, 0);
            return arr;
          }
          rewinddir(dir);
        }
        std::vector<ReadDirItem>::iterator assetsDirItemsIt = assetsDirItems.begin();
        
        jsi::Array arr = jsi::Array(runtime, len);
        len = 0;
        while (true) {
          if (isAndroidAssets) {
            if (assetsDirItemsIt == end(assetsDirItems)) {
              break;
            }
          } else {
            dent = readdir(dir);
            if (dent == NULL) {
              break;
            }
            if (std::strcmp(dent->d_name, ".") == 0 || std::strcmp(dent->d_name, "..") == 0) {
              continue;
            }
          }
          if (propName == "readdir") {
            arr.setValueAtIndex(
              runtime,
              len,
              jsi::String::createFromUtf8(runtime, dent->d_name)
            );
          } else {
            std::string absolutePath(dirPath);
            if (isAndroidAssets) {
              absolutePath = assetsDirItemsIt->path;
            } else {
              absolutePath.append("/");
              absolutePath.append(dent->d_name);
              stat(absolutePath.c_str(), &t_stat);
            }
            jsi::Object obj = jsi::Object(runtime);
            obj.setProperty(runtime, "path", jsi::String::createFromUtf8(runtime, absolutePath));
            obj.setProperty(
              runtime,
              "ctime",
              jsi::Value(static_cast<float>(isAndroidAssets ? 0 : t_stat.st_ctime))
            );
            obj.setProperty(
              runtime,
              "mtime",
              jsi::Value(static_cast<float>(isAndroidAssets ? 0 : t_stat.st_mtime))
            );
            obj.setProperty(
              runtime,
              "size",
              jsi::Value(static_cast<float>(isAndroidAssets ? assetsDirItemsIt->size : t_stat.st_size))
            );
            if (isNewFormat) {
              obj.setProperty(
                runtime,
                "mode",
                jsi::Value(static_cast<int>(isAndroidAssets ? 0 : t_stat.st_mode))
              );
            }
            obj.setProperty(
              runtime,
              "name",
              jsi::String::createFromUtf8(runtime, isAndroidAssets ? assetsDirItemsIt->name : dent->d_name)
            );
            bool isDirectory = isAndroidAssets ? assetsDirItemsIt->isDirectory : static_cast<bool>(t_stat.st_mode & S_IFDIR);
            if (isNewFormat) {
              obj.setProperty(runtime, "isDirectory", jsi::Value(isDirectory));
            } else {
              obj.setProperty(runtime, "isDirectory", jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "isDirectory"),
                0,
                [&isDirectory](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) {
                  return jsi::Value(isDirectory);
                }
              ));
            }
            bool isFile = isAndroidAssets ? !assetsDirItemsIt->isDirectory : static_cast<bool>(t_stat.st_mode & S_IFREG);
            if (isNewFormat) {
              obj.setProperty(runtime, "isFile", jsi::Value(isFile));
            } else {
              obj.setProperty(runtime, "isFile", jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "isFile"),
                0,
                [&isFile](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) {
                  return jsi::Value(isFile);
                }
              ));
            }
            
            arr.setValueAtIndex(runtime, len, std::move(obj));
          }
          if (isAndroidAssets) {
            assetsDirItemsIt++;
          }
          len++;
        }
        if (propName != "readDirAssets") {
          closedir(dir);
        }

        return arr;
      }
    );
  }
  if (propName == "readFile" || propName == "readFileAssets" || propName == "readFileRes" || propName == "read") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this, propName](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifndef __ANDROID__
        if (propName == "readFileAssets") {
          throw jsi::JSError(runtime, "readFileAssets command only for Android");
        } else if (propName == "readFileRes") {
          throw jsi::JSError(runtime, "readFileRes command only for Android");
        }
#endif
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (propName == "read" && count > 4) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }if (propName != "read" && count > 2) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        long length{0};
        if (propName == "read" && count > 1 && arguments[1].isNumber()) {
          length = arguments[1].asNumber();
        }
        long offset{0};
        if (propName == "read" && count > 2 && arguments[2].isNumber()) {
          offset = arguments[2].asNumber();
        }
        std::string encoding{"utf8"};
        if (propName == "read" && count == 4 && arguments[3].isString()) {
          encoding = arguments[3].asString(runtime).utf8(runtime);
        } else if (propName != "read" && count == 2 && arguments[1].isString()) {
          encoding = arguments[1].asString(runtime).utf8(runtime);
        }
        if (encoding != "utf8" && encoding != "base64" && encoding != "uint8" && encoding != "ascii") {
          throw jsi::JSError(runtime, "Wrong encoding!");
        }

        try {
          if (propName == "readFileAssets" || propName == "readFileRes") {
#ifdef __ANDROID__
            return encoding == "ascii"
              ? jsi::String::createFromAscii(
                  runtime,
                  platformHelper->readFileAssetsOrRes(filePath.c_str(), propName == "readFileRes").c_str()
              ) : jsi::String::createFromUtf8(
                  runtime,
                  platformHelper->readFileAssetsOrRes(filePath.c_str(), propName == "readFileRes").c_str()
              );
#endif
          } else {
            if (encoding == "base64") {
              std::string buffer = readFile(filePath.c_str(), offset, length);
              jsi::String res = jsi::String::createFromUtf8(runtime, base64::to_base64(buffer));
              return res;
            } else if (encoding == "uint8") {
              std::vector<unsigned char> buffer = readFileUint8(filePath.c_str(), offset, length);
              jsi::Array res = jsi::Array(runtime, buffer.size());
              int len = 0;
              for (const int i : buffer) {
                res.setValueAtIndex(
                  runtime,
                  len,
                  jsi::Value(i)
                );
                len++;
              }
              return res;
            } else {
              std::string buffer = readFile(filePath.c_str(), offset, length);
              jsi::String res = encoding == "ascii"
                ? jsi::String::createFromAscii(runtime, buffer)
                : jsi::String::createFromUtf8(runtime, buffer);
              return res;
            }
          }
        } catch (const char *error_message) {
          throw jsi::JSError(runtime, error_message);
        } catch (std::exception const& e) {
          throw jsi::JSError(runtime, e.what());
        }
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "writeFile" || propName == "appendFile" || propName == "write") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this, propName](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        std::string encoding{"utf8"};
        if (propName == "write" && count == 4 && arguments[3].isString()) {
          encoding = arguments[3].asString(runtime).utf8(runtime);
        } else if (propName != "write" && count == 3 && arguments[2].isString()) {
          encoding = arguments[2].asString(runtime).utf8(runtime);
        }
        if (encoding != "utf8" && encoding != "base64" && encoding != "uint8" && encoding != "ascii") {
          throw jsi::JSError(runtime, "Wrong encoding!");
        }
        if (encoding == "uint8" && (count < 2 || !arguments[1].isObject())) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('content') has to be of type number[]!");
        } else if (encoding != "uint8" && (count < 2 || !arguments[1].isString())) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('content') has to be of type string!");
        }
        if (propName == "write" && count > 4) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        } else if (propName != "write" && count > 3) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }
        int offset{-1};
        if (propName == "write" && count > 2 && arguments[2].isNumber()) {
          offset = arguments[2].asNumber();
        }
        if (offset < -1) {
          throw jsi::JSError(runtime, "Negative position");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);

        std::string content{""};
        uint8_t *contentArr = nullptr;
        int contentLength{0};
        if (encoding == "uint8" || encoding == "ascii") {
          jsi::Array jsiObj = arguments[1].asObject(runtime).asArray(runtime);
          if (!jsiObj.isArray(runtime)) {
            throw jsi::JSError(runtime, "Second argument ('content') has to be of type number[]!");
          }
          jsi::Array jsiArr = arguments[1].asObject(runtime).asArray(runtime);
          contentArr = new uint8_t[jsiArr.size(runtime)];
          contentLength = jsiArr.size(runtime) * sizeof(uint8_t);
          for (int i = 0; i < jsiArr.size(runtime); i++) {
            if (!jsiArr.getValueAtIndex(runtime, i).isNumber()) {
              throw jsi::JSError(runtime, "Second argument ('content') has to be of type number[]!");
            }
            int chr = jsiArr.getValueAtIndex(runtime, i).asNumber();
            contentArr[i] = (int) jsiArr.getValueAtIndex(runtime, i).asNumber();
          }
        } else {
          content = arguments[1].asString(runtime).utf8(runtime);
        }
        
        try {
          if (encoding == "base64") {
            if (propName == "write" && offset > -1) {
              writeWithOffset(
                filePath.c_str(),
                base64::from_base64(content),
                offset
              );
            } else {
              writeFile(
                filePath.c_str(),
                base64::from_base64(content).c_str(),
                propName == "appendFile" || (propName == "write" && offset == -1)
              );
            }
          } else if (encoding == "uint8") {
            if (propName == "write" && offset > -1) {
              std::string replaceString(contentLength, 0);
              for (size_t i = 0; i < contentLength; i++) {
                replaceString[i] = static_cast<char>(contentArr[i]);
              }
              writeWithOffset(filePath.c_str(), replaceString, offset);
            } else {
              writeFileUint8(
                filePath.c_str(),
                contentArr,
                contentLength,
                propName == "appendFile" || (propName == "write" && offset == -1)
              );
            }
          } else {
            if (propName == "write" && offset > -1) {
              writeWithOffset(
                filePath.c_str(),
                content,
                offset
              );
            } else {
              writeFile(
                filePath.c_str(),
                content.c_str(),
                propName == "appendFile" || (propName == "write" && offset == -1)
              );
            }
          }
        } catch (const char *error_message) {
          throw jsi::JSError(runtime, error_message);
        } catch (std::exception const& e) {
          throw jsi::JSError(runtime, e.what());
        }

        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "moveFile") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count < 2 || !arguments[1].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('destPath') has to be of type string!");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        std::string destPath = arguments[1].asString(runtime).utf8(runtime);

        try {
          fs::rename(filePath.c_str(), destPath.c_str());
        } catch (fs::filesystem_error& e) {
          throw jsi::JSError(runtime, e.what());
        }

        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "copyFolder") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count < 2 || !arguments[1].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('destPath') has to be of type string!");
        }

        std::string srcFolderPath = arguments[0].asString(runtime).utf8(runtime);
        std::string destFolderPath = arguments[1].asString(runtime).utf8(runtime);

        struct stat t_stat;
        int res = stat(srcFolderPath.c_str(), &t_stat);
        if (res < 0) {
          throw jsi::JSError(runtime, "Dir not exists");
        }
        if (!(t_stat.st_mode & S_IFDIR)) {
          throw jsi::JSError(runtime, "Not a directory");
        }

        try {
          fs::copy(srcFolderPath, destFolderPath, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
        } catch (fs::filesystem_error& e) {
          throw jsi::JSError(runtime, e.what());
        }

        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "copyFile" || propName == "copyFileAssets" || propName == "copyFileRes") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this, propName](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifndef __ANDROID__
        if (propName == "copyFileAssets") {
          throw jsi::JSError(runtime, "copyFileAssets command only for Android");
        } else if (propName == "copyFileRes") {
          throw jsi::JSError(runtime, "copyFileRes command only for Android");
        }
#endif
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count < 2 || !arguments[1].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('destPath') has to be of type string!");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        std::string destPath = arguments[1].asString(runtime).utf8(runtime);

        try {
          if (propName == "copyFileAssets" || propName == "copyFileRes") {
#ifdef __ANDROID__
            platformHelper->copyFileAssetsOrRes(filePath.c_str(), destPath.c_str(), propName == "copyFileRes");
#endif
          } else {
            fs::copy(filePath.c_str(), destPath.c_str());
          }
        } catch (fs::filesystem_error& e) {
          throw jsi::JSError(runtime, e.what());
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }

        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "copyAssetsFileIOS") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __APPLE__
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('imageUri') has to be of type string!");
        }
        if (count < 2 || !arguments[1].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('destPath') has to be of type string!");
        }
        if (count < 3 || !arguments[2].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "Third argument ('width') has to be of type number!");
        }
        if (count < 4 || !arguments[3].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "Third argument ('height') has to be of type number!");
        }
        std::string imageUri = arguments[0].asString(runtime).utf8(runtime);
        std::string destPath = arguments[1].asString(runtime).utf8(runtime);
        int width = arguments[2].asNumber();
        int height = arguments[2].asNumber();
        float scale{1.0};
        if (count > 4 && arguments[4].isNumber()) {
          scale = arguments[4].asNumber();
        }
        float compression{1.0};
        if (count > 5 && arguments[5].isNumber()) {
          compression = arguments[5].asNumber();
        }
        std::string resizeMode{"contain"};
        if (count > 6 && arguments[6].isString()) {
          resizeMode = arguments[6].asString(runtime).utf8(runtime);
        }
        
        if (imageUri.rfind("ph://", 0) == std::string::npos) {
          try {
            fs::copy(imageUri.c_str(), destPath.c_str());
          } catch (fs::filesystem_error& e) {
            throw jsi::JSError(runtime, e.what());
          }
          return jsi::String::createFromUtf8(runtime, destPath);
        }

        try {
          platformHelper->copyAssetsFileIOS(imageUri.c_str(), destPath.c_str(), width, height, scale, compression, resizeMode.c_str());
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }
        
        return jsi::String::createFromUtf8(runtime, destPath);
#else
        throw jsi::JSError(runtime, "copyAssetsFileIOS command only for iOS");
#endif
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "copyAssetsVideoIOS") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __APPLE__
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('videoUri') has to be of type string!");
        }
        if (count < 2 || !arguments[1].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('destPath') has to be of type string!");
        }
        std::string videoUri = arguments[0].asString(runtime).utf8(runtime);
        std::string destPath = arguments[1].asString(runtime).utf8(runtime);
        if (videoUri.rfind("ph://", 0) == std::string::npos) {
          try {
            fs::copy(videoUri.c_str(), destPath.c_str());
          } catch (fs::filesystem_error& e) {
            throw jsi::JSError(runtime, e.what());
          }
          return jsi::String::createFromUtf8(runtime, destPath);
        }

        try {
          platformHelper->copyAssetsVideoIOS(videoUri.c_str(), destPath.c_str());
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }

        return jsi::String::createFromUtf8(runtime, destPath);
#else
        throw jsi::JSError(runtime, "copyAssetsVideoIOS command only for iOS");
#endif
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "unlink") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count != 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        
        std::string filePath = arguments[0].asString(runtime).utf8(runtime);

        try {
          fs::remove_all(filePath.c_str());
        } catch (fs::filesystem_error& e) {
          throw jsi::JSError(runtime, e.what());
        }
        
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "exists" || propName == "existsAssets" || propName == "existsRes") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this, propName](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifndef __ANDROID__
        if (propName == "existsAssets") {
          throw jsi::JSError(runtime, "existsAssets command only for Android");
        } else if (propName == "existsRes") {
          throw jsi::JSError(runtime, "existsRes command only for Android");
        }
#endif
        if (count != 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        
        std::string filePath = arguments[0].asString(runtime).utf8(runtime);

        bool exists{false};
        if (propName == "existsAssets" || propName == "existsRes") {
#ifdef __ANDROID__
          exists = platformHelper->existsAssetsOrRes(filePath.c_str(), propName == "existsRes");
#endif
        } else {
          struct stat t_stat;
          exists = stat(filePath.c_str(), &t_stat) >= 0;
        }
        
        return jsi::Value(exists);
      }
    );
  }
  if (propName == "hash") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 2 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count < 2 || !arguments[1].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "Second argument ('algorithm') has to be of type string!");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        std::string algorithm = arguments[1].asString(runtime).utf8(runtime);

        try {
          std::string buffer = readFile(filePath.c_str(), 0, -1);
          jsi::String res = jsi::String::createFromUtf8(runtime, "");

          if (algorithm == "md5") {
            res = jsi::String::createFromUtf8(runtime, md5(buffer));
          } else if (algorithm == "sha1") {
            res = jsi::String::createFromUtf8(runtime, sha1(buffer));
          } else if (algorithm == "sha224") {
            res = jsi::String::createFromUtf8(runtime, sha224(buffer));
          } else if (algorithm == "sha256") {
            res = jsi::String::createFromUtf8(runtime, sha256(buffer));
          } else if (algorithm == "sha384") {
            res = jsi::String::createFromUtf8(runtime, sha384(buffer));
          } else if (algorithm == "sha512") {
            res = jsi::String::createFromUtf8(runtime, sha512(buffer));
          } else {
            throw "Wrong algorithm (only mds, sha1, sha224, sha256, sha384, sha512 allowed)";
          }

          return res;
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "touch") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }
        if (count > 3) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        long int mtime{0};
        bool isMTime{false};
        if (count > 1 && arguments[1].isNumber()) {
          mtime = arguments[1].asNumber() / 1000;
          isMTime = true;
        }
        long int ctime{0};
        bool isCTime{false};
        if (count > 2 && arguments[2].isNumber()) {
          ctime = arguments[2].asNumber() / 1000;
          isCTime = true;
        }

        struct stat t_stat;
        int res = stat(filePath.c_str(), &t_stat);
        if (res < 0) {
            throw jsi::JSError(runtime, "File not exists");
        }

        if (isMTime) {
          struct utimbuf new_times;
          new_times.modtime = mtime > 0 ? mtime : std::time(0);
          new_times.actime = mtime > 0 ? mtime : std::time(0);
          utime(filePath.c_str(), &new_times);
        }
        if (isCTime) {
          // Not available
        }
        stat(filePath.c_str(), &t_stat);

        return jsi::Value(static_cast<int>(t_stat.st_mtime));
      }
    );
  }
  if (propName == "mkdir") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if ((count != 1 && count != 2) || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('filepath') has to be of type string!");
        }

        std::string filePath = arguments[0].asString(runtime).utf8(runtime);
        bool isExcludedFromBackupKey{false};
        if (count == 2 && arguments[1].isObject()) {
            jsi::Value propIsExcludedFromBackupKey = arguments[1].asObject(runtime).getProperty(runtime, "NSURLIsExcludedFromBackupKey");
            if (propIsExcludedFromBackupKey.isBool()) {
                isExcludedFromBackupKey = propIsExcludedFromBackupKey.getBool();
            }
        }

        try {
          fs::create_directories(filePath);

          if (isExcludedFromBackupKey) {
#ifdef __APPLE__
            NSString *nsFilePath = [NSString stringWithCString:filePath.c_str()
                                                          encoding:[NSString defaultCStringEncoding]];
            NSURL *url = [NSURL fileURLWithPath:nsFilePath];
            BOOL success = [url setResourceValue:[NSNumber numberWithBool:YES] forKey:NSURLIsExcludedFromBackupKey error:nil];

            if (!success) {
              throw jsi::JSError(runtime, "Can not set NSURLIsExcludedFromBackupKey");
            }
#endif
          }
        } catch (fs::filesystem_error& e) {
          throw jsi::JSError(runtime, e.what());
        }

        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "downloadFile") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 3 || !arguments[0].isObject()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('options') has to be of type object!");
        }
        if (count > 3) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }
        jsi::Object options = arguments[0].asObject(runtime);
        if (!options.hasProperty(runtime, "fromUrl")) {
          throw jsi::JSError(runtime, "fromUrl option is mandatory");
        }
        if (!options.hasProperty(runtime, "toFile")) {
          throw jsi::JSError(runtime, "toFile option is mandatory");
        }
        std::shared_ptr<jsi::Function> completeFunc = std::make_shared<jsi::Function>(arguments[1].asObject(runtime).asFunction(runtime));
        std::shared_ptr<jsi::Function> errorFunc = std::make_shared<jsi::Function>(arguments[2].asObject(runtime).asFunction(runtime));
        std::string fromUrl{""};
        std::string toFile{""};
        std::map<std::string, std::string> headers;
        bool background{false};
        bool discretionary{false};
        bool cacheable{true};
        int progressInterval{0};
        float progressDivider{0};
        std::shared_ptr<jsi::Function> beginCallbackFunc;
        std::shared_ptr<jsi::Function> progressCallbackFunc;
        std::shared_ptr<jsi::Function> resumableCallbackFunc;
        int connectionTimeout{0};
        int readTimeout{0};
        int backgroundTimeout{0};
        jsi::Value optionFromUrl = options.getProperty(runtime, "fromUrl");
        if (optionFromUrl.isString()) {
          fromUrl = optionFromUrl.asString(runtime).utf8(runtime);
          if (fromUrl.size() == 0) {
            throw jsi::JSError(runtime, "fromUrl option is mandatory");
          }
        } else {
          throw jsi::JSError(runtime, "fromUrl option is string");
        }
        jsi::Value optionToFile = options.getProperty(runtime, "toFile");
        if (optionToFile.isString()) {
          toFile = optionToFile.asString(runtime).utf8(runtime);
          if (toFile.size() == 0) {
            throw jsi::JSError(runtime, "toFile option is mandatory");
          }
        } else {
          throw jsi::JSError(runtime, "toFile option is string");
        }
        if (options.hasProperty(runtime, "headers")) {
          jsi::Value optionHeaders = options.getProperty(runtime, "headers");
          if (optionHeaders.isObject()) {
            jsi::Object tempHeaders = optionHeaders.asObject(runtime);
            jsi::Array headerNames = tempHeaders.getPropertyNames(runtime);
            for (int i = 0; i < headerNames.size(runtime); i++){
              std::string headerKey = headerNames.getValueAtIndex(runtime, i).asString(runtime).utf8(runtime);
              std::string headerValue = tempHeaders.getProperty(runtime, headerKey.c_str()).asString(runtime).utf8(runtime);
              headers[headerKey] = headerValue;
            }
          }
        }
        if (options.hasProperty(runtime, "begin")) {
          jsi::Value optionBegin = options.getProperty(runtime, "begin");
          if (optionBegin.isObject()) {
            beginCallbackFunc = std::make_shared<jsi::Function>(optionBegin.asObject(runtime).asFunction(runtime));
          }
        }
        if (options.hasProperty(runtime, "progress")) {
          jsi::Value optionProgress = options.getProperty(runtime, "progress");
          if (optionProgress.isObject()) {
            progressCallbackFunc = std::make_shared<jsi::Function>(optionProgress.asObject(runtime).asFunction(runtime));
          }
        }
        if (options.hasProperty(runtime, "resumable")) {
          jsi::Value optionResumable = options.getProperty(runtime, "resumable");
          if (optionResumable.isObject()) {
            resumableCallbackFunc = std::make_shared<jsi::Function>(optionResumable.asObject(runtime).asFunction(runtime));
          }
        }
        if (options.hasProperty(runtime, "background")) {
          jsi::Value optionBackground = options.getProperty(runtime, "background");
          if (optionBackground.isBool()) {
            background = optionBackground.asBool();
          }
        }
        if (options.hasProperty(runtime, "discretionary")) {
          jsi::Value optionDiscretionary = options.getProperty(runtime, "discretionary");
          if (optionDiscretionary.isBool()) {
            discretionary = optionDiscretionary.asBool();
          }
        }
        if (options.hasProperty(runtime, "cacheable")) {
          jsi::Value optionCacheable = options.getProperty(runtime, "cacheable");
          if (optionCacheable.isBool()) {
            cacheable = optionCacheable.asBool();
          }
        }
        if (options.hasProperty(runtime, "progressInterval")) {
          jsi::Value optionProgressInterval = options.getProperty(runtime, "progressInterval");
          if (optionProgressInterval.isNumber()) {
            progressInterval = optionProgressInterval.asNumber();
          }
        }
        if (options.hasProperty(runtime, "progressDivider")) {
          jsi::Value optionProgressDivider = options.getProperty(runtime, "progressDivider");
          if (optionProgressDivider.isNumber()) {
            progressDivider = optionProgressDivider.asNumber();
          }
        }
        if (options.hasProperty(runtime, "connectionTimeout")) {
          jsi::Value optionConnectionTimeout = options.getProperty(runtime, "connectionTimeout");
          if (optionConnectionTimeout.isNumber()) {
            connectionTimeout = optionConnectionTimeout.asNumber();
          }
        }
        if (options.hasProperty(runtime, "readTimeout")) {
          jsi::Value optionReadTimeout = options.getProperty(runtime, "readTimeout");
          if (optionReadTimeout.isNumber()) {
            readTimeout = optionReadTimeout.asNumber();
          }
        }
        if (options.hasProperty(runtime, "backgroundTimeout")) {
          jsi::Value optionBackgroundTimeout = options.getProperty(runtime, "backgroundTimeout");
          if (optionBackgroundTimeout.isNumber()) {
            backgroundTimeout = optionBackgroundTimeout.asNumber();
          }
        }
        
        RNFSTurboPlatformHelper::jobId += 1;
        int jobId = RNFSTurboPlatformHelper::jobId;
        
        RNFSTurboCompleteDownloadCallback completeCallback = [&runtime, completeFunc](int jobId, int statusCode, float bytesWritten) -> void {
          jsi::Object result = jsi::Object(runtime);
          result.setProperty(runtime, "jobId", jsi::Value(jobId));
          result.setProperty(runtime, "statusCode", jsi::Value(statusCode));
          result.setProperty(runtime, "bytesWritten", jsi::Value(bytesWritten));
          completeFunc->call(runtime, std::move(result));
        };
        
        RNFSTurboErrorCallback errorCallback = [&runtime, errorFunc](int jobId, const char* errorMessage) -> void {
          errorFunc->call(runtime, jsi::String::createFromUtf8(runtime, errorMessage));
        };
        
        std::optional<RNFSTurboBeginDownloadCallback> beginCallback = std::nullopt;
        if (beginCallbackFunc) {
          beginCallback = [&runtime, beginCallbackFunc](int jobId, int statusCode, float contentLength, std::map<std::string, std::string> headers) -> void {
            jsi::Object result = jsi::Object(runtime);
            result.setProperty(runtime, "jobId", jsi::Value(jobId));
            result.setProperty(runtime, "statusCode", jsi::Value(statusCode));
            result.setProperty(runtime, "contentLength", jsi::Value(contentLength));
            jsi::Object jsHeaders = jsi::Object(runtime);
            for (auto const& [key, val] : headers) {
              jsHeaders.setProperty(runtime, key.c_str(), jsi::String::createFromUtf8(runtime, val));
            }
            result.setProperty(runtime, "headers", jsHeaders);
            beginCallbackFunc->call(runtime, std::move(result));
          };
        }
        std::optional<RNFSTurboProgressDownloadCallback> progressCallback = std::nullopt;
        if (progressCallbackFunc) {
          progressCallback = [&runtime, progressCallbackFunc](int jobId, float contentLength, float bytesWritten) -> void {
            jsi::Object result = jsi::Object(runtime);
            result.setProperty(runtime, "jobId", jsi::Value(jobId));
            result.setProperty(runtime, "contentLength", jsi::Value(contentLength));
            result.setProperty(runtime, "bytesWritten", jsi::Value(bytesWritten));
            progressCallbackFunc->call(runtime, std::move(result));
          };
        }
        std::optional<RNFSTurboResumableDownloadCallback> resumableCallback = std::nullopt;
        if (resumableCallbackFunc) {
          resumableCallback = [&runtime, resumableCallbackFunc](int jobId) -> void {
            resumableCallbackFunc->call(runtime, jsi::Value(jobId));
          };
        }
        
        try {
          platformHelper->downloadFile(
            jobId,
            fromUrl.c_str(),
            toFile.c_str(),
            headers,
            background,
            discretionary,
            cacheable,
            progressInterval,
            progressDivider,
            readTimeout,
            backgroundTimeout,
            connectionTimeout,
            completeCallback,
            errorCallback,
            beginCallback,
            progressCallback,
            resumableCallback
          );
        } catch (const char* errorMessage) {
          errorFunc->call(runtime, jsi::String::createFromUtf8(runtime, errorMessage));
        }

        return jsi::Value(jobId);
      }
    );
  }
  if (propName == "stopDownload") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count != 1 || !arguments[0].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('jobId') has to be of type number!");
        }
        
        int jobId = arguments[0].asNumber();

        platformHelper->stopDownload(jobId);
        
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "resumeDownload") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __APPLE__
        if (count != 1 || !arguments[0].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('jobId') has to be of type number!");
        }
        
        int jobId = arguments[0].asNumber();

        platformHelper->resumeDownload(jobId);
#else
        throw jsi::JSError(runtime, "resumeDownload command only for iOS");
#endif
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "isResumable") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __APPLE__
        if (count != 1 || !arguments[0].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('jobId') has to be of type number!");
        }
        
        int jobId = arguments[0].asNumber();

        bool isResumable = platformHelper->isResumable(jobId);
        return jsi::Value(isResumable);
#else
        throw jsi::JSError(runtime, "isResumable command only for iOS");
#endif
      }
    );
  }
  if (propName == "completeHandlerIOS") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __APPLE__
        if (count != 1 || !arguments[0].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('jobId') has to be of type number!");
        }
        
        int jobId = arguments[0].asNumber();

        platformHelper->completeHandlerIOS(jobId);
#else
        throw jsi::JSError(runtime, "completeHandlerIOS command only for iOS");
#endif
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "uploadFiles") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count < 3 || !arguments[0].isObject()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('options') has to be of type object!");
        }
        if (count > 3) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }
        jsi::Object options = arguments[0].asObject(runtime);
        if (!options.hasProperty(runtime, "toUrl")) {
          throw jsi::JSError(runtime, "toUrl option is mandatory");
        }
        if (!options.hasProperty(runtime, "files")) {
          throw jsi::JSError(runtime, "files option is mandatory");
        }
        std::shared_ptr<jsi::Function> completeFunc = std::make_shared<jsi::Function>(arguments[1].asObject(runtime).asFunction(runtime));
        std::shared_ptr<jsi::Function> errorFunc = std::make_shared<jsi::Function>(arguments[2].asObject(runtime).asFunction(runtime));
        std::string toUrl{""};
        bool binaryStreamOnly{false};
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> fields;
        std::string method{"POST"};
        std::shared_ptr<jsi::Function> beginCallbackFunc;
        std::shared_ptr<jsi::Function> progressCallbackFunc;
        jsi::Value optionToUrl = options.getProperty(runtime, "toUrl");
        if (optionToUrl.isString()) {
          toUrl = optionToUrl.asString(runtime).utf8(runtime);
          if (toUrl.size() == 0) {
            throw jsi::JSError(runtime, "toUrl option is mandatory");
          }
        } else {
          throw jsi::JSError(runtime, "toUrl option is string");
        }
        if (options.hasProperty(runtime, "binaryStreamOnly")) {
          jsi::Value optionBinaryStreamOnly = options.getProperty(runtime, "binaryStreamOnly");
          if (optionBinaryStreamOnly.isBool()) {
            binaryStreamOnly = optionBinaryStreamOnly.asBool();
          }
        }
        jsi::Value optionFiles = options.getProperty(runtime, "files");
        if (!optionFiles.isObject()) {
          throw jsi::JSError(runtime, "files option is object");
        }
        jsi::Array tmpFiles = optionFiles.asObject(runtime).asArray(runtime);
        int filesNum = tmpFiles.size(runtime);
        if (filesNum == 0) {
          throw jsi::JSError(runtime, "files option is mandatory");
        }
        UploadFileItem files[filesNum];
        for (int i = 0; i < filesNum; i++){
          jsi::Object obj = tmpFiles.getValueAtIndex(runtime, i).asObject(runtime);
          if (
              !obj.hasProperty(runtime, "name") ||
              !obj.hasProperty(runtime, "filename") ||
              !obj.hasProperty(runtime, "filepath") ||
              !obj.hasProperty(runtime, "filetype")
          ) {
            throw jsi::JSError(runtime, "wrong file data");
          }
          jsi::Value tmpName = obj.getProperty(runtime, "name");
          jsi::Value tmpFilename = obj.getProperty(runtime, "filename");
          jsi::Value tmpFilepath = obj.getProperty(runtime, "filepath");
          jsi::Value tmpFiletype = obj.getProperty(runtime, "filetype");
          if (
              !tmpName.isString() ||
              !tmpFilename.isString() ||
              !tmpFilepath.isString() ||
              !tmpFiletype.isString()
          ) {
            throw jsi::JSError(runtime, "wrong file data");
          }
          files[i] = {
            tmpName.asString(runtime).utf8(runtime),
            tmpFilename.asString(runtime).utf8(runtime),
            tmpFilepath.asString(runtime).utf8(runtime),
            tmpFiletype.asString(runtime).utf8(runtime),
          };
        }
        if (options.hasProperty(runtime, "headers")) {
          jsi::Value optionHeaders = options.getProperty(runtime, "headers");
          if (optionHeaders.isObject()) {
            jsi::Object tempHeaders = optionHeaders.asObject(runtime);
            jsi::Array headerNames = tempHeaders.getPropertyNames(runtime);
            for (int i = 0; i < headerNames.size(runtime); i++){
              std::string headerKey = headerNames.getValueAtIndex(runtime, i).asString(runtime).utf8(runtime);
              std::string headerValue = tempHeaders.getProperty(runtime, headerKey.c_str()).asString(runtime).utf8(runtime);
              headers[headerKey] = headerValue;
            }
          }
        }
        if (options.hasProperty(runtime, "fields")) {
          jsi::Value optionFields = options.getProperty(runtime, "fields");
          if (optionFields.isObject()) {
            jsi::Object tempFields = optionFields.asObject(runtime);
            jsi::Array fieldNames = tempFields.getPropertyNames(runtime);
            for (int i = 0; i < fieldNames.size(runtime); i++){
              std::string fieldKey = fieldNames.getValueAtIndex(runtime, i).asString(runtime).utf8(runtime);
              std::string fieldValue = tempFields.getProperty(runtime, fieldKey.c_str()).asString(runtime).utf8(runtime);
              fields[fieldKey] = fieldValue;
            }
          }
        }
        if (options.hasProperty(runtime, "method")) {
          jsi::Value optionMethod = options.getProperty(runtime, "method");
          if (optionMethod.isString()) {
            method = optionMethod.asString(runtime).utf8(runtime);
          }
        }
        if (options.hasProperty(runtime, "begin")) {
          jsi::Value optionBegin = options.getProperty(runtime, "begin");
          if (optionBegin.isObject()) {
            beginCallbackFunc = std::make_shared<jsi::Function>(optionBegin.asObject(runtime).asFunction(runtime));
          }
        }
        if (options.hasProperty(runtime, "progress")) {
          jsi::Value optionProgress = options.getProperty(runtime, "progress");
          if (optionProgress.isObject()) {
            progressCallbackFunc = std::make_shared<jsi::Function>(optionProgress.asObject(runtime).asFunction(runtime));
          }
        }
        
        RNFSTurboPlatformHelper::jobId += 1;
        int jobId = RNFSTurboPlatformHelper::jobId;
        
        RNFSTurboCompleteUploadCallback completeCallback = [&runtime, completeFunc](int jobId, int statusCode, std::map<std::string, std::string> headers, const char* body) -> void {
          jsi::Object result = jsi::Object(runtime);
          result.setProperty(runtime, "jobId", jsi::Value(jobId));
          result.setProperty(runtime, "statusCode", jsi::Value(statusCode));
          jsi::Object jsHeaders = jsi::Object(runtime);
          for (auto const& [key, val] : headers) {
            jsHeaders.setProperty(runtime, key.c_str(), jsi::String::createFromUtf8(runtime, val));
          }
          result.setProperty(runtime, "headers", jsHeaders);
          result.setProperty(runtime, "body", jsi::String::createFromUtf8(runtime, body));
          completeFunc->call(runtime, std::move(result));
        };
        
        RNFSTurboErrorCallback errorCallback = [&runtime, errorFunc](int jobId, const char* errorMessage) -> void {
          errorFunc->call(runtime, jsi::String::createFromUtf8(runtime, errorMessage));
        };
        
        std::optional<RNFSTurboBeginUploadCallback> beginCallback = std::nullopt;
        if (beginCallbackFunc) {
          beginCallback = [&runtime, beginCallbackFunc](int jobId) -> void {
            jsi::Object result = jsi::Object(runtime);
            result.setProperty(runtime, "jobId", jsi::Value(jobId));
            beginCallbackFunc->call(runtime, std::move(result));
          };
        }
        std::optional<RNFSTurboProgressUploadCallback> progressCallback = std::nullopt;
        if (progressCallbackFunc) {
          progressCallback = [&runtime, progressCallbackFunc](int jobId, float totalBytesExpectedToSend, float totalBytesSent) -> void {
            jsi::Object result = jsi::Object(runtime);
            result.setProperty(runtime, "jobId", jsi::Value(jobId));
            result.setProperty(runtime, "totalBytesExpectedToSend", jsi::Value(totalBytesExpectedToSend));
            result.setProperty(runtime, "totalBytesSent", jsi::Value(totalBytesSent));
            progressCallbackFunc->call(runtime, std::move(result));
          };
        }
        
        try {
          platformHelper->uploadFiles(
            jobId,
            toUrl.c_str(),
            binaryStreamOnly,
            files,
            filesNum,
            headers,
            fields,
            method.c_str(),
            completeCallback,
            errorCallback,
            beginCallback,
            progressCallback
          );
        } catch (const char* errorMessage) {
          errorFunc->call(runtime, jsi::String::createFromUtf8(runtime, errorMessage));
        }

        return jsi::Value(jobId);
      }
    );
  }
  if (propName == "stopUpload") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count != 1 || !arguments[0].isNumber()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('jobId') has to be of type number!");
        }
        
        int jobId = arguments[0].asNumber();

        platformHelper->stopUpload(jobId);
        
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "getFSInfo") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
        if (count > 0) [[unlikely]] {
          throw jsi::JSError(runtime, "The function does not take arguments!");
        }
        
        try {
          FSInfo fsInfo = platformHelper->getFSInfo();
          
          jsi::Object result = jsi::Object(runtime);
          result.setProperty(runtime, "totalSpace", jsi::Value(static_cast<float>(fsInfo.totalSpace)));
          result.setProperty(runtime, "freeSpace", jsi::Value(static_cast<float>(fsInfo.freeSpace)));
          if (fsInfo.totalSpaceEx.has_value()) {
            result.setProperty(runtime, "totalSpaceEx", jsi::Value(static_cast<float>(fsInfo.totalSpaceEx.value())));
          }
          if (fsInfo.freeSpaceEx.has_value()) {
            result.setProperty(runtime, "freeSpaceEx", jsi::Value(static_cast<float>(fsInfo.freeSpaceEx.value())));
          }
          
          return result;
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }
        
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "scanFile") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __ANDROID__
        if (count < 2 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('path') has to be of type string!");
        }
        if (count > 2) [[unlikely]] {
          throw jsi::JSError(runtime, "Too many arguments");
        }

        std::string path = arguments[0].asString(runtime).utf8(runtime);

        std::shared_ptr<jsi::Function> scanFunc = std::make_shared<jsi::Function>(
          arguments[1].asObject(runtime).asFunction(runtime)
        );

        RNFSTurboPlatformHelper::jobId += 1;
        int jobId = RNFSTurboPlatformHelper::jobId;

        RNFSTurboScanCallback scanCallback = [&runtime, scanFunc](int jobId, std::string path) -> void {
          jsi::Object result = jsi::Object(runtime);
          result.setProperty(runtime, "jobId", jsi::Value(jobId));
          result.setProperty(runtime, "path", jsi::String::createFromUtf8(runtime, path));
          scanFunc->call(runtime, std::move(result));
        };

        try {
          platformHelper->scanFile(jobId, path.c_str(), scanCallback);
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }
#else
        throw jsi::JSError(runtime, "scanFile command only for Android");
#endif
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "getAllExternalFilesDirs") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __ANDROID__
        if (count > 0) [[unlikely]] {
          throw jsi::JSError(runtime, "The function does not take arguments!");
        }

        try {
          std::vector<std::string> items = platformHelper->getAllExternalFilesDirs();
          int len = items.size();

          std::vector<std::string>::iterator itemsIt = items.begin();

          jsi::Array arr = jsi::Array(runtime, len);

          len = 0;
          while (itemsIt != end(items)) {
            arr.setValueAtIndex(
              runtime,
              len,
              jsi::String::createFromUtf8(runtime, *itemsIt)
            );
            itemsIt++;
            len++;
          }

          return arr;
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }
#else
        throw jsi::JSError(runtime, "getAllExternalFilesDirs command only for Android");
#endif
        return jsi::Value::undefined();
      }
    );
  }
  if (propName == "pathForGroup") {
    return jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, propName),
      1,
      [this](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
#ifdef __APPLE__
        if (count != 1 || !arguments[0].isString()) [[unlikely]] {
          throw jsi::JSError(runtime, "First argument ('groupIdentifier') has to be of type string!");
        }
        
        std::string groupIdentifier = arguments[0].asString(runtime).utf8(runtime);

        try {
          std::string path = platformHelper->pathForGroup(groupIdentifier.c_str());

          return jsi::String::createFromUtf8(runtime, path);
        } catch (const char* error_message) {
          throw jsi::JSError(runtime, error_message);
        }
#else
        throw jsi::JSError(runtime, "pathForGroup command only for iOS");
#endif
        return jsi::Value::undefined();
      }
    );
  }

  return jsi::Value::undefined();
}
