import { createRNFSTurbo } from "./createRNFSTurbo";
import type { Configuration } from "./NativeRNFSTurboModule";
import type {
  RNFSTurboInterface,
  OverloadedStatResult,
  OverloadedReadDirItem,
  Algorithms,
  MkdirOptions,
  OverloadedReadResult,
  DownloadFileOptions,
  DownloadResult,
  DownloadError,
  DownloadResultFunc,
  DownloadErrorFunc,
  OverloadedDownloadResult,
  UploadFileOptions,
  UploadResult,
  UploadError,
  UploadResultFunc,
  UploadErrorFunc,
  OverloadedUploadResult,
  FSInfoResult,
  ScanResult,
  ScanResultFunc,
  OverloadedScanResult,
  ReadOptions,
  WriteOptions,
  MoveCopyOptions,
} from "./Types";

/**
 * A single RNFSTurbo instance.
 */
class RNFSTurbo implements RNFSTurboInterface {
  private nativeInstance: RNFSTurboInterface;

  private functionCache: Partial<RNFSTurboInterface>;

  private configuration: Configuration;

  /**
   * Creates a new RNFSTurbo instance with the given Configuration.
   * If no custom `id` is supplied, `'rnfsturbo'` will be used.
   */
  constructor() {
    const { configuration, instance } = createRNFSTurbo();
    this.nativeInstance = instance;
    this.configuration = configuration;
    this.functionCache = {};
  }

  private getFunctionFromCache<T extends keyof RNFSTurboInterface>(
    functionName: T,
  ): RNFSTurbo[T] {
    if (this.functionCache[functionName] == null) {
      this.functionCache[functionName] = this.nativeInstance[functionName];
    }
    return this.functionCache[functionName] as RNFSTurbo[T];
  }

  get MainBundlePath(): string {
    return this.configuration.mainBundlePath;
  }

  get CachesDirectoryPath(): string {
    return this.configuration.cachesDirectoryPath;
  }

  get DocumentDirectoryPath(): string {
    return this.configuration.documentDirectoryPath;
  }

  get TemporaryDirectoryPath(): string {
    return this.configuration.temporaryDirectoryPath;
  }

  get LibraryDirectoryPath(): string {
    return this.configuration.libraryDirectoryPath;
  }

  get ExternalDirectoryPath(): string {
    return this.configuration.externalDirectoryPath;
  }

  get ExternalStorageDirectoryPath(): string {
    return this.configuration.externalStorageDirectoryPath;
  }

  get ExternalCachesDirectoryPath(): string {
    return this.configuration.externalCachesDirectoryPath;
  }

  get DownloadDirectoryPath(): string {
    return this.configuration.mainBundlePath;
  }

  get PicturesDirectoryPath(): string {
    return this.configuration.picturesDirectoryPath;
  }

  get RoamingDirectoryPath(): string {
    return this.configuration.roamingDirectoryPath;
  }

  stat<T extends boolean | undefined = false>(
    filepath: string,
    isNewFormat?: T,
  ): OverloadedStatResult<T> {
    const func = this.getFunctionFromCache("stat");
    return func(filepath, isNewFormat);
  }

  readDir<T extends boolean | undefined = false>(
    dirpath: string,
    isNewFormat?: T,
  ): OverloadedReadDirItem<T>[] {
    const func = this.getFunctionFromCache("readDir");
    return func(dirpath, isNewFormat);
  }

  readDirAssets<T extends boolean | undefined = false>(
    dirpath: string,
    isNewFormat?: T,
  ): OverloadedReadDirItem<T>[] {
    const func = this.getFunctionFromCache("readDirAssets");
    return func(dirpath, isNewFormat);
  }

  readdir(dirpath: string): string[] {
    const func = this.getFunctionFromCache("readdir");
    return func(dirpath);
  }

  readFile<T extends ReadOptions = undefined>(
    filepath: string,
    options?: T,
  ): OverloadedReadResult<T> {
    const func = this.getFunctionFromCache("readFile");
    return func(filepath, options);
  }

  read<T extends ReadOptions = undefined>(
    filepath: string,
    length: number,
    position: number,
    options?: T,
  ): OverloadedReadResult<T> {
    const func = this.getFunctionFromCache("read");
    return func(filepath, length, position, options);
  }

