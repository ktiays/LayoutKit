// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "LayoutKit",
    platforms: [
        .macOS(.v11),
        .iOS(.v13),
        .watchOS(.v6),
        .tvOS(.v13),
    ],
    products: [
        .library(name: "LayoutKit", targets: ["LayoutKit"])
    ],
    targets: [
        .target(name: "LayoutKit", dependencies: [
            "VpackBridge",
        ]),
        .target(name: "VpackBridge", publicHeadersPath: "include")
    ],
    cxxLanguageStandard: .cxx20
)
