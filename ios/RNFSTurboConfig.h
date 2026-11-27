// Build configuration for Swift Package Manager builds.
//
// Package.swift force-includes this header into every RNFSTurbo source file.
// spm/autolinking-plugin.js writes RNFSTurboConfig.generated.h into the app's
// ios/build/generated/autolinking/plugin-libs directory (added to the include
// path by Package.swift) on `npx react-native spm update` (or `add`), based on
// the app's package.json ("rnFsTurbo": { "useEncryption": true }) or the
// RNFSTURBO_USE_ENCRYPTION environment variable.
//
// CocoaPods builds don't use this header; they enable encryption with the
// RNFSTURBO_USE_ENCRYPTION preprocessor definition in the Podfile.

#pragma once

#if __has_include(<RNFSTurboConfig.generated.h>)
#include <RNFSTurboConfig.generated.h>
#endif
