# Build and run FrontierX

FrontierX currently targets developers and early playtesters. An optimized local
single-player candidate is prepared; public binary distribution is not cleared.
You need a lawfully obtained retail installation to play.

## Prepare your game data

Keep retail data, fonts, saves, and runtime output outside the checkout. Historical
development resources in this repository do not replace a retail installation.

1. Build the native targets with the commands below.
2. Prepare external fonts, speech, and movies with the [media guide](NATIVE_TERRAN_MEDIA.md).
3. Follow the [playtest setup and launch guide](LINUX_PLAYTEST_CI.md) to configure
   your installation, preserve your saves, and prepare a candidate.

Training uses `--mission-campaign u`; the playtest guide supplies the remaining
required arguments. Default builds leave optional fonts and FFmpeg support off.

## Build from source


### Linux

Install CMake 3.25 or newer, Ninja, GCC or Clang with a C++ toolchain, Python 3,
LLVM's `llvm-rc`, `pkg-config`, SDL3 development files, and shaderc's `glslc`.
Visible rendering requires a working Vulkan driver.

From the repository root:

```sh
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

Use `linux-clang-debug` for Clang. These presets build native development targets,
the shell, and regression checks without retail data. GPU checks require a real
display; a skipped check does not verify rendering.

### Enable fonts, speech, and movies

A full-media build requires FreeType 2.14 or newer, external original-family font
faces, and the reviewed shared-LGPL FFmpeg configuration. Follow the
[dependency record](DEPENDENCIES.md) before configuring:

```sh
cmake --preset linux-gcc-debug \
  -DFRONTIERX_BUILD_NATIVE_FONTS=ON \
  -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON \
  -DFRONTIERX_BUILD_FFMPEG_VIDEO=ON \
  -DCMAKE_PREFIX_PATH=/external/lgpl-prefix
cmake --build --preset linux-gcc-debug
```

Replace `/external/lgpl-prefix` with your reviewed dependency prefix. Supply the
external font mapping through `--ui-font-map`. The [playtest guide](LINUX_PLAYTEST_CI.md)
describes candidate preparation and launch.


### Windows baseline

The supported restoration build requires Windows 10 or 11, Visual Studio 2022
with the C++ desktop workload, Python 3 with the `py` launcher, and the June 2010
DirectX SDK in `DXSDK` beside the repository.

Rebuild without deploying into a retail installation from PowerShell:

```powershell
.\Build-Conquest.ps1 -Configuration Debug -NoDeploy
.\Build-Conquest.ps1 -Configuration Final -NoDeploy
```

See the [Windows restoration handoff](../HANDOFF.md) and [retail test matrix](../RETAIL_TEST_MATRIX.md).
Runtime testing also requires the DirectX 9 runtime components and retail data.

## Check current limitations

Read the [single-player finalization record](RETAIL_SINGLEPLAYER_FINALIZATION.md)
for gameplay acceptance and outstanding checks. The [dependency inventory](DEPENDENCIES.md)
and [publication review](PUBLICATION_REVIEW.md) track distribution requirements.

These instructions were moved from the README on 2026-11-07 without changing
commands. This documentation change adds no build or runtime evidence.