  readFileAssets(filepath: string, options?: ReadOptions): string[] {
    const func = this.getFunctionFromCache("readFileAssets");
    return func(filepath, options);
  }

  readFileRes(filepath: string, options?: ReadOptions): string[] {
    const func = this.getFunctionFromCache("readFileRes");
    return func(filepath, options);
  }

  writeFile(
    filepath: string,
    contents: string | number[],
    options?: WriteOptions,
  ): void {
    const func = this.getFunctionFromCache("writeFile");
    return func(filepath, contents, options);
  }

  appendFile(
    filepath: string,
    contents: string | number[],
    options?: WriteOptions,
  ): void {
    const func = this.getFunctionFromCache("appendFile");
    return func(filepath, contents, options);
  }

  write(
    filepath: string,
    contents: string | number[],
    position?: number,
    options?: WriteOptions,
  ): void {
    const func = this.getFunctionFromCache("write");
    return func(filepath, contents, position, options);
  }

  moveFile(
    filepath: string,
    destPath: string,
    options?: MoveCopyOptions,
  ): void {
    const func = this.getFunctionFromCache("moveFile");
    return func(filepath, destPath, options);
  }

  copyFolder(
    srcFolderPath: string,
    destFolderPath: string,
    options?: MoveCopyOptions,
  ): void {
    const func = this.getFunctionFromCache("copyFolder");
    return func(srcFolderPath, destFolderPath, options);
  }

  copyFile(
    filepath: string,
    destPath: string,
    options?: MoveCopyOptions,
  ): void {
    const func = this.getFunctionFromCache("copyFile");
    return func(filepath, destPath, options);
  }

  copyFileAssets(filepath: string, destPath: string): void {
    const func = this.getFunctionFromCache("copyFileAssets");
    return func(filepath, destPath);
  }

  copyFileRes(filepath: string, destPath: string): void {
    const func = this.getFunctionFromCache("copyFileRes");
    return func(filepath, destPath);
  }

  copyAssetsFileIOS(
    imageUri: string,
    destPath: string,
    width: number,
    height: number,
    scale?: number,
    compression?: number,
    resizeMode?: string,
  ): string | undefined {
    const func = this.getFunctionFromCache("copyAssetsFileIOS");
    return func(
      imageUri,
      destPath,
      width,
      height,
      scale,
      compression,
      resizeMode,
    );
  }

  copyAssetsVideoIOS(videoUri: string, destPath: string): string | undefined {
    const func = this.getFunctionFromCache("copyAssetsVideoIOS");
    return func(videoUri, destPath);
  }

  unlink(filepath: string): void {
    const func = this.getFunctionFromCache("unlink");
    return func(filepath);
  }

  exists(filepath: string): boolean {
    const func = this.getFunctionFromCache("exists");
    return func(filepath);
  }

  existsAssets(filepath: string): boolean {
    const func = this.getFunctionFromCache("existsAssets");
    return func(filepath);
  }

  existsRes(filepath: string): boolean {
    const func = this.getFunctionFromCache("existsRes");
    return func(filepath);
  }

  hash(filepath: string, algorithm: Algorithms): string {
    const func = this.getFunctionFromCache("hash");
    return func(filepath, algorithm);
  }

  touch(
    filepath: string,
    mtime?: Date | number,
    ctime?: Date | number,
  ): number {
    const func = this.getFunctionFromCache("touch");
    return func(filepath, mtime?.valueOf(), ctime?.valueOf());
  }

  mkdir(filepath: string, options?: MkdirOptions): void {
    const func = this.getFunctionFromCache("mkdir");
    return func(filepath, options);
  }

