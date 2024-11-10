import {
  getRNFSTurboModule,
  type Configuration,
} from "./NativeRNFSTurboModule";
import type { RNFSTurboInterface } from "./Types";

export const createRNFSTurbo = (): {
  configuration: Configuration;
  instance: RNFSTurboInterface;
} => {
  const { configuration, module } = getRNFSTurboModule();

  const instance = module.createRNFSTurbo(configuration) as RNFSTurboInterface;
  if (__DEV__) {
    if (typeof instance !== "object" || instance == null) {
      throw new Error(
        "Failed to create RNFSTurbo instance - an unknown object was returned by createRNFSTurbo(..)!",
      );
    }
  }
  return { configuration, instance };
};
