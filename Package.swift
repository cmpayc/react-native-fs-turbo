// swift-tools-version: 6.0

import Foundation
import PackageDescription

let configHeader = Context.packageDirectory + "/ios/RNFSTurboConfig.h"
let generatedConfigDir = URL(fileURLWithPath: Context.packageDirectory).deletingLastPathComponent().path

let package = Package(
    name: "RNFSTurbo",
    platforms: [.iOS("15.1")],
    products: [
        .library(name: "RNFSTurbo", targets: ["RNFSTurbo"]),
    ],
    dependencies: [
        .package(name: "ReactNative", path: "../../../../xcframeworks"),
        .package(name: "React-GeneratedCode", path: "../../../ios"),
    ],
    targets: [
        .target(
            name: "RNFSTurbo",
            dependencies: [
                .product(name: "ReactHeaders", package: "ReactNative"),
                .product(name: "ReactNativeHeaders", package: "ReactNative"),
                .product(name: "ReactNativeDependenciesHeaders", package: "ReactNative"),
                .product(name: "ReactAppHeaders", package: "React-GeneratedCode"),
            ],
            path: ".",
            sources: [
                "cpp",
                "ios/RNFSTurboDownloader.mm",
                "ios/RNFSTurboLogger.mm",
                "ios/RNFSTurboModule.mm",
                "ios/RNFSTurboPlatformHelper.mm",
                "ios/RNFSTurboUploader.mm",
            ],
            resources: [.copy("ios/PrivacyInfo.xcprivacy")],
            publicHeadersPath: "ios",
            cxxSettings: [
                .headerSearchPath("ios"),
                .headerSearchPath("cpp"),
                .define("USE_ARM_AES"),
                .define("DEBUG", .when(configuration: .debug)),
                .define("NDEBUG", .when(configuration: .release)),
                .unsafeFlags(["-x", "objective-c++", "-include", configHeader, "-I", generatedConfigDir]),
            ],
            linkerSettings: [
                .linkedFramework("Foundation"),
                .linkedFramework("UIKit"),
                .linkedFramework("Photos"),
                .linkedFramework("AVFoundation"),
                .linkedFramework("MobileCoreServices"),
            ]
        ),
    ],
    cxxLanguageStandard: .cxx20
)