  downloadFile<T extends DownloadResultFunc = undefined>(
    options: DownloadFileOptions,
    completeCallback?: T,
    errorCallback?: DownloadErrorFunc,
  ): OverloadedDownloadResult<T> {
    const funcResult: {
      resolve: DownloadResult | null;
      reject: DownloadError | null;
    } = {
      resolve: null,
      reject: null,
    };
    const func = this.getFunctionFromCache("downloadFile");
    const tmpCallbackFunc =
      completeCallback ||
      ((res: DownloadResult) => {
        funcResult.resolve = res;
      });
    const tmpErrorFunc =
      errorCallback ||
      ((res: DownloadError) => {
        funcResult.reject = res;
      });

    const promise = !completeCallback
      ? new Promise<DownloadResult>((resolve, reject) => {
          const callbackInterval = setInterval(() => {
            if (funcResult.resolve) {
              clearInterval(callbackInterval);
              resolve(funcResult.resolve);
            } else if (funcResult.reject) {
              clearInterval(callbackInterval);
              reject(funcResult.reject);
            }
          }, 100);
        })
      : undefined;

    const jobId: any = func(options, tmpCallbackFunc, tmpErrorFunc);

    if (completeCallback) {
      return { jobId } as OverloadedDownloadResult<T>;
    }

    return {
      jobId,
      promise,
    } as OverloadedDownloadResult<T>;
  }

  stopDownload(jobId: number): void {
    const func = this.getFunctionFromCache("stopDownload");
    return func(jobId);
  }

  resumeDownload(jobId: number): void {
    const func = this.getFunctionFromCache("resumeDownload");
    return func(jobId);
  }

  isResumable(jobId: number): boolean {
    const func = this.getFunctionFromCache("isResumable");
    return func(jobId);
  }

  completeHandlerIOS(jobId: number): void {
    const func = this.getFunctionFromCache("completeHandlerIOS");
    return func(jobId);
  }

  uploadFiles<T extends UploadResultFunc = undefined>(
    options: UploadFileOptions,
    completeCallback?: T,
    errorCallback?: UploadErrorFunc,
  ): OverloadedUploadResult<T> {
    const funcResult: {
      resolve: UploadResult | null;
      reject: UploadError | null;
    } = {
      resolve: null,
      reject: null,
    };
    const func = this.getFunctionFromCache("uploadFiles");
    const tmpCallbackFunc =
      completeCallback ||
      ((res: UploadResult) => {
        funcResult.resolve = res;
      });
    const tmpErrorFunc =
      errorCallback ||
      ((res: UploadError) => {
        funcResult.reject = res;
      });

    const promise = !completeCallback
      ? new Promise<UploadResult>((resolve, reject) => {
          const callbackInterval = setInterval(() => {
            if (funcResult.resolve) {
              clearInterval(callbackInterval);
              resolve(funcResult.resolve);
            } else if (funcResult.reject) {
              clearInterval(callbackInterval);
              reject(funcResult.reject);
            }
          }, 100);
        })
      : undefined;

    const jobId: any = func(options, tmpCallbackFunc, tmpErrorFunc);

    if (completeCallback) {
      return { jobId } as OverloadedUploadResult<T>;
    }

    return {
      jobId,
      promise,
    } as OverloadedUploadResult<T>;
  }

  stopUpload(jobId: number): void {
    const func = this.getFunctionFromCache("stopUpload");
    return func(jobId);
  }

  getFSInfo(): FSInfoResult {
    const func = this.getFunctionFromCache("getFSInfo");
    return func();
  }

  scanFile<T extends ScanResultFunc = undefined>(
    path: string,
    completeCallback?: T,
  ): OverloadedScanResult<T> {
    const funcResult: {
      resolve: ScanResult | null;
    } = {
      resolve: null,
    };
    const func = this.getFunctionFromCache("scanFile");
    const tmpCallbackFunc =
      completeCallback ||
      ((res: ScanResult) => {
        funcResult.resolve = res;
      });

    const promise = !completeCallback
      ? new Promise<ScanResult | string>((resolve) => {
          const callbackInterval = setInterval(() => {
            if (funcResult.resolve) {
              clearInterval(callbackInterval);
              resolve(funcResult.resolve.path);
            }
          }, 100);
        })
      : undefined;

    const jobId: any = func(path, tmpCallbackFunc);

    if (completeCallback) {
      return { jobId } as OverloadedScanResult<T>;
    }

    return promise as OverloadedScanResult<T>;
  }

  getAllExternalFilesDirs(): string[] {
    const func = this.getFunctionFromCache("getAllExternalFilesDirs");
    return func();
  }

  pathForGroup(groupIdentifier: string): string {
    const func = this.getFunctionFromCache("pathForGroup");
    return func(groupIdentifier);
  }
}

const RNFSTurboInstance = new RNFSTurbo();

export default RNFSTurboInstance;
