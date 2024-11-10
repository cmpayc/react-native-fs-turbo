import type { TurboModule } from "react-native";
import { TurboModuleRegistry } from "react-native";
import { ModuleNotFoundError } from "./ModuleNotFoundError";

export interface Spec extends TurboModule {
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

export function getRNFSTurboPlatformContextTurboModule(): Spec {
  try {
    if (module == null) {
      // 1. Get the TurboModule
      module = TurboModuleRegistry.getEnforcing<Spec>(
        "RNFSTurboPlatformContextModule",
      );
    }
    return module;
  } catch {
    // TurboModule could not be found!
    throw new ModuleNotFoundError();
  }
}
