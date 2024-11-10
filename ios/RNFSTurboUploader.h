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

#if !TARGET_OS_OSX
#import <MobileCoreServices/MobileCoreServices.h>
#endif

typedef void (^UploadCompleteCallback)(NSString*, NSURLResponse *);
typedef void (^UploadErrorCallback)(NSError*);
typedef void (^UploadBeginCallback)(void);
typedef void (^UploadProgressCallback)(NSNumber*, NSNumber*);

@interface RNFSTurboUploadParams : NSObject

@property (copy) NSString* toUrl;
@property (copy) NSArray* files;
@property (copy) NSDictionary* headers;
@property (copy) NSDictionary* fields;
@property (copy) NSString* method;
@property (assign) BOOL binaryStreamOnly;
@property (copy) UploadCompleteCallback completeCallback;    // Upload has finished (data written)
@property (copy) UploadErrorCallback errorCallback;         // Something gone wrong
@property (copy) UploadBeginCallback beginCallback;         // Upload has started
@property (copy) UploadProgressCallback progressCallback;   // Upload is progressing

@end

@interface RNFSTurboUploader : NSObject <NSURLConnectionDelegate>

- (void)uploadFiles:(RNFSTurboUploadParams*)params;
- (void)stopUpload;

@end