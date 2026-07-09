# Android Build Notes

This directory contains the Android Gradle project for the Zelda64 Recompiled Android port.

## Build

From the repository root:

```sh
ANDROID_HOME="$HOME/Library/Android/sdk" tools/ci/setup_android_deps.sh
tools/ci/prepare_android_generated_sources.sh
gradle --no-daemon -p android :app:assembleRelease -PzeldaProbe=false
```

Use release APKs for gameplay, FPS, and frame pacing testing. Debug APKs are useful for quick developer checks, but they are not a good baseline for gaming performance.

Release signing is read from environment variables or Gradle properties:

```sh
ANDROID_KEYSTORE_PATH=/path/to/release-keystore.jks \
ANDROID_KEYSTORE_PASSWORD=... \
ANDROID_KEY_ALIAS=android-release \
ANDROID_KEY_PASSWORD=... \
gradle --no-daemon -p android :app:assembleRelease -PzeldaProbe=false
```

## GitHub Actions

The `Build Android APK` workflow can be run manually from the Actions tab. It builds a signed release APK by default and uploads it as an artifact when the generated runtime sources are already available to the runner.

GitHub cannot generate those sources by itself without the user's own decompressed ROM input. If the workflow reports missing `RecompiledFuncs`, `rsp`, or `RecompiledPatches` files, generate them locally first with:

```sh
tools/ci/prepare_android_generated_sources.sh
```

The workflow needs these repository secrets to sign release APKs:

- `ANDROID_KEYSTORE_BASE64`
- `ANDROID_KEYSTORE_PASSWORD`
- `ANDROID_KEY_ALIAS`
- `ANDROID_KEY_PASSWORD`

For GitHub-hosted release builds, the workflow also needs access to a private generated-source ZIP because ROM-derived generated sources are not committed to this public repo. Set:

- `ZELDA_ANDROID_GENERATED_SOURCES_URL`
- `ZELDA_ANDROID_GENERATED_SOURCES_TOKEN`, only if the URL needs a bearer token

The ZIP should extract these directories at the repository root:

```text
RecompiledFuncs/
RecompiledPatches/
rsp/
```

The default probe target and debug runtime APK can still be built for quick SDL lifecycle checks:

```sh
gradle --no-daemon -p android :app:assembleDebug
```

For Samsung or driver-specific Vulkan startup debugging, build the minimal Vulkan smoke probe. This uses the normal Android activity and SDL surface path, but skips the game runtime, generated code, bundled mods, and RT64:

```sh
gradle --no-daemon -p android :app:assembleDebug -PzeldaVulkanProbe=true
```

## Native Binary Checks

Android release builds must keep every packaged native shared object 16 KiB page-size compatible. This matters on newer Android 15/16 devices and is checked by:

```sh
tools/ci/verify_android_apk.sh path/to.apk runtime
```

The dependency setup script also rebuilds SDL2 when the cached `libSDL2.so` was produced with old 4 KiB `PT_LOAD` alignment. To check the device page size during testing:

```sh
adb shell getconf PAGE_SIZE
```

## Required Inputs

- `ANDROID_HOME`, defaulting to `$HOME/Library/Android/sdk`
- `ANDROID_NDK_HOME`, defaulting to `$ANDROID_HOME/ndk/28.2.13676358`
- `ZELDA_ANDROID_SDL2_PREFIX`, defaulting to `$HOME/Android/prefixes/SDL2-2.32.10-android-arm64`
- `ZELDA_ANDROID_FREETYPE_PREFIX`, required for full runtime builds

## Runtime Paths

`ZeldaSDLActivity` extracts program assets from the APK and sets:

- `APP_PROGRAM_PATH` to `<app files>/program`
- `APP_FOLDER_PATH` to `/sdcard/Zelda64` when storage access is granted
- `APP_NATIVE_LIBS_PATH` to `<app code cache>/native_mods`
- `APP_ANDROID_VERSION_NAME` to the APK `versionName`

If storage access is not granted yet, the app temporarily falls back to app-private data storage until the user grants permission.

## Bundled Android Mods

Android-native mod packages live under:

```text
android/app/src/main/assets/bundled_mods/
```

These are copied into `/sdcard/Zelda64/mods` on launch. The current bundled packages are:

- `ProxyMM_KV.nrm`
- `ProxyRecomp_KV005.so`
- `yazmt_mm_corelib.nrm`
- `yazmt_mm_global_objects.nrm`
- `yazmt_mm_playermodelmanager.nrm`
- `yazmt_mm_playermodelmanager_fsmodels.nrm`
- `yazmt_mm_playermodelmanager_fsmodels_extlib.so`
- `magemods_audio_api.nrm`
- `magemods_audio_api.so`

They are bundled as complete Android-ready mod pairs because desktop native binaries are not compatible with Android. Keeping the packages in the APK prevents users from accidentally replacing the Android `.so` files with Linux or desktop versions from normal mod downloads.

## Do Not Commit

Do not commit ROMs, decompressed ROMs, generated recompilation sources from private inputs, APKs, AABs, Gradle output, CMake output, app-private saves, configs, or imported user mods.
