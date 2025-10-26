//
//  RNFSTurboModule.m
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 23.10.25.
//

#import "RNFSTurboModule.h"
#import "RNFSTurboInstall.h"
#import <Foundation/Foundation.h>

@implementation RNFSTurboModule {
  std::weak_ptr<facebook::react::CallInvoker> _callInvoker;
}

RCT_EXPORT_MODULE()

- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams&)params {
  _callInvoker = params.jsInvoker;
  return std::make_shared<facebook::react::NativeRNFSTurboModuleSpecJSI>(params);
}

- (void)installJSIBindingsWithRuntime:(facebook::jsi::Runtime&)runtime {
  std::shared_ptr<facebook::react::CallInvoker> callInvoker = _callInvoker.lock();
  if (callInvoker == nullptr) {
    throw std::runtime_error("CallInvoker not found");
  }

  cmpayc::rnfsturbo::install(runtime, callInvoker);
}

- (NSString*)getMainBundlePath {
  return [[NSBundle mainBundle] bundlePath];
}

- (NSString*)getCachesDirectoryPath {
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
  return [paths firstObject];
}

- (NSString*)getDocumentDirectoryPath {
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  return [paths firstObject];
}

- (NSString*)getTemporaryDirectoryPath {
  return NSTemporaryDirectory();
}

- (NSString*)getLibraryDirectoryPath {
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
  return [paths firstObject];
}

- (NSString*)getExternalDirectoryPath {
  return @"";
}

- (NSString*)getExternalStorageDirectoryPath {
  return @"";
}

- (NSString*)getExternalCachesDirectoryPath {
  return @"";
}

- (NSString*)getDownloadDirectoryPath {
  return @"";
}

- (NSString*)getPicturesDirectoryPath {
  return @"";
}

- (NSString*)getRoamingDirectoryPath {
  return @"";
}

- (nonnull NSNumber *)createRNFSTurbo { 
  return @(true);
}

@end
