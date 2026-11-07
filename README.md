<p align="center">
  <a href="https://gamesdb.launchbox-app.com/games/images/32522-conquest-frontier-wars">
    <img src="https://images.launchbox-app.com/3ab0fd39-b49b-4a78-8a7c-d121ccb8a49b.jpg" alt="Original North American Conquest: Frontier Wars box cover" width="240">
  </a>
</p>

<h1 align="center">FrontierX</h1>

<p align="center">
  <strong>Conquest: Frontier Wars, running natively on 64-bit Linux.</strong>
</p>

<p align="center">
  <a href="#getting-started">Getting started</a> ·
  <a href="#current-status">Status</a> ·
  <a href="#build-from-source">Build</a> ·
  <a href="#contribute">Contribute</a> ·
  <a href="docs/PROJECT.md">Documentation</a>
</p>

---

FrontierX brings the original *Conquest: Frontier Wars* engine to modern Linux.
Command fleets across star systems, maintain supply lines, and play through the
Terran campaign with native graphics, sound, and input. No Wine is required.

The port preserves the game's original C and C++ engine and retail gameplay while
replacing Windows dependencies at the existing platform interfaces.

> **You need a lawfully obtained retail installation to play.** The historical
> development resources in this source tree do not replace it. FrontierX is a
> modified Program Derivative released solely under the
> [original Conquest source license](Conquest%20Source%20License.txt). The entire
> derivative and its recipients are subject to those exact terms.

## Highlights

- **Complete Terran campaign.** All 16 missions and the ending are accepted through
  user playtesting in the delivered Linux build.
- **Native Linux x86_64.** SDL3 handles windows, input, and audio; SDL GPU provides
  Vulkan rendering.
- **Original presentation.** Models, effects, HUD, both minimaps, animated portraits,
  speech, and movies are integrated for the accepted campaign.
- **Single-player menus and skirmish.** Original menus, Options, and offline AI
  matches are integrated, including Admirals and diplomacy. Full skirmish
  acceptance is still in progress.
- **Fullscreen and save/load.** Borderless fullscreen, Alt+Enter, campaign progress,
  and mission persistence use the original game flows and native platform adapters.

## Getting started

FrontierX is currently for developers and early playtesters. An optimized local
single-player candidate has been prepared; public binary distribution has not
been cleared. Start with a source build and your own external game data.

