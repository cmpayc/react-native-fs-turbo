/* eslint-disable import/no-import-module-exports */
import type { TurboModule } from "react-native";
import { TurboModuleRegistry } from "react-native";
import { ModuleNotFoundError } from "./ModuleNotFoundError";

/**
 * Used for configuration of a single RNFSTurbo instance.
 */
export interface Configuration {
  mainBundlePath: string;
  cachesDirectoryPath: string;
  documentDirectoryPath: string;
  temporaryDirectoryPath: string;
  libraryDirectoryPath: string;
  externalDirectoryPath: string;
  externalStorageDirectoryPath: string;
  externalCachesDirectoryPath: string;
  downloadDirectoryPath: string;
  picturesDirectoryPath: string;
  roamingDirectoryPath: string;
}

/**
 * Used for configuration of a single RNFSTurbo instance.
 */

export interface Spec extends TurboModule {
  /**
   * Create a new instance of RNFSTurbo.
   */
  readonly createRNFSTurbo: () => boolean;
  getMainBundlePath(): string;
  getCachesDirectoryPath(): string;
  getDocumentDirectoryPath(): string;
  getTemporaryDirectoryPath(): string;
  getLibraryDirectoryPath(): string;
  getExternalDirectoryPath(): string;
  getExternalStorageDirectoryPath(): string;
  getExternalCachesDirectoryPath(): string;
  getDownloadDirectoryPath(): string;
  getPicturesDirectoryPath(): string;
  getRoamingDirectoryPath(): string;
}

let module: Spec | null;
let configuration: Configuration;

export function getRNFSTurboModule(): {
  configuration: Configuration;
  module: Spec;
} {
  try {
    if (module == null) {
      // 1. Load RNFS TurboModule
      module = TurboModuleRegistry.getEnforcing<Spec>("RNFSTurboModule");

      // 2. Create platform-specific configuration
      configuration = {
        mainBundlePath: module.getMainBundlePath(),
        cachesDirectoryPath: module.getCachesDirectoryPath(),
        documentDirectoryPath: module.getDocumentDirectoryPath(),
        temporaryDirectoryPath: module.getTemporaryDirectoryPath(),
        libraryDirectoryPath: module.getLibraryDirectoryPath(),
        externalDirectoryPath: module.getExternalDirectoryPath(),
        externalStorageDirectoryPath: module.getExternalStorageDirectoryPath(),
        externalCachesDirectoryPath: module.getExternalCachesDirectoryPath(),
        downloadDirectoryPath: module.getDownloadDirectoryPath(),
        picturesDirectoryPath: module.getPicturesDirectoryPath(),
        roamingDirectoryPath: module.getRoamingDirectoryPath(),
      };
    }

    return { configuration, module };
  } catch {
    // TurboModule could not be found!
    throw new ModuleNotFoundError();
  }
}
