# FrontierX

FrontierX is a community project to bring *Conquest: Frontier Wars* to native
64-bit Linux. It preserves the original C and C++ engine, retail gameplay, and
save formats while replacing Windows dependencies behind the engine's existing
interfaces.

The immediate goal is a playable single-player campaign. The Windows restoration
remains the regression baseline, and Linux campaign support is expanding mission
by mission.

**A lawfully obtained retail installation is required to play.** Retail game data
is not included. FrontierX is a modified version of the Program, released solely
under the [original Fever Pitch Studios license](Conquest%20Source%20License.txt).
The entire derivative and its recipients are subject to those exact terms.

## Current state

Status as of **2026-10-19**. FrontierX is in active development and is intended for
developers and early playtesters. Expect defects and incomplete campaign coverage.

| Area | Current scope |
|---|---|
| Native Linux | Linux x86_64, SDL3 window/input/audio, and Vulkan rendering through SDL GPU |
| Training | Original Terran training accepted in its documented Linux scope |
| Terran campaign | Missions 1–6 accepted through user playtests in their reported scope |
| Later missions | Missions 7–12 have varying integration and test coverage; missions 13–16 are not yet linked |
| Presentation | Models, textures, effects, HUD, minimaps, portraits, speech, and movies are integrated for selected missions; complete coverage remains mission-specific |
| Windows | VS2022 Win32 restoration remains the supported baseline; affected native Windows rebuild and module-ABI checks remain open for current Linux changes |
| Other platforms and multiplayer | Later work |

Acceptance records describe particular playthroughs, not every unit, scenario, or
hardware configuration. Later-mission crash repairs, missing media, whole-process
leaks, and broader compatibility checks remain open. Full cross-platform
determinism and retail parity have not been established.

See the [campaign mission matrix](docs/NATIVE_TERRAN_P2.md#mission-matrix) for
individual mission status and the [active delivery plan](docs/PORT_READINESS.md#active-plan-playable-single-player-first-2026-10-13)
for priorities. Focused documents distinguish automated checks, synthetic runtime,
retail-data runtime, and user gameplay evidence.

## Build on Linux

The checked-in GCC and Clang presets require:

- Linux x86_64, CMake 3.25 or newer, and Ninja.
- GCC or Clang with a C++ toolchain.
- Python 3 and LLVM's resource compiler, `llvm-rc`.
- `pkg-config` and SDL3 development files.
- shaderc's `glslc` shader compiler.

Visible native rendering also requires a working Vulkan driver. Consult the
[dependency record](docs/DEPENDENCIES.md) for reviewed versions, provenance, and
optional dependencies.

From the repository root, configure, build, and run the checks:

```sh
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug
```

Replace `linux-gcc-debug` with `linux-clang-debug` for Clang. These commands build
the native development targets, shell, and regression checks. Building them does
not require retail data. Some visible GPU checks need a real display; a skipped
check is not evidence that rendering works on your machine.

### Prepare a playable candidate

The default configuration leaves optional fonts and FFmpeg media support off.
A full-media campaign candidate requires FreeType 2.14 or newer, external font
faces, and the reviewed external shared-LGPL FFmpeg configuration for speech and
movies.

After preparing those dependencies, configure the optional build:

```sh
cmake --preset linux-gcc-debug \
  -DFRONTIERX_BUILD_NATIVE_FONTS=ON \
  -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON \
  -DFRONTIERX_BUILD_FFMPEG_VIDEO=ON \
  -DCMAKE_PREFIX_PATH=/external/lgpl-prefix
cmake --build --preset linux-gcc-debug
```

Replace `/external/lgpl-prefix` with your reviewed dependency prefix. Keep your
retail installation, fonts, saves, and runtime output outside the checkout.
Original-family fonts are supplied through an external `--ui-font-map` file.

Follow the [Linux playtest preparation and launch guide](docs/LINUX_PLAYTEST_CI.md)
for the external data configuration, candidate preparation, exact launcher, and
mission checklist. The [media guide](docs/NATIVE_TERRAN_MEDIA.md) explains the
font and movie requirements. Training uses `--mission-campaign u` at the native
shell entry point; the preparation guide supplies the remaining required options.

## Build the Windows baseline

The Windows build requires Windows 10 or 11, Visual Studio 2022 with the Desktop
development with C++ workload, Python 3 with the `py` launcher, and the June 2010
DirectX SDK. The build expects the SDK in `DXSDK` beside the repository.
Runtime testing also requires the DirectX 9 runtime components and retail data.

To rebuild without deploying files into a retail installation, run in PowerShell:

```powershell
.\Build-Conquest.ps1 -Configuration Debug -NoDeploy
.\Build-Conquest.ps1 -Configuration Final -NoDeploy
```

The `build-debug.cmd` and `build-final.cmd` wrappers also support the established
build-and-deploy workflow. Review deployment settings before using them. See the
[Windows restoration handoff](HANDOFF.md) and
[retail test matrix](RETAIL_TEST_MATRIX.md) for baseline details and runtime evidence.

## Report a problem or contribute

Bug reports are most useful when they include the revision, compiler/build
configuration, mission, steps to reproduce, and expected and observed behavior.
For rendering or audio problems, include the relevant hardware and driver details.
Remove personal paths and other private information from logs before sharing.
Do not attach retail assets, saves containing retail material, binaries, or memory
dumps to public reports.

Before a substantial change, check the [delivery priorities](docs/PORT_READINESS.md)
and [project architecture](docs/PROJECT.md). Small fixes to original production
owners are preferred. Preserve retail behavior, deterministic simulation, fixed
save layouts, and the Windows baseline. Contributor and coding guidance is in
[AGENTS.md](AGENTS.md) and [.github/instructions](.github/instructions).

Useful documentation entry points:

- [Build graph](docs/BUILD_GRAPH.md): native targets and validation boundaries.
- [Native training](docs/NATIVE_SINGLEPLAYER_SLICE.md): training scope and evidence.
- [Terran campaign integration](docs/NATIVE_TERRAN_CAMPAIGN.md): campaign entry, persistence, and transitions.
- [Game material path](docs/GAME_MATERIAL_PATH.md): production rendering integration and known limits.
- [Native audio](docs/NATIVE_AUDIO_STREAMER.md): speech, streaming, and decoder requirements.

## Origins, license, and game data

FrontierX builds on [Revan67's ConquestFWR restoration](https://github.com/Revan67/ConquestFWR)
and the source released by Fever Pitch Studios in December 2013. That release is
a later CQ2 development snapshot, rather than the exact 2001 retail source.
Restoring compatibility with shipped data is therefore part of the work.

This is a modified version of the Program. Revan67's restoration and FrontierX changes, with their dates and attribution, are recorded in
[MODIFICATIONS.md](MODIFICATIONS.md).

The controlling [Conquest source license](Conquest%20Source%20License.txt) permits
non-commercial use only and is revocable. Payment, donations, reimbursement, or
other consideration for the Program, distribution, warranty, or support are
prohibited. The complete license governs, including its warranty and liability
terms. [LEGAL.md](LEGAL.md) records the project's distribution procedures and does
not amend the license.

Retail executables, DLLs, maps, textures, audio, movies, fonts, manuals, and SDK
redistributables must remain outside this repository and public packages.
Dependency redistribution requires a separate provenance and license review.
