import type { TurboModule } from "react-native";
import { TurboModuleRegistry } from "react-native";
import { UnsafeObject } from "react-native/Libraries/Types/CodegenTypes";
import { ModuleNotFoundError } from "./ModuleNotFoundError";
import { getRNFSTurboPlatformContextTurboModule } from "./NativeRNFSTurboPlatformContextModule";

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
   * The returned {@linkcode UnsafeObject} is a `jsi::HostObject`.
   */
  readonly createRNFSTurbo: (configuration: Configuration) => UnsafeObject;
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

      // 2. Get the PlatformContext TurboModule as well
      const platformContext = getRNFSTurboPlatformContextTurboModule();

      // 3. Initialize it with the storage directores from platform-specific context
      configuration = {
        mainBundlePath: platformContext.getMainBundlePath(),
        cachesDirectoryPath: platformContext.getCachesDirectoryPath(),
        documentDirectoryPath: platformContext.getDocumentDirectoryPath(),
        temporaryDirectoryPath: platformContext.getTemporaryDirectoryPath(),
        libraryDirectoryPath: platformContext.getLibraryDirectoryPath(),
        externalDirectoryPath: platformContext.getExternalDirectoryPath(),
        externalStorageDirectoryPath:
          platformContext.getExternalStorageDirectoryPath(),
        externalCachesDirectoryPath:
          platformContext.getExternalCachesDirectoryPath(),
        downloadDirectoryPath: platformContext.getDownloadDirectoryPath(),
        picturesDirectoryPath: platformContext.getPicturesDirectoryPath(),
        roamingDirectoryPath: platformContext.getRoamingDirectoryPath(),
      };
    }

    return { configuration, module };
  } catch {
    // TurboModule could not be found!
    throw new ModuleNotFoundError();
  }
}
