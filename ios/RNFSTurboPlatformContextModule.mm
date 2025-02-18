//
//  RNFSTurboPlatformContextModule.m
//  react-native-fs-turbo
//
//  Created by Sergei Kazakov on 03.08.24.
//

#import "RNFSTurboPlatformContextModule.h"
#import <Foundation/Foundation.h>

@implementation RNFSTurboPlatformContextModule

RCT_EXPORT_MODULE()

- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams&)params {
  return std::make_shared<facebook::react::NativeRNFSTurboPlatformContextModuleSpecJSI>(params);
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

@end
