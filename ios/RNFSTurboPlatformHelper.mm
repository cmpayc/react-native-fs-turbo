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

#import <Foundation/Foundation.h>
#import <Photos/Photos.h>
#import "RNFSTurboDownloader.h"
#import "RNFSTurboUploader.h"
#import "RNFSTurboPlatformHelper.h"

namespace cmpayc::rnfsturbo {

RNFSTurboPlatformHelper::RNFSTurboPlatformHelper() {}

void RNFSTurboPlatformHelper::copyAssetsFileIOS(const char* imageUri, const char* destPath, int width, int height, float scale, float compression, const char* resizeMode) {
  NSString *nsImageUri = [NSString stringWithCString:imageUri
                                            encoding:[NSString defaultCStringEncoding]];
  NSString *nsDestPath = [NSString stringWithCString:destPath
                                            encoding:[NSString defaultCStringEncoding]];
  NSURL* url = [NSURL URLWithString:nsImageUri];
  __block NSError *nsError = nil;
  
  PHFetchResult *phAssetFetchResult = nil;
  if (![url.scheme isEqualToString:@"ph"]) {
    throw "Not a photo asset.";
  } else {
    phAssetFetchResult = [PHAsset fetchAssetsWithLocalIdentifiers:@[[nsImageUri substringFromIndex: 5]] options:nil];
  }
  if (phAssetFetchResult.count == 0) {
    throw "Failed to fetch PHAsset with local identifier with no error message.";
  }

  PHAsset *phAsset = [phAssetFetchResult firstObject];
  PHImageRequestOptions *options = [PHImageRequestOptions new];
  options.networkAccessAllowed = YES;
  options.deliveryMode = PHImageRequestOptionsDeliveryModeHighQualityFormat;
  
  CGSize size = CGSizeMake(width, height);
  
  BOOL useMaximumSize = CGSizeEqualToSize(size, CGSizeZero);
  CGSize targetSize;
  if (useMaximumSize) {
    targetSize = PHImageManagerMaximumSize;
    options.resizeMode = PHImageRequestOptionsResizeModeNone;
  } else {
    targetSize = CGSizeApplyAffineTransform(size, CGAffineTransformMakeScale(scale, scale));
    options.resizeMode = PHImageRequestOptionsResizeModeExact;
  }

  PHImageContentMode contentMode = PHImageContentModeAspectFill;
  if (strcmp(resizeMode, "contain") == 0) {
    contentMode = PHImageContentModeAspectFit;
  }

  [[PHImageManager defaultManager] requestImageForAsset:phAsset
                                             targetSize:targetSize
                                            contentMode:contentMode
                                                options:options
                                          resultHandler:^(UIImage *result, NSDictionary<NSString *, id> *info) {
    if (result) {
      NSData *imageData = UIImageJPEGRepresentation(result, compression );
      [imageData writeToFile:nsDestPath atomically:YES];
    } else {
      NSMutableDictionary* details = [NSMutableDictionary dictionary];
      [details setValue:info[PHImageErrorKey] forKey:NSLocalizedDescriptionKey];
      NSError *nsError = [NSError errorWithDomain:@"RNFS" code:501 userInfo:details];
      std::string error = std::string([[nsError localizedDescription] UTF8String]);

      throw error;
    }
  }];

  if (nsError) {
    std::string error = std::string([[nsError localizedDescription] UTF8String]);
    throw error;
  }
}

void RNFSTurboPlatformHelper::copyAssetsVideoIOS(const char* videoUri, const char* destPath) {
  NSString *nsVideoUri = [NSString stringWithCString:videoUri
                                                     encoding:[NSString defaultCStringEncoding]];
  NSString *nsDestPath = [NSString stringWithCString:destPath
                                                     encoding:[NSString defaultCStringEncoding]];

  NSURL* url = [NSURL URLWithString:nsVideoUri];
  __block NSError *nsError = nil;

  PHFetchResult *phAssetFetchResult = nil;
  
  if (![url.scheme isEqualToString:@"ph"]) {
    throw "Not a photo asset";
  } else {
    phAssetFetchResult = [PHAsset fetchAssetsWithLocalIdentifiers:@[[nsVideoUri substringFromIndex: 5]] options:nil];
  }
  if (phAssetFetchResult.count == 0) {
    throw "Failed to fetch PHAsset with local identifier with no error message.";
  }

  PHAsset *phAsset = [phAssetFetchResult firstObject];
  PHVideoRequestOptions *options = [[PHVideoRequestOptions alloc] init];
  options.networkAccessAllowed = YES;
  options.version = PHVideoRequestOptionsVersionOriginal;
  options.deliveryMode = PHVideoRequestOptionsDeliveryModeAutomatic;
  
  dispatch_group_t group = dispatch_group_create();
  dispatch_group_enter(group);

  [[PHImageManager defaultManager] requestAVAssetForVideo:phAsset options:options resultHandler:^(AVAsset *asset, AVAudioMix *audioMix, NSDictionary *info) {
    if ([asset isKindOfClass:[AVURLAsset class]]) {
      NSURL *url = [(AVURLAsset *)asset URL];
      BOOL writeResult = false;
        
      if (@available(iOS 9.0, *)) {
          NSURL *destinationUrl = [NSURL fileURLWithPath:nsDestPath relativeToURL:nil];
          writeResult = [[NSFileManager defaultManager] copyItemAtURL:url toURL:destinationUrl error:&nsError];
      } else {
          NSData *videoData = [NSData dataWithContentsOfURL:url];
          writeResult = [videoData writeToFile:nsDestPath options:NSDataWritingAtomic error:&nsError];
      }

      dispatch_group_leave(group);
    }
  }];
  dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
  
  if (nsError) {
    throw [nsError localizedDescription];
  }
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
  NSString *nsFromUrl = [NSString stringWithCString:fromUrl
                                            encoding:[NSString defaultCStringEncoding]];
  NSString *nsToFile = [NSString stringWithCString:toFile
                                        encoding:[NSString defaultCStringEncoding]];
  NSMutableDictionary *nsHeaders = [[NSMutableDictionary alloc] init];
  for (const auto& [headerKey, headerValue] : headers) {
    [nsHeaders setValue:@(headerValue.c_str()) forKey:@(headerKey.c_str())];
  }
  RNFSTurboDownloadParams* params = [RNFSTurboDownloadParams alloc];
  params.fromUrl = nsFromUrl;
  params.toFile = nsToFile;
  params.headers = nsHeaders;
  params.background = background;
  params.discretionary = discretionary;
  params.cacheable = cacheable;
  params.progressInterval = @(progressInterval);
  params.progressDivider = @(progressDivider);
  params.readTimeout = @(readTimeout);
  params.backgroundTimeout = @(backgroundTimeout);

  __block BOOL callbackFired = NO;

  params.completeCallback = ^(NSNumber* statusCode, NSNumber* bytesWritten) {
    if (callbackFired) {
      return;
    }
    callbackFired = YES;
    completeCallback(jobId, [statusCode intValue], [bytesWritten floatValue]);
  };

  params.errorCallback = ^(NSError* error) {
    if (callbackFired) {
      return;
    }
    callbackFired = YES;
    errorCallback(jobId, [[error localizedDescription] UTF8String]);
  };

  if (beginCallback.has_value()) {
    params.beginCallback = ^(NSNumber* statusCode, NSNumber* contentLength, NSDictionary* nsHeaders) {
      std::map<std::string, std::string> headers;
      for (id key in nsHeaders) {
        NSString *headerValue = [nsHeaders objectForKey:key];
        headers[[key UTF8String]] = [headerValue UTF8String];
      }
      beginCallback.value()(jobId, [statusCode intValue], [contentLength floatValue], headers);
    };
  }

  if (progressCallback.has_value()) {
    params.progressCallback = ^(NSNumber* contentLength, NSNumber* bytesWritten) {
      progressCallback.value()(jobId, [contentLength floatValue], [bytesWritten floatValue]);
    };
  }

  if (resumableCallback.has_value()) {
    params.resumableCallback = ^() {
      resumableCallback.value()(jobId);
    };
  }

  if (!RNFSTurboPlatformHelper::downloaders) {
    RNFSTurboPlatformHelper::downloaders = [[NSMutableDictionary alloc] init];
  }

  RNFSTurboDownloader* downloader = [RNFSTurboDownloader alloc];

  NSString *uuid = [downloader downloadFile:params];

  [RNFSTurboPlatformHelper::downloaders setValue:downloader forKey:[@(jobId) stringValue]];
  if (uuid) {
    if (!RNFSTurboPlatformHelper::uuids) {
      RNFSTurboPlatformHelper::uuids = [[NSMutableDictionary alloc] init];
    }
    [RNFSTurboPlatformHelper::uuids setValue:uuid forKey:[@(jobId) stringValue]];
  }
}

void RNFSTurboPlatformHelper::stopDownload(int jobId) {
  RNFSTurboDownloader* downloader = [RNFSTurboPlatformHelper::downloaders objectForKey:[@(jobId) stringValue]];

  if (downloader != nil) {
    [downloader stopDownload];
  }
}

void RNFSTurboPlatformHelper::resumeDownload(int jobId) {
  RNFSTurboDownloader* downloader = [RNFSTurboPlatformHelper::downloaders objectForKey:[@(jobId) stringValue]];

  if (downloader != nil) {
    [downloader resumeDownload];
  }
}

bool RNFSTurboPlatformHelper::isResumable(int jobId) {
  RNFSTurboDownloader* downloader = [RNFSTurboPlatformHelper::downloaders objectForKey:[@(jobId) stringValue]];

  if (downloader != nil) {
    return [downloader isResumable];
  } else {
    return false;
  }
}

void RNFSTurboPlatformHelper::completeHandlerIOS(int jobId) {
  RNFSTurboDownloader* downloader = [RNFSTurboPlatformHelper::downloaders objectForKey:[@(jobId) stringValue]];
  NSString *uuid = [RNFSTurboPlatformHelper::uuids objectForKey:[@(jobId) stringValue]];

  if (downloader != nil && uuid != nil) {
    [downloader completeHandler:uuid];
  }
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
  NSString *nsToUrl = [NSString stringWithCString:toUrl
                                            encoding:[NSString defaultCStringEncoding]];
  NSMutableArray *nsFiles = [[NSMutableArray alloc] init];
  for (int i = 0; i < filesNum; i++) {
    NSMutableDictionary *nsFile = [[NSMutableDictionary alloc] init];
    [nsFile setValue:@(files[i].name.c_str()) forKey:@"name"];
    [nsFile setValue:@(files[i].filename.c_str()) forKey:@"filename"];
    [nsFile setValue:@(files[i].filepath.c_str()) forKey:@"filepath"];
    [nsFile setValue:@(files[i].filetype.c_str()) forKey:@"filetype"];
    [nsFiles addObject:nsFile];
  }
  NSMutableDictionary *nsHeaders = [[NSMutableDictionary alloc] init];
  for (const auto& [headerKey, headerValue] : headers) {
    [nsHeaders setValue:@(headerValue.c_str()) forKey:@(headerKey.c_str())];
  }
  NSMutableDictionary *nsFields = [[NSMutableDictionary alloc] init];
  for (const auto& [fieldKey, fieldValue] : fields) {
    [nsFields setValue:@(fieldValue.c_str()) forKey:@(fieldKey.c_str())];
  }
  NSString *nsMethod = [NSString stringWithCString:method
                                            encoding:[NSString defaultCStringEncoding]];
  RNFSTurboUploadParams* params = [RNFSTurboUploadParams alloc];
  params.toUrl = nsToUrl;
  params.files = nsFiles;
  params.binaryStreamOnly = binaryStreamOnly;
  params.headers = nsHeaders;
  params.fields = nsFields;
  params.method = nsMethod;

  params.completeCallback = ^(NSString* body, NSURLResponse *resp) {
    [RNFSTurboPlatformHelper::uploaders removeObjectForKey:[@(jobId) stringValue]];

    std::map<std::string, std::string> headers;
    if ([resp isKindOfClass:[NSHTTPURLResponse class]]) {
      NSDictionary* nsHeaders = ((NSHTTPURLResponse *)resp).allHeaderFields;
      for (id key in nsHeaders) {
        NSString *headerValue = [nsHeaders objectForKey:key];
        headers[[key UTF8String]] = [headerValue UTF8String];
      }
      int statusCode = ((NSHTTPURLResponse *)resp).statusCode;
      
      completeCallback(
        jobId,
        statusCode,
        headers,
        [body UTF8String]
      );
    } else {
      completeCallback(jobId, 0, headers, "");
    }
  };

  params.errorCallback = ^(NSError* error) {
    [RNFSTurboPlatformHelper::uploaders removeObjectForKey:[@(jobId) stringValue]];
    errorCallback(jobId, [[error localizedDescription] UTF8String]);
  };

  if (beginCallback.has_value()) {
    params.beginCallback = ^() {
      beginCallback.value()(jobId);
    };
  }

  if (progressCallback.has_value()) {
    params.progressCallback = ^(NSNumber* totalBytesExpectedToSend, NSNumber* totalBytesSent) {
      progressCallback.value()(jobId, [totalBytesExpectedToSend floatValue], [totalBytesSent floatValue]);
    };
  }

  if (!RNFSTurboPlatformHelper::uploaders) {
    RNFSTurboPlatformHelper::uploaders = [[NSMutableDictionary alloc] init];
  }

  RNFSTurboUploader* uploader = [RNFSTurboUploader alloc];

  [uploader uploadFiles:params];
}

void RNFSTurboPlatformHelper::stopUpload(int jobId) {
  RNFSTurboUploader* uploader = [RNFSTurboPlatformHelper::uploaders objectForKey:[@(jobId) stringValue]];

  if (uploader != nil) {
    [uploader stopUpload];
  }
}

FSInfo RNFSTurboPlatformHelper::getFSInfo() {
  unsigned long long totalSpace = 0;
  unsigned long long totalFreeSpace = 0;

  __autoreleasing NSError *error = nil;
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error:&error];

