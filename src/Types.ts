export type StatResult<S, T extends Date | number> = {
  path: string; // The same as filepath argument
  ctime: T; // The creation date of the file (Date for old format, number (unixtime, sec) for new format)
  mtime: T; // The last modified date of the file (Date for old format, number (unixtime, sec) for new format)
  size: number; // Size in bytes
  mode: number; // UNIX file mode
  originalFilepath: string; // ANDROID: In case of content uri this is the pointed file path, otherwise is the same as path
  isFile: S; // Is the file just a file?
  isDirectory: S; // Is the file a directory?
};

export type ReadDirItem<S, T extends Date | number> = {
  ctime: T; // The creation date of the file (iOS only) (Date for old format, number (unixtime, sec) for new format)
  mtime: T; // The last modified date of the file (Date for old format, number (unixtime, sec) for new format)
  name: string; // The name of the item
  path: string; // The absolute path to the item
  size: string; // Size in bytes
  isFile: S; // Is the file just a file?
  isDirectory: S; // Is the file a directory?
};

export type DownloadFileOptions = {
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

export type DownloadResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number; // The HTTP status code
  bytesWritten: number; // The number of bytes written to the file
};

export type DownloadError = {
  jobId: number;
  errorMessage: string;
};

export type DownloadBeginCallbackResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number; // The HTTP status code
  contentLength: number; // The total size in bytes of the download resource
  headers: { [key: string]: string | number }; // The HTTP response headers from the server
};

export type DownloadProgressCallbackResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  contentLength: number; // The total size in bytes of the download resource
  bytesWritten: number; // The number of bytes written to the file so far
};

export type UploadFileOptions = {
  toUrl: string; // URL to upload file to
  binaryStreamOnly?: boolean; // Allow for binary data stream for file to be uploaded without extra headers, Default is 'false'
  files: UploadFileItem[]; // An array of objects with the file information to be uploaded.
  headers?: { [key: string]: string | number }; // An object of headers to be passed to the server
  fields?: { [key: string]: string | number }; // An object of fields to be passed to the server
  method?: string; // Default is 'POST', supports 'POST' and 'PUT'
  begin?: (res: UploadBeginCallbackResult) => void;
  progress?: (res: UploadProgressCallbackResult) => void;
};

export type UploadFileItem = {
  name: string; // Name of the file, if not defined then filename is used
  filename: string; // Name of file
  filepath: string; // Path to file
  filetype: string; // The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension
};

export type UploadResult = {
  jobId: number; // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  statusCode: number; // The HTTP status code
  headers: { [key: string]: string | number }; // The HTTP response headers from the server
  body: string; // The HTTP response body
};

export type UploadError = {
  jobId: number;
  errorMessage: string;
};

export type UploadBeginCallbackResult = {
  jobId: number; // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
};

export type UploadProgressCallbackResult = {
  jobId: number; // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  totalBytesExpectedToSend: number; // The total number of bytes that will be sent to the server
  totalBytesSent: number; // The number of bytes sent to the server
};

export type FSInfoResult = {
  totalSpace: number; // The total amount of storage space on the device (in bytes).
  freeSpace: number; // The amount of available storage space on the device (in bytes).
  totalSpaceEx?: number; // The amount of available external storage space on the device (in bytes) (android only)
  freeSpaceEx?: number; // The amount of available external storage space on the device (in bytes) (android only)
};

export type ScanResult = {
  jobId: number;
  path: string;
};

export type ScanError = {
  jobId: number;
  errorMessage: string;
};

export type Algorithms =
  | "md5"
  | "sha1"
  | "sha224"
  | "sha256"
  | "sha384"
  | "sha512";

export type DownloadResultFunc = ((res: DownloadResult) => void) | undefined;
export type DownloadErrorFunc = ((res: DownloadError) => void) | undefined;

export type UploadResultFunc = ((res: UploadResult) => void) | undefined;
export type UploadErrorFunc = ((res: UploadError) => void) | undefined;

export type ScanResultFunc = ((res: ScanResult) => void) | undefined;

export type OverloadedStatResult<S extends boolean | undefined> = S extends
  | false
  | undefined
  ? StatResult<() => boolean, Date>
  : StatResult<boolean, number>;

export type OverloadedReadDirItem<T extends boolean | undefined> = T extends
  | false
  | undefined
  ? ReadDirItem<() => boolean, Date>
  : ReadDirItem<boolean, number>;

export type OverloadedReadResult<T extends ReadOptions> = T extends
  | "uint8"
  | "float32"
  | Partial<{
      encoding: "uint8" | "float32";
    }>
  ? number[]
  : string;

export type OverloadedDownloadResult<T extends DownloadResultFunc> =
  T extends undefined
    ? { jobId: number; promise: Promise<DownloadResult> }
    : { jobId: number };

export type OverloadedUploadResult<T extends UploadResultFunc> =
  T extends undefined
    ? { jobId: number; promise: Promise<UploadResult> }
    : { jobId: number };

export type OverloadedScanResult<T extends ScanResultFunc> = T extends undefined
  ? Promise<ScanResult>
  : { jobId: number };

