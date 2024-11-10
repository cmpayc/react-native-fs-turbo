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

typedef void (^DownloadCompleteCallback)(NSNumber*, NSNumber*);
typedef void (^ErrorCallback)(NSError*);
typedef void (^BeginCallback)(NSNumber*, NSNumber*, NSDictionary*);
typedef void (^ProgressCallback)(NSNumber*, NSNumber*);
typedef void (^ResumableCallback)(void);
typedef void (^CompletionHandler)(void);

@interface RNFSTurboDownloadParams : NSObject

@property (copy) NSString* fromUrl;
@property (copy) NSString* toFile;
@property (copy) NSDictionary* headers;
@property (copy) DownloadCompleteCallback completeCallback;   // Download has finished (data written)
@property (copy) ErrorCallback errorCallback;                 // Something went wrong
@property (copy) BeginCallback beginCallback;                 // Download has started (headers received)
@property (copy) ProgressCallback progressCallback;           // Download is progressing
@property (copy) ResumableCallback resumableCallback;         // Download has stopped but is resumable
@property        bool background;                             // Whether to continue download when app is in background
@property        bool discretionary;                          // Whether the file may be downloaded at the OS's discretion (iOS only)
@property        bool cacheable;                              // Whether the file may be stored in the shared NSURLCache (iOS only)
@property (copy) NSNumber* progressInterval;
@property (copy) NSNumber* progressDivider;
@property (copy) NSNumber* readTimeout;                       // How long (in milliseconds) a task should wait for additional data to arrive before giving up
@property (copy) NSNumber* backgroundTimeout;                 // How long (in milliseconds) to wait for an entire resource to transfer before giving up


@end

@interface RNFSTurboDownloader : NSObject <NSURLSessionDelegate, NSURLSessionDownloadDelegate>

- (NSString *)downloadFile:(RNFSTurboDownloadParams*)params;
- (void)stopDownload;
- (void)resumeDownload;
- (BOOL)isResumable;
- (void)completeHandler:(NSString *)uuid;
+ (void)setCompletionHandlerForIdentifier: (NSString *)identifier completionHandler: (CompletionHandler)completionHandler;

@end