  if (dictionary) {
    NSNumber *fileSystemSizeInBytes = [dictionary objectForKey: NSFileSystemSize];
    NSNumber *freeFileSystemSizeInBytes = [dictionary objectForKey:NSFileSystemFreeSize];
    totalSpace = [fileSystemSizeInBytes unsignedLongLongValue];
    totalFreeSpace = [freeFileSystemSizeInBytes unsignedLongLongValue];

    FSInfo fsInfo = {totalSpace, totalFreeSpace};
    
    return fsInfo;
  } else {
    throw [[error localizedDescription] UTF8String];
  }
}

const char* RNFSTurboPlatformHelper::pathForGroup(const char* groupIdentifier) {
  NSString *nsGroupIdentifier = [NSString stringWithCString:groupIdentifier
                                                  encoding:[NSString defaultCStringEncoding]];
  NSURL *groupURL = [[NSFileManager defaultManager]containerURLForSecurityApplicationGroupIdentifier: nsGroupIdentifier];
  
  if (groupURL) {
    return [[groupURL path] UTF8String];
  } else {
    throw [[NSString stringWithFormat:@"No directory for group '%@' found", nsGroupIdentifier] UTF8String];
  }
}

void RNFSTurboPlatformHelper::setResourceValue(
  const char *path,
  const char *optionType,
  const char *optionValue
) {
  NSString* nsFilePath = [NSString stringWithUTF8String:path];
  NSString* type = [NSString stringWithUTF8String:optionType];
  NSString* value = [NSString stringWithUTF8String:optionValue];
  NSURL *nsFileUrl = [NSURL fileURLWithPath:nsFilePath];
  if ([type isEqualToString:NSFileProtectionKey]) {
    NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];
    [attributes setValue:value forKey:@"NSFileProtectionKey"];
    [[NSFileManager defaultManager] setAttributes:attributes ofItemAtPath:nsFilePath error:nil];
  } else if ([type isEqualToString:NSURLIsExcludedFromBackupKey]) {
    BOOL isExclude = [value isEqualToString:@"YES"];
    [nsFileUrl setResourceValue:@(isExclude) forKey:NSURLIsExcludedFromBackupKey error:nil];
  }
}

}