export type EncodingType = "utf8" | "base64" | "uint8" | "float32" | "ascii";

export type IOSProtectionTypes =
  | "NSFileProtectionNone"
  | "NSFileProtectionComplete"
  | "NSFileProtectionCompleteUnlessOpen"
  | "NSFileProtectionCompleteUntilFirstUserAuthentication"
  | "NSFileProtectionCompleteWhenUserInactive";

export type ReadOptions =
  | EncodingType
  | {
      encoding?: EncodingType;
    }
  | undefined;

export type WriteOptions =
  | EncodingType
  | {
      encoding?: EncodingType;
      NSFileProtectionKey?: IOSProtectionTypes;
    }
  | undefined;

export type MoveCopyOptions =
  | {
      NSFileProtectionKey?: IOSProtectionTypes;
      NSURLIsExcludedFromBackupKey?: boolean;
    }
  | undefined;

export type MkdirOptions =
  | {
      NSFileProtectionKey?: IOSProtectionTypes;
      NSURLIsExcludedFromBackupKey?: boolean;
    }
  | undefined;

/**
 * Represents a single RNFSTurbo instance.
 */
export interface RNFSTurboInterface {
  readonly MainBundlePath: string;
  readonly CachesDirectoryPath: string;
  readonly DocumentDirectoryPath: string;
  readonly TemporaryDirectoryPath: string;
  readonly LibraryDirectoryPath: string;
  readonly ExternalDirectoryPath: string;
  readonly ExternalStorageDirectoryPath: string;
  readonly ExternalCachesDirectoryPath: string;
  readonly DownloadDirectoryPath: string;
  readonly PicturesDirectoryPath: string;
  readonly RoamingDirectoryPath: string;

  stat<T extends boolean | undefined = false>(
    filepath: string,
    isNewFormat?: T,
  ): OverloadedStatResult<T>;
  readDir<T extends boolean | undefined = false>(
    dirpath: string,
    isNewFormat?: T,
  ): OverloadedReadDirItem<T>[];
  readDirAssets<T extends boolean | undefined = false>(
    dirpath: string,
    isNewFormat?: T,
  ): OverloadedReadDirItem<T>[];
  readdir(dirpath: string): string[];
  readFile<T extends ReadOptions = undefined>(
    filepath: string,
    options?: T,
  ): OverloadedReadResult<T>;
  read<T extends ReadOptions = undefined>(
    filepath: string,
    length: number,
    position: number,
    options?: T,
  ): OverloadedReadResult<T>;
  readFileAssets(filepath: string, options?: ReadOptions): string[];
  readFileRes(filepath: string, options?: ReadOptions): string[];
  writeFile(
    filepath: string,
    contents: string | number[],
    options?: WriteOptions,
  ): void;
  appendFile(
    filepath: string,
    contents: string | number[],
    options?: WriteOptions,
  ): void;
  write(
    filepath: string,
    contents: string | number[],
    position?: number,
    options?: WriteOptions,
  ): void;
  moveFile(filepath: string, destPath: string, options?: MoveCopyOptions): void;
  copyFolder(
    srcFolderPath: string,
    destFolderPath: string,
    options?: MoveCopyOptions,
  ): void;
  copyFile(filepath: string, destPath: string, options?: MoveCopyOptions): void;
  copyFileAssets(filepath: string, destPath: string): void;
  copyFileRes(filepath: string, destPath: string): void;
  copyAssetsFileIOS(
    imageUri: string,
    destPath: string,
    width: number,
    height: number,
    scale?: number,
    compression?: number,
    resizeMode?: string,
  ): string | undefined;
  copyAssetsVideoIOS(videoUri: string, destPath: string): string | undefined;
  unlink(filepath: string, checkExistence?: boolean): void;
  exists(filepath: string): boolean;
  existsAssets(filepath: string): boolean;
  existsRes(filepath: string): boolean;
  hash(filepath: string, algorithm: Algorithms): string;
  touch(filepath: string, mtime?: Date | number, ctime?: Date | number): number;
  mkdir(filepath: string, options?: MkdirOptions): void;
  downloadFile<T extends DownloadResultFunc | undefined = undefined>(
    options: DownloadFileOptions,
    completeCallback?: T,
    errorCallback?: DownloadErrorFunc,
  ): OverloadedDownloadResult<T>;
  stopDownload(jobId: number): void;
  resumeDownload(jobId: number): void;
  isResumable(jobId: number): boolean;
  completeHandlerIOS(jobId: number): void;
  uploadFiles<T extends UploadResultFunc = undefined>(
    options: UploadFileOptions,
    completeCallback?: T,
    errorCallback?: UploadErrorFunc,
  ): OverloadedUploadResult<T>;
  stopUpload(jobId: number): void;
  getFSInfo(): FSInfoResult;
  scanFile<T extends ScanResultFunc = undefined>(
    path: string,
    completeCallback?: T,
  ): OverloadedScanResult<T>;
  getAllExternalFilesDirs(): string[];
  pathForGroup(groupIdentifier: string): string;
}