1. [Build the native targets](#build-from-source).
2. Prepare fonts, speech, and movies with the [full-media guide](docs/NATIVE_TERRAN_MEDIA.md).
3. Follow the [playtest setup and launch guide](docs/LINUX_PLAYTEST_CI.md) to configure
   external data, preserve your saves, and prepare a candidate.

Keep retail data, fonts, saves, and runtime output outside the checkout. Training
uses `--mission-campaign u`; the launch guide supplies the remaining required
arguments. Default builds leave optional fonts and FFmpeg media support off.

## Current status

Reviewed **2026-11-07**, based on recorded runtime checks and user acceptance from
**2026-10-20**. This README update adds no new gameplay validation.

| Area | Status |
|---|---|
| Training and Terran campaign | Accepted in the documented native Linux scope, including the campaign ending |
| Menus, Options, and fullscreen | Integrated; bounded X11 and Wayland runtime checks recorded |
| Offline skirmish | Live AI, research, Admiral recruitment, and save/load exercised; complete matches and broader ability coverage still need acceptance |
| Unit catalogue | 162 of 164 race-tagged archetypes admitted; two variants lack required models in the tested external data |
| Stability | Focused regression and sanitizer checks pass; whole-process graphical memory leaks remain unresolved |
| Windows | VS2022 Win32 restoration remains the regression baseline; affected rebuilds and module-ABI checks remain open |
| Multiplayer, macOS, and ARM64 | Deferred |

Campaign completion does not establish every optional ability, all hardware, or
full retail parity. Cross-platform determinism and public distribution review
remain open. See [single-player finalization](docs/RETAIL_SINGLEPLAYER_FINALIZATION.md)
for the remaining work and [unit and ability coverage](docs/NATIVE_UNIT_COVERAGE.md)
for the exact catalogue gaps.

The [publication review](docs/PUBLICATION_REVIEW.md) records the original
source-archive comparison and outstanding third-party distribution checks.

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

<details>
<summary>Enable fonts, speech, and movies for a playable candidate</summary>

A full-media build requires FreeType 2.14 or newer, external original-family font
faces, and the reviewed shared-LGPL FFmpeg configuration. Follow the
[dependency record](docs/DEPENDENCIES.md) before configuring:

```sh
cmake --preset linux-gcc-debug \
  -DFRONTIERX_BUILD_NATIVE_FONTS=ON \
  -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON \
  -DFRONTIERX_BUILD_FFMPEG_VIDEO=ON \
  -DCMAKE_PREFIX_PATH=/external/lgpl-prefix
cmake --build --preset linux-gcc-debug
```

Replace `/external/lgpl-prefix` with your reviewed dependency prefix. Supply the
external font mapping through `--ui-font-map`. The [playtest guide](docs/LINUX_PLAYTEST_CI.md)
describes candidate preparation and launch.

</details>

### Windows baseline

The supported restoration build requires Windows 10 or 11, Visual Studio 2022
with the C++ desktop workload, Python 3 with the `py` launcher, and the June 2010
DirectX SDK in `DXSDK` beside the repository.

Rebuild without deploying into a retail installation from PowerShell:

```powershell
.\Build-Conquest.ps1 -Configuration Debug -NoDeploy
.\Build-Conquest.ps1 -Configuration Final -NoDeploy
```

See the [Windows restoration handoff](HANDOFF.md) and [retail test matrix](RETAIL_TEST_MATRIX.md).
Runtime testing also requires the DirectX 9 runtime components and retail data.

## Project goals

- Preserve shipped gameplay, simulation behavior, and fixed save formats.
- Finish reliable native Linux single-player play, including optional abilities
  and complete skirmish matches.
- Keep the Windows baseline viable as platform dependencies are replaced.
- Prepare reproducible builds and packaging with external retail data.

The [delivery plan](docs/PORT_READINESS.md) sets priorities. The
[project architecture](docs/PROJECT.md) and [build graph](docs/BUILD_GRAPH.md)
explain the engine boundaries and native targets.

## Contribute

Bug reports, playtest feedback, documentation, and focused code fixes are welcome.
Use the [issue tracker](https://github.com/Michael2543/frontierX/issues) for reproducible
problems. Include your revision, build configuration, mission or map, steps to
reproduce, and expected and observed behavior. Include GPU and driver details for
rendering problems.

Remove personal paths and private information from shared logs. Keep retail
assets, saves containing retail material, binaries, and memory dumps out of public
reports.

Before a substantial change, read [AGENTS.md](AGENTS.md) and the
[scoped contributor instructions](.github/instructions). Preserve original game
behavior and add focused validation for the production owner you change.

## Origins and credits

- **Fever Pitch Studios and the original development team** for *Conquest: Frontier Wars*
  and the December 2013 source release.
- **[Revan67's ConquestFWR restoration](https://github.com/Revan67/ConquestFWR)** for the
  Windows restoration on which FrontierX builds.
- **FrontierX contributors and playtesters** for the native Linux port, compatibility
  repairs, and gameplay feedback. Dated attribution is in [MODIFICATIONS.md](MODIFICATIONS.md).

The released source is a later CQ2 development snapshot, so restoring compatibility
with the shipped game is part of this project. FrontierX is a community modification.

The original North American box cover above is referenced from the
[LaunchBox Games Database](https://gamesdb.launchbox-app.com/games/images/32522-conquest-frontier-wars).
The artwork and trademarks belong to their respective owners. The image is hosted
externally and is not included in the source tree or covered by a new project license.

## License

FrontierX is released solely under the complete, unmodified
[Conquest source license](Conquest%20Source%20License.txt), copyright Fever Pitch
Studios, December 2013. Its terms are non-commercial and revocable, with the
warranty and liability provisions stated in that license.

Payment, donations, reimbursement, and other consideration for the Program,
distribution, warranty, or support are prohibited. [LEGAL.md](LEGAL.md) documents
project procedures without changing the controlling license. Retail media and
third-party redistributables require their own distribution basis; public
packaging remains subject to the [dependency review](docs/DEPENDENCIES.md).
