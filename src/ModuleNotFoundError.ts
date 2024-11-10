declare global {
  // A react-native internal from TurboModuleRegistry.js
  // eslint-disable-next-line no-var, @typescript-eslint/naming-convention, no-underscore-dangle, vars-on-top
  var __turboModuleProxy: unknown | undefined;
}

const BULLET_POINT = "\n* ";

function messageWithSuggestions(
  message: string,
  suggestions: string[],
): string {
  return message + BULLET_POINT + suggestions.join(BULLET_POINT);
}

export class ModuleNotFoundError extends Error {
  constructor() {
    // TurboModule not found, something went wrong!
    // eslint-disable-next-line no-underscore-dangle
    if (global.__turboModuleProxy == null) {
      // TurboModules are not available/new arch is not enabled.
      // react-native-fs-turbo requires new arch (react-native >0.74)
      const message =
        "Failed to create a new RNFSTurbo instance: react-native-fs-turbo requires TurboModules, but the new architecture is not enabled!";
      const suggestions: string[] = [];
      const error = messageWithSuggestions(message, suggestions);
      super(error);
      return;
    }

    const message = "Failed to create a new RNFSTurbo instance.";
    const suggestions: string[] = [];

    const error = messageWithSuggestions(message, suggestions);
    super(error);
  }
}
