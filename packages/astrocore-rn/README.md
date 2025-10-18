# AstroCore React Native Bindings

This package exposes the AstroCore C++ engine to React Native via JSI with a zero-copy Float32Array surface. It ships as a reusable package with shared C++ sources for iOS and Android while keeping the JavaScript API ergonomic.

## Features

- One-time `install()` helper that registers the native bindings for a `jsi::Runtime`.
- Minimal JS/TS wrapper that returns typed views backed by native ring buffers.
- Shared C++ core compiled for both iOS and Android with identical build flags.
- Optional React hook for wiring device pose updates to `computeFrame`.

## Directory Overview

- `src/` – TypeScript surface exported to React Native apps.
- `ios/` and `android/` – Platform glue code that installs the JSI bindings.
- `cpp/` – Platform-agnostic C++ implementation of AstroCore and unit tests.

## Getting Started

1. Ensure your React Native app can compile native modules (CocoaPods + Android NDK).
2. Add this package to the monorepo (e.g. with Yarn workspaces).
3. Call `AstroCore.install()` on app launch before using the API.
4. Feed star catalogs and observer settings once, then stream pose updates through `updatePose`.

See `src/SkyEngine.ts` for the expected JavaScript surface and `cpp/src/jsi_bindings.cpp` for the native host object contract.
