## react-native-fs-turbo

* This library repeats all the methods of the original [react-native-fs](https://github.com/itinance/react-native-fs) but all the code is executed synchronously in C++ files via JSI (and JNI) and written using TypeScript.
* New methods for working with files have also been added, including AES256 encryption (disabled by default)

> [!IMPORTANT]
> `react-native-fs-turbo` is a pure C++ TurboModule, and **requires the new architecture to be enabled**. (react-native 0.75+)
> - If you want to use `react-native-fs-turbo`, you need to enable the new architecture in your app (see ["Enable the New Architecture for Apps"](https://github.com/reactwg/react-native-new-architecture/blob/main/docs/enable-apps.md))
> - If you cannot use the new architecture yet, use the standard [react-native-fs](https://github.com/itinance/react-native-fs) library.
> - React-Native version 0.74 is no longer supported. Use the `0.3.7` library version. (`"react-native-fs-turbo": "0.3.7")

## Benchmark

<details open>
<summary>Benchmark tests of <b>react-native-fs-turbo</b> and <b>react-native-fs</b> libraries (React-Native 0.82.0, iPhone 12 Pro (iOS 18.6.2), Samsung Galaxy Note 20 Ultra (Android 13), release mode, 10,000 iterations)</summary>
<br>
Conducted 10 tests, showed average time (only basic functions tested)
<br>
<b>serial</b> - sequential async tests
<br>
<b>parallel</b> - parallel async tests (20 promises per batch)
<br>
<b>readDir</b> test checked reading of 10,000 files in a folder
<br>

#### Basic functions

| Library                             | writeFile     | appendFile     |  writeFile (100Mb, 1 iter) |
| ----------------------------------- | ------------- | -------------- | -------------------------- |
| react-native-fs (ios, serial)       | 9.793s        | 3.424s         | 58.608s                    |
| react-native-fs (ios, parallel)     | 9.251s        | 2.911s         | --                         |
| react-native-fs-turbo (ios)         | 6.254s (^1.5) | 1.836s (^1.6)  | 0.192s (^305)              |
| react-native-fs (android, serial)   | 8.309s        | 5.571s         | Crash (10Mb - 6.989s)      |
| react-native-fs (android, parallel) | 1.655s        | 1.092s         | --                         |
| react-native-fs-turbo (android)     | 0.701s (^2.3) | 0.266s (^4.1)  | 3.510s (^20)               |

| Library                             | readFile (utf8) | readFile (base64) | readFile (100Mb, 1 iter) |
| ----------------------------------- | --------------- | ----------------- | ------------------------ |
| react-native-fs (ios, serial)       | 5.347s          | 4.385s            | 120.863s                 |
| react-native-fs (ios, parallel)     | 4.590s          | 4.202s            | --                       |
| react-native-fs-turbo (ios)         | 1.578s (^2.9)   | 1.597s (^2.6)     | 0.073s (^1655)           |
| react-native-fs (android, serial)   | 9.067s          | 3.932s            | Crash (10Mb - 13.758s)   |
| react-native-fs (android, parallel) | 2.438s          | 0.984s            | --                       |
| react-native-fs-turbo (android)     | 0.283s (^8.6)   | 0.284s (^3.4)     | 0.204 (^674)             |

| Library                             |  stat          | unlink        |
| ----------------------------------- | -------------- | ------------- |
| react-native-fs (ios, serial)       | 2.542s         | 2.607s        |
| react-native-fs (ios, parallel)     | 2.332s         | 2.421s        |
| react-native-fs-turbo (ios)         | 0.538s (^4.3)  | 1.475s (^1.6) |
| react-native-fs (android, serial)   | 4.132s         | 5.691s        |
| react-native-fs (android, parallel) | 1.087s         | 1.456s        |
| react-native-fs-turbo (android)     | 0.159s (^6.8)  | 0.401s (^3.6) |

| Library                             | hash (md5)    | hash (sha512) | readDir       |
| ----------------------------------- | ------------- | ------------- | ------------- |
| react-native-fs (ios, serial)       | 4.452s        | 9.040s        | 2.181s        |
| react-native-fs (ios, parallel)     | 4.216s        | 8.602s        | 2.154s        |
| react-native-fs-turbo (ios)         | 1.616s (^2.6) | 3.271s (^2.6) | 0.447s (^4.8) |
| react-native-fs (android, serial)   | 4.124s        | 9.609s        | 0.303s        |
| react-native-fs (android, parallel) | 1.696s        | 4.560s        | 0.407s        |
| react-native-fs-turbo (android)     | 0.301s (^5.6) | 0.640s (^7.1) | 0.083s (^4.9) |

#### AES256 Encryption (extra)

| Library                             | readFile (cbc) | writeFile (cbc) |
| ----------------------------------- | -------------- | --------------- |
| react-native-fs-turbo (ios)         | 1.602s         | 6.515s          |
| react-native-fs-turbo (android)     | 0.406s         | 2.215s          |

</details>


## Installation

### React Native

```sh
yarn add react-native-fs-turbo
cd ios && pod install
```

### Android

#### Proguard
If you're using Proguard, make sure to add the following rule at proguard-rules.pro:

```
-keep class com.cmpayc.rnfsturbo.** { *; }
```

### AES256 Encryption (disabled by default)
To enable AES256 encryption methods, you need to add encryption flags to Android and iOS builds.

#### iOS
Add a new strings to `ios/Podfile` file to `post_install` section

```ruby
  post_install do |installer|
    ...
    installer.pods_project.targets.each do |target|
      if target.name == 'RNFSTurbo'
        target.build_configurations.each do |config|
          config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] ||= ['$(inherited)', 'RNFSTURBO_USE_ENCRYPTION=1']
        end
      end
    end
  end
```

#### Android
Add a new string to `android/gradle.properties` file

```
rnFsTurboUseEncryption=true
```

## Usage

### Import

Simply import the library anywhere. A single instance is used

```ts
import RNFSTurbo from 'react-native-fs-turbo';
```

### Examples

#### Basic

```ts
import RNFSTurbo from 'react-native-fs-turbo';

try {
  const results = RNFSTurbo.readDir(RNFSTurbo.DocumentDirectoryPath, true); // use true as the second parameter to get a pure object without functions

  const files: string = [];
  const dirs: string = [];
  results.forEach((dirStat) => {
    if (dirStat.isFile) {
      files.push(dirStat.path);
    } else {
      dirs.push(dirStat.path);
    }
  });
} catch (err: Error) {
  console.log(err.message);
}
```

#### File creation

```ts
import RNFSTurbo from 'react-native-fs-turbo';

const pathUtf8 = `${RNFSTurbo.DocumentDirectoryPath}/test_utf8.txt`;
const pathBase64 = `${RNFSTurbo.DocumentDirectoryPath}/test_base64.txt`;
const pathUint8 = `${RNFSTurbo.DocumentDirectoryPath}/test_uint8.txt`;
const pathFloat32 = `${RNFSTurbo.DocumentDirectoryPath}/test_float32.txt`;

try {
  RNFSTurbo.writeFile(path, 'Lorem ipsum dolor sit amet', 'utf8');
  RNFSTurbo.writeFile(path, 'TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQ=', 'base64');
  // An array with uint8 can be passed as input data
  RNFSTurbo.writeFile(path, [76, 111, 114, 101, 109, 32, 105, 112, 115, 117, 109, 32, 100, 111, 108, 111, 114, 32, 115, 105, 116, 32, 97, 109, 101, 116], 'uint8');
  // An array with float32 can be passed as input data
  RNFSTurbo.writeFile(path, [0.4233244061470032, 0.5435456037521362, 2.5345540046691895, 7.2343244552612305, 3.867867946624756, 0.7876875996589661], 'float32');
} catch (err: Error) {
  console.log(err.message);
}
```

#### File deletion

```ts
import RNFSTurbo from 'react-native-fs-turbo';

const path = `${RNFSTurbo.DocumentDirectoryPath}/test.txt`;

try {
  RNFSTurbo.unlink(path);
} catch (err: Error) {
  console.log(err.message);
}
```

#### File upload

```ts
import RNFSTurbo, { UploadFileItem } from 'react-native-fs-turbo';

const uploadUrl = 'http://requestb.in/XXXXXXX';  // For testing purposes, go to http://requestb.in/ and create your own link
// create an array of objects of the files you want to upload

const files: UploadFileItem[] = [
  {
    name: 'test1',
    filename: 'test1.w4a',
    filepath: `${RNFSTurbo.DocumentDirectoryPath}/test1.w4a`,
    filetype: 'audio/x-m4a',
  },
  {
    name: 'test2',
    filename: 'test2.w4a',
    filepath: `${RNFSTurbo.DocumentDirectoryPath}/test2.w4a`,
    filetype: 'audio/x-m4a',
  },
];

const beginCallback = (res: UploadBeginCallbackResult) => {
  const jobId = res.jobId;
  console.log('Upload has begun. Job id', jobId);
};

const progressCallback = (res: UploadProgressCallbackResult) => {
  const percentage = Math.floor(
    (res.totalBytesSent / res.totalBytesExpectedToSend) * 100,
  );
  console.log('Upload progress', percentage);
};

const completeCallback = (res: UploadResult) => {
  if (res.statusCode === 200) {
    console.log('Files uploaded'); // response.statusCode, response.headers, response.body
  } else {
    console.log('Server error');
  }
};

const errorCallback = (err: UploadError) => {
  if (err.errorMessage === 'cancelled') {
    // cancelled by user
  }
  console.log('Error', err);
};

// Pass a completion callback and an error callback as the second and third parameters to get rid of promises
const uploadJob = RNFSTurbo.uploadFiles(
  {
    toUrl: uploadUrl,
    files: files,
    method: 'POST',
    headers: {},
    fields: {
      submit: 'yes',
    },
    begin: beginCallback,
    progress: progressCallback,
  },
  completeCallback,
  errorCallback,
);
console.log('Upload job id', uploadJob.jobId);
```

#### Encryption (extra)

```ts
const aesKey = '29f4734849a0ee82fd9fd56e9cc4d163';

RNFSTurbo.writeFile(
  `${RNFSTurbo.DocumentDirectoryPath}/encrypted.txt`,
  'Hello world!',
  {
    encrypted: true,
    passphrase: aesKey,
    mode: 'ecb',
  },
);

const decrypted = RNFSTurbo.readFile(
  `${RNFSTurbo.DocumentDirectoryPath}/encrypted.txt`,
  {
    encrypted: true,
    passphrase: aesKey,
    mode: 'ecb',
  },
);

console.log('Decrypted data', decrypted);
```

## API

### Constants

The following constants are available on the `RNFSTurbo` export:
- `MainBundlePath` (`string`) The absolute path to the main bundle directory (iOS only)
- `CachesDirectoryPath` (`string`) The absolute path to the caches directory
- `ExternalCachesDirectoryPath` (`string`) The absolute path to the external caches directory (Android only)
- `DocumentDirectoryPath`  (`string`) The absolute path to the document directory
- `DownloadDirectoryPath` (`string`) The absolute path to the download directory (Android only)
- `TemporaryDirectoryPath` (`string`) The absolute path to the temporary directory (falls back to Caching-Directory on Android)
- `LibraryDirectoryPath` (`string`) The absolute path to the NSLibraryDirectory (iOS only)
- `ExternalDirectoryPath` (`string`) The absolute path to the external files, shared directory (Android only)
- `ExternalStorageDirectoryPath` (`string`) The absolute path to the external storage, shared directory (Android only)
- `PicturesDirectoryPath` (`string`) The absolute path to the pictures directory (Android only)
- `RoamingDirectoryPath` (`string`) The absolute path to the roaming directory (Not available)


IMPORTANT: when using `ExternalStorageDirectoryPath` it's necessary to request permissions (on Android) to read and write on the external storage, here an example: [React Native Offical Doc](https://reactnative.dev/docs/permissionsandroid)

### `stat(filepath: string, isNewFormat?: boolean): StatResult`

Stats an item at `filepath`. If the `filepath` is linked to a virtual file, for example Android Content URI, the `originalPath` can be used to find the pointed file path.
The promise resolves with an object with the following properties:

```ts
type StatResult = {
  path: string; // The same as filepath argument
  ctime: Date | number; // The creation date of the file (Date for old format, number (unixtime, sec) for new format)
  mtime: Date | number; // The last modified date of the file (Date for old format, number (unixtime, sec) for new format)
  size: number; // Size in bytes
  mode: number; // UNIX file mode
  originalFilepath: string; // ANDROID: In case of content uri this is the pointed file path, otherwise is the same as path
  isFile: boolean | () => boolean; // Is the file just a file? Will be boolean if passed isNewFormat=true
  isDirectory: boolean | () => boolean; // Is the file a directory? Will be boolean if passed isNewFormat=true
};
```

Note: additional conversion of unixtime (number) to Date takes time, so number (unixtime, sec) is returned in the new format. For backward compatibility, the Date is returned in the old format.

### `readDir(dirpath: string, isNewFormat?: boolean): ReadDirItem[]`

Reads the contents of `path`. This must be an absolute path. Use the above path constants to form a usable file path.

The returned promise resolves with an array of objects with the following properties:

```ts
type ReadDirItem = {
  ctime: Date | number; // The creation date of the file (iOS only) (Date for old format, number (unixtime, sec) for new format)
  mtime: Date | number; // The last modified date of the file (Date for old format, number (unixtime, sec) for new format)
  name: string; // The name of the item
  path: string; // The absolute path to the item
  size: string; // Size in bytes
  isFile: boolean | () => boolean; // Is the file just a file? Will be boolean if passed isNewFormat=true
  isDirectory: boolean | () => boolean; // Is the file a directory? Will be boolean if passed isNewFormat=true
};
```

Note: additional conversion of unixtime (number) to Date takes time, so number (unixtime, sec) is returned in the new format. For backward compatibility, the Date is returned in the old format.

### (Android only) `readDirAssets(dirpath: string, isNewFormat?: boolean): ReadDirItem[]`

Reads the contents of `dirpath ` in the Android app's assets folder.
`dirpath ` is the relative path to the file from the root of the `assets` folder.

The returned promise resolves with an array of objects with the following properties:

```ts
type ReadDirItem = {
  name: string;     // The name of the item
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes.
  						// Note that the size of files compressed during the creation of the APK (such as JSON files) cannot be determined.
  						// `size` will be set to -1 in this case.
  isFile: boolean | () => boolean;        // Is the file just a file? Will be boolean if passed isNewFormat=true
  isDirectory: boolean | () => boolean;   // Is the file a directory? Will be boolean if passed isNewFormat=true
};
```

### `readdir(dirpath: string) => string[]`

Node.js style version of `readDir` that returns only the names. Note the lowercase `d`.

### `readFile(filepath: string, options?: ReadOptions): string | number[]`

Reads the file at `path` and return contents. `options` can be string of encrypted types or object, default is `utf8`. Use `base64` or `uint8` or `float32` encoding for reading binary files.

```ts
type ReadOptions =
  | 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32'
  | {
      encoding: 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32'.
      // Next flags will work only if encryption is enabled
      encrypted?: boolean;
      passphrase?: string | number[];
      iv?: string | number[];
      mode?: "ecb" | "cbc" | "cfb";
      padding?: "ansi_x9.23" | "iso/iec_7816-4" | "pkcs5/pkcs7" | "zero" | "no";
    };
```

Note: you will take quite a performance hit if you are reading big files

### `read(filepath: string, length: number, position: number, options?: ReadOptions): string | number[]`

Reads `length` bytes from the given `position` of the file at `path` and returns contents. `options` can be string of encrypted types or object, default is `utf8`. Use `base64` or `uint8` or `float32` encoding for reading binary files.

```ts
type ReadOptions =
  | 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32'
  | { encoding: 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32' };
```

Note: reading big files piece by piece using this method may be useful in terms of performance.
Note: `float32` size is 4 bytes, so `position` and `length` should be specified in bytes (multiplied by 4)
Note: encryption doesn't work for partial file reading

### (Android only) `readFileAssets(filepath: string, options?: ReadOptions) => string[]`

Reads the file at `path` in the Android app's assets folder and return contents. `options` can be string of encrypted types or object, default is `utf8`. Use `base64` encoding for reading binary files.

`filepath` is the relative path to the file from the root of the `assets` folder.

```ts
type ReadOptions =
  | 'utf8' | 'ascii' | 'base64'
  | { encoding: 'utf8' | 'ascii' | 'base64' };
```

### (Android only) `readFileRes: (filepath: string, options?: ReadOptions) => string[]`

Reads the file named `filename` in the Android app's `res` folder and return contents. Only the file name (not folder) needs to be specified. The file type will be detected from the extension and automatically located within `res/drawable` (for image files) or `res/raw` (for everything else). `options` can be string of encrypted types or object, default is `utf8`. Use `base64` encoding for reading binary files.

```ts
type ReadOptions =
  | 'utf8' | 'ascii' | 'base64'
  | { encoding: 'utf8' | 'ascii' };
```

### `writeFile(filepath: string, contents: string | number[], options?: WriteOptions): void`

Write the `contents` to `filepath`. `options` can be string of encrypted types or object, default is `utf8`

```ts
type WriteOptions =
  | 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32'
  | {
      encoding?: 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32',
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive" // iOS 17+ only,
      // Next flags will work only if encryption is enabled
      encrypted?: boolean;
      passphrase?: string | number[];
      iv?: string | number[];
      mode?: "ecb" | "cbc" | "cfb";
      padding?: "ansi_x9.23" | "iso/iec_7816-4" | "pkcs5/pkcs7" | "zero" | "no";
    };
```

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.

### `appendFile(filepath: string, contents: string | number[], options?: WriteOptions): void`

Append the `contents` to `filepath`. `encoding` can be string of encrypted types or object, default is `utf8`.

```ts
type WriteOptions =
  | 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32'
  | {
      encoding?: 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32',
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive" // iOS 17+ only
    };
```

Note: encryption doesn't work for to partially write a file

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.

### `write(filepath: string, contents: string | number[], position?: number, options?: WriteOptions): void`

Write the `contents` to `filepath` at the given random access position. When `position` is `undefined` or `-1` the contents is appended to the end of the file. `encoding` can be string of encrypted types or object, default is `utf8`.

```ts
type WriteOptions =
  | 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32'
  | {
      encoding?: 'utf8' | 'ascii' | 'base64' | 'uint8' | 'float32',
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive" // iOS 17+ only
    };
```

Note: `float32` size is 4 bytes, so `position` should be specified in bytes (multiplied by 4)
Note: encryption doesn't work for to partially write a file

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.

### `moveFile(filepath: string, destPath: string, options?: MoveCopyOptions): void`

Moves the file located at `filepath` to `destPath`. This is more performant than reading and then re-writing the file data because the move is done natively and the data doesn't have to be copied or cross the bridge.

```ts
type MoveCopyOptions =
  | {
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive" // iOS 17+ only
    };
```

Note: Overwrites existing file

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.

### `copyFolder(srcFolderPath: string, destFolderPath: string, options?: MoveCopyOptions): void`

Copies the contents located at `srcFolderPath` to `destFolderPath`.

```ts
type MoveCopyOptions =
  | {
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive" // iOS 17+ only
    };
```

Note: Recursively replaces all files and folders

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.

### `copyFile(filepath: string, destPath: string, options?: MoveCopyOptions): void`

Copies the file located at `filepath` to `destPath`.

```ts
type MoveCopyOptions =
  | {
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive" // iOS 17+ only
    };
```

Note: Error will be thrown if the file already exists.

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.

### (Android only) `copyFileAssets(filepath: string, destPath: string): void`

Copies the file at `filepath` in the Android app's assets folder and copies it to the given `destPath ` path.

Note: Will overwrite destPath if it already exists.

### (Android only) `copyFileRes(filepath: string, destPath: string): void`

Copies the file named `filename` in the Android app's res folder and copies it to the given `destPath ` path. `res/drawable` is used as the source parent folder for image files, `res/raw` for everything else.

Note: Will overwrite destPath if it already exists.

### (iOS only) `copyAssetsFileIOS(imageUri: string, destPath: string, width: number, height: number, scale?: number, compression?: number, resizeMode?: string): string | undefined`

*Not available on Mac Catalyst.*

Reads an image file from Camera Roll and writes to `destPath`. This method [assumes the image file to be JPEG file](https://github.com/itinance/react-native-fs/blob/f2f8f4a058cd9acfbcac3b8cf1e08fa1e9b09786/RNFSManager.m#L752-L753). This method will download the original from iCloud if necessary.

#### Parameters

##### `imageUri` string (required)

URI of a file in Camera Roll. Can be [either of the following formats](https://github.com/itinance/react-native-fs/blob/f2f8f4a058cd9acfbcac3b8cf1e08fa1e9b09786/RNFSManager.m#L781-L785):

- `ph://CC95F08C-88C3-4012-9D6D-64A413D254B3/L0/001`
- `assets-library://asset/asset.JPG?id=CC95F08C-88C3-4012-9D6D-64A413D254B3&ext=JPG`

##### `destPath` string (required)

Destination to which the copied file will be saved, e.g. `RNFS.TemporaryDirectoryPath + 'example.jpg'`.

##### `width` number (required)

Copied file's image width will be resized to `width`. [If 0 is provided, width won't be resized.](https://github.com/itinance/react-native-fs/blob/f2f8f4a058cd9acfbcac3b8cf1e08fa1e9b09786/RNFSManager.m#L808)

##### `height` number (required)

Copied file's image height will be resized to `height`. [If 0 is provided, height won't be resized.](https://github.com/itinance/react-native-fs/blob/f2f8f4a058cd9acfbcac3b8cf1e08fa1e9b09786/RNFSManager.m#L808)

##### `scale` number (optional)

Copied file's image will be scaled proportional to `scale` factor from `width` x `height`. If both `width` and `height` are 0, the image won't scale. Range is [0.0, 1.0] and default is 1.0.

##### `compression` number (optional)

Quality of copied file's image. The value 0.0 represents the maximum compression (or lowest quality) while the value 1.0 represents the least compression (or best quality). Range is [0.0, 1.0] and default is 1.0.

##### `resizeMode` string (optional)

If `resizeMode` is 'contain', copied file's image will be scaled so that its larger dimension fits `width` x `height`. If `resizeMode` is other value than 'contain', the image will be scaled so that it completely fills `width` x `height`. Default is 'contain'. Refer to [PHImageContentMode](https://developer.apple.com/documentation/photokit/phimagecontentmode).

#### Return value

##### `Promise<string>`

Copied file's URI.

#### Video-Support

One can use this method also to create a thumbNail from a video in a specific size.
Currently it is impossible to specify a concrete position, the OS will decide wich
Thumbnail you'll get then.
To copy a video from assets-library and save it as a mp4-file, refer to copyAssetsVideoIOS.

Further information: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
The promise will on success return the final destination of the file, as it was defined in the destPath-parameter.

### (iOS only) `copyAssetsVideoIOS(videoUri: string, destPath: string): string | undefined`

*Not available on Mac Catalyst.*

Copies a video from assets-library, that is prefixed with 'assets-library://asset/asset.MOV?...' to a specific destination.

### `unlink(filepath: string, checkExistence?: boolean = true): void`

Unlinks the item at `filepath`. If the item does not exist, an error will be thrown.
Second parameter `checkExistence` is optional and checks the existence of a file or folder before deleting it (if the file or folder doesn't exist, an error is thrown). By default, `checkExistence = true` for backward compatibility with `react-native-fs`

Also recursively deletes directories (works like Linux `rm -rf`).

### `exists(filepath: string) => boolean`

Check if the item exists at `filepath`. If the item does not exist, return false.

### (Android only) `existsAssets(filepath: string) => boolean`

Check in the Android assets folder if the item exists. `filepath` is the relative path from the root of the assets folder. If the item does not exist, return false.

### (Android only) `existsRes(filepath: string): boolean`

Check in the Android res folder if the item named `filename` exists. `res/drawable` is used as the parent folder for image files, `res/raw` for everything else. If the item does not exist, return false.

### `hash(filepath: string, algorithm: Algorithms): string`

Reads the file at `path` and returns its checksum as determined by `algorithm`.

```ts
type Algorithms =
  | "md5"
  | "sha1"
  | "sha224"
  | "sha256"
  | "sha384"
  | "sha512";
```

### `touch(filepath: string, mtime?: Date | number, ctime?: Date | number): number`

Sets the modification timestamp `mtime` of the file at filepath.

Note: `ctime` no longer supported

### `mkdir(filepath: string, options?: MkdirOptions): void`

Create a directory at `filepath`. Automatically creates parents and does not throw if already exists (works like Linux `mkdir -p`).

```ts
type MkdirOptions =
  | {
      NSFileProtectionKey?:
        |"NSFileProtectionNone"
        | "NSFileProtectionComplete"
        | "NSFileProtectionCompleteUnlessOpen"
        | "NSFileProtectionCompleteUntilFirstUserAuthentication"
        | "NSFileProtectionCompleteWhenUserInactive"; // iOS 17+ only
      NSURLIsExcludedFromBackupKey?: boolean;
    };
```

(IOS only): `options.NSFileProtectionKey` property can be provided to set this attribute on iOS platforms.
(IOS only): The `options.NSURLIsExcludedFromBackupKey` property can be provided to set this attribute on iOS platforms. Apple will *reject* apps for storing offline cache data that does not have this attribute.

### `downloadFile(options: DownloadFileOptions, completeCallback?: DownloadResultFunc, errorCallback?: DownloadErrorFunc): DownloadResult | Promise<DownloadResult>`

Download file from `options.fromUrl` to `options.toFile`. Will overwrite any previously existing file.

```ts
type DownloadFileOptions = {
  fromUrl: string; // URL to download file from
  toFile: string; // Local filesystem path to save the file to
  headers?: { [key: string]: string | number }; // An object of headers to be passed to the server
  background?: boolean; // Continue the download in the background after the app terminates (iOS only)
  discretionary?: boolean; // Allow the OS to control the timing and speed of the download to improve perceived performance  (iOS only)
  cacheable?: boolean; // Whether the download can be stored in the shared NSURLCache (iOS only, defaults to true)
  progressInterval?: number;
  progressDivider?: number;
  begin?: (res: DownloadBeginCallbackResult) => void; // Note: it is required when progress prop provided
  progress?: (res: DownloadProgressCallbackResult) => void;
  resumable?: () => void; // only supported on iOS yet
  connectionTimeout?: number; // only supported on Android yet
  readTimeout?: number; // supported on Android and iOS
  backgroundTimeout?: number; // Maximum time (in milliseconds) to download an entire resource (iOS only, useful for timing out background downloads)
};
```

If `options.begin` is provided, it will be invoked once upon download starting when headers have been received and passed a single argument with the following properties:

```ts
type DownloadBeginCallbackResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number; // The HTTP status code
  contentLength: number; // The total size in bytes of the download resource
  headers: { [key: string]: string | number }; // The HTTP response headers from the server
};
```

If `options.progress` is provided, it will be invoked continuously and passed a single argument with the following properties:

```ts
type DownloadProgressCallbackResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  contentLength: number; // The total size in bytes of the download resource
  bytesWritten: number; // The number of bytes written to the file so far
};
```

If `options.progressInterval` is provided, it will return progress events in the maximum frequency of `progressDivider`.
For example, if `progressInterval` = 100, you will not receive callbacks more often than every 100th millisecond.

If `options.progressDivider` is provided, it will return progress events that divided by `progressDivider`.

For example, if `progressDivider` = 10, you will receive only ten callbacks for this values of progress: 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100
Use it for performance issues.
If `progressDivider` = 0, you will receive all `progressCallback` calls, default value is 0.

(IOS only): `options.background` (`Boolean`) - Whether to continue downloads when the app is not focused (default: `false`)
                           This option is currently only available for iOS, see the [Background Downloads Tutorial (iOS)](#background-downloads-tutorial-ios) section.

(IOS only): If `options.resumable` is provided, it will be invoked when the download has stopped and and can be resumed using `resumeDownload()`.

If `completeCallback` is provided, function will not return promise and `completeCallback` will be invoked after the download is complete and passed a single argument with the following properties:

```ts
type DownloadResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode?: number; // The HTTP status code
  bytesWritten?: number; // The number of bytes written to the file
};
```

If `errorCallback` is provided with `completeCallback` function it will be invoked in case of downloading error and passed a single argument with the following properties:

```ts
type DownloadError = {
  jobId: number;
  errorMessage: string;
};
```

### `stopDownload(jobId: number): void`

Abort the current download job with this ID. The partial file will remain on the filesystem.

### (iOS only) `resumeDownload(jobId: number): void`

Resume the current download job with this ID.

### (iOS only) `isResumable(jobId: number): boolean`

Check if the the download job with this ID is resumable with `resumeDownload()`.

Example:

```js
if (RNFS.isResumable(jobId)) {
    RNFS.resumeDownload(jobId)
}
```

### (iOS only) `completeHandlerIOS(jobId: number): void`

For use when using background downloads, tell iOS you are done handling a completed download.

Read more about background downloads in the [Background Downloads Tutorial (iOS)](#background-downloads-tutorial-ios) section.

### `uploadFiles(options: UploadFileOptions, completeCallback?: UploadResultFunc, errorCallback?: UploadErrorFunc): UploadResult`

### (iOS only) `stopUpload(jobId: number): void`

Abort the current upload job with this ID.

### `getFSInfo(): FSInfoResult`

Returns an object with the following properties:

```js
type FSInfoResult = {
  totalSpace: number; // The total amount of storage space on the device (in bytes).
  freeSpace: number; // The amount of available storage space on the device (in bytes).
  totalSpaceEx?: number; // The amount of available external storage space on the device (in bytes) (android only)
  freeSpaceEx?: number; // The amount of available external storage space on the device (in bytes) (android only)
  encryptionEnabled: boolean; // Check if encryption is enabled in the RNFSTurbo library
};
```

### (Android only) `scanFile(path: string, completeCallback?: ScanResultFunc): ScanResult`

Scan the file using [Media Scanner](https://developer.android.com/reference/android/media/MediaScannerConnection).

### (Android only) `getAllExternalFilesDirs(): string[]`

Returns an array with the absolute paths to application-specific directories on all shared/external storage devices where the application can place persistent files it owns.

### (iOS only) `pathForGroup(groupIdentifier: string): string`

`groupIdentifier` (`string`) Any value from the *com.apple.security.application-groups* entitlements list.

Returns the absolute path to the directory shared for all applications with the same security group identifier.
This directory can be used to to share files between application of the same developer.

Invalid group identifier will cause a rejection.

For more information read the [Adding an App to an App Group](https://developer.apple.com/library/content/documentation/Miscellaneous/Reference/EntitlementKeyReference/Chapters/EnablingAppSandbox.html#//apple_ref/doc/uid/TP40011195-CH4-SW19) section.

## Background Downloads Tutorial (iOS)

Background downloads in iOS require a bit of a setup.

First, in your `AppDelegate.m` file add the following:

```js
#import <RNFSManager.h>

...

- (void)application:(UIApplication *)application handleEventsForBackgroundURLSession:(NSString *)identifier completionHandler:(void (^)())completionHandler
{
  [RNFSManager setCompletionHandlerForIdentifier:identifier completionHandler:completionHandler];
}

```

The `handleEventsForBackgroundURLSession` method is called when a background download is done and your app is not in the foreground.

We need to pass the `completionHandler` to RNFS along with its `identifier`.

The JavaScript will continue to work as usual when the download is done but now you must call `RNFS.completeHandlerIOS(jobId)` when you're done handling the download (show a notification etc.)

**BE AWARE!** iOS will give about 30 sec. to run your code after `handleEventsForBackgroundURLSession` is called and until `completionHandler`
is triggered so don't do anything that might take a long time (like unzipping), you will be able to do it after the user re-launces the app,
otherwide iOS will terminate your app.

## Thanks

Thanks to [@mrousavy](https://github.com/mrousavy) for valuable examples of working with C++ Turbo Modules

## License

MIT
