# Native single-player menus and offline skirmish

## Loading an existing save from the frontend (2026-10-20)

**Actual external-data reproduction:** the user's unchanged named offline save
`Skirmish 1` reaches 90 percent, then raises SIGFPE in
FogOfWar::RevealZone at `mwidth/mapX`. GDB traces the caller through
ObjectList::ResolveAssociations, Mission::Reload and the original
MenuLoadSave::onSelection. This is an initialization-order defect, not evidence
of damaged save data. Native frontend drawing also requires `b3DEnabled`, so the
Windows-era load-dialog check incorrectly skips ChangeInterfaceRes and its
CQE_ENTERING_INGAMEMODE event. The fog grid dimensions are still zero.

Native MenuLoadSave now checks GetCurrentInterfaceRes before entering gameplay.
It sends the existing transition before Mission::Load, preserving the original
fog owner, map dimensions, saved bytes, simulation order and Windows branch.
No zero-divisor fallback or renderer validation relaxation is introduced.

A subsequent actual GUI interval also exposed an independent signed-pane
contract issue. A core dump shows Static::draw centering the resource text
`37/45` in pane `(415,-4)..(493,17)` at a 1261x1390 viewport. The font adapter
incorrectly rejected that valid offscreen origin. NativeFont now accepts bounded
signed pane origins, preserves authored wrapping and glyph placement, and clips
the resulting spans to the viewport. Inverted panes and out-of-range coordinates
remain fatal. The existing CheckNativeFontTextLayout regression compares the
exact clipped `37/45` spans against a translated reference and verifies that
compensating offsets preserve the original placement. The original Windows font
path is unchanged.

**Regression method:** extend the existing exclusive graphical menu diagnostic
with `--menu-check load-save --save-check-file NAME.mission --smoke-ms 2500`
and the normal full-media startup arguments. Use an isolated copy of genuine
saves and configuration. The helper renders the original Load dialog, locates
the filename in its actual list, selects it, renders the selection and invokes
the original Open handler. The normal source mission loop then runs, performs
QuickSaveFile/QuickLoadFile and requires unchanged update count, AI count and
single-player classification before normal close. Only diagnostic copies are
written. Existing new-match F7/F8 checks cannot detect missing frontend entry,
because those owners have already received the gameplay transition.

**Validation:** Linux x86_64 full-media Clang 22.1.8 Debug and
RelWithDebInfo, GCC 16.2.1 Debug and fatal Clang ASan/UBSan compile the affected
native owners. In each configuration the focused CTests
`portability.native_shell`, `portability.layout`,
`portability.render_batcher_runtime`, `portability.game_material_manager`,
`portability.game_material_manager_cr` and
`portability.game_material_manager_missing_end` pass. The font-enabled shell
check executes the signed-pane glyph regression with an external font face;
its remaining inputs are synthetic/repository-owned.

Actual external-data GUI execution loads the unchanged `Skirmish 1` through
the original menu and runs its simulation and subsequent F7/F8 owners, preserving
the update count and one AI player. The copied candidate executable and runtime
resources repeat this under Wayland, then close normally with zero mission audio
streams and idle hint ownership. The last genuine Terran mission-16 quicksave
also loads and completes its quicksave/reload under X11. These are bounded
save-recovery checks, not full-match acceptance. Private resized X11 intervals
complete, but the window manager overrides their requested narrow dimensions;
only the failed core and exact glyph regression establish the 1261x1390 case.

The final fatal ASan/UBSan X11 interval also loads this genuine named save,
runs frames, preserves update count 42005 and one AI through quicksave/reload,
and completes normal mission close without an invalid-access or UB report.
Whole-process LeakSanitizer still fails at exit with 64,381 bytes / 924
allocations in host/unloaded-library stacks, including SDL X11 setup. No
suppression is applied and this is not reported as a clean whole-process pass.
The preceding load-only sanitizer interval reported 64,659 bytes / 924
allocations; neither result closes the existing graphical leak gate.

All nine genuine saves/progress files and the complete user configuration are
copied byte-for-byte into the separate corrected local candidate. Diagnostic
quicksaves never replace those originals. Native Windows Debug/Final and relevant
ABI execution remain unavailable; no persisted format or shared type changes.
Final user gameplay acceptance remains pending.

## Skirmish admirals, diplomacy and chat (2026-10-20)

**Source review and concrete regressions:** ResearchButton still carried the
2026-10-12 native Admiral exclusion, although the original Flagship, Fleet and
retail Admiral data owners have since been integrated. ScrollingText's native
hotkey branch explicitly rejected diplomacy and both chat menus. Restore those
original owners, rather than replacing their mechanics or adding multiplayer.
Campaign restrictions on diplomacy/player filtering remain the original
`MISSION_DEFINED` restrictions; quick chat retains its original campaign path.

The native link now includes Menu_Diplomacy, Menu_PlayerChat, Menu_Chat and
DiplomacyButton. Original local gift/alliance packet handling and attributed
chat text execute through ScrollingText. SDL legacy input records and bounded
Windows-wide-string formatting use the existing native adapters. The obsolete
Admiral availability latch and its click/context-hotkey exclusions are removed;
normal research prerequisites, availability, costs, timing and queues remain.

**Actual external-data reproduction:** opening diplomacy initially crashed in
DiplomacyButton::draw. Its raw retail record stores `shapeFile` at byte 8; GCC
and Clang packed the two signed eight-bit fields before the byte array at byte
6. The native record now represents the reserved bytes explicitly and asserts
byte 8 / total 40. All nine original VFX images load through the native image
owner. Button dimensions and handshake offsets follow current display scaling.
The Windows and schema-generator bitfield declarations remain intact.

Quick chat exposed Edit2's unconditional access to the absent Windows debug
hotkey service. Native editing continues to suspend/resume the actual gameplay
hotkeys and excludes that Windows service. Closing native quick chat explicitly
releases its focused edit before deleting controls, restoring the gameplay
hotkeys and EventSystem-owned text focus. Native social hotkeys also honor the
active original modal frame. The first real Admiral reload exposed this focus
lifetime defect; it is not treated as a completed runtime interval.
At 2560x1440, GDB also reproduced
an empty focused edit highlight with x0=32 and x1=23: the right edge omitted
the text origin. Native drawing now adds that origin, as the populated selection
already does. Strict native rectangle validation is unchanged.

**Regression method:** extend the existing isolated `skirmish-builder` GUI
check, with `--skirmish-check-race terran|mantis|solarian --smoke-ms 2500` and
normal full-media arguments. Its private fixture opens the original dialogs
through ScrollingText hotkeys; checks alliance Apply/restore, gift amount/Reset,
recipient filtering, attributed local chat insertion, quick chat and closure;
and repeats after a normal full mission quicksave/reload. It prepares the stock
Academy, Warlord Training or Citadel and prerequisite/resources only inside this
exclusive diagnostic. Recruitment activates an enabled original research control
through CMenu_ResBuild, runs the unchanged stock build time and requires a live
Admiral, its populated original Fleet panel and its identity after reload.
Ordinary campaign/skirmish never calls these fixture helpers. Existing CPU
checks cannot establish real dialog rendering or recruitment completion, so
this is an explicit host-dependent extension, not a substitute simulation.

**Compilation and automated CPU execution:** Linux x86_64, Clang 22.1.8 Debug,
Clang RelWithDebInfo, GCC 16.2.1 Debug and fatal Clang ASan/UBSan full-media builds
compile the original owners. In each build, the following six focused CTests
execute and pass (synthetic/repository-owned inputs):

```sh
ctest --test-dir "$BUILD" --output-on-failure -R '^portability\.(native_shell|layout|render_batcher_runtime|game_material_manager(_cr|_missing_end)?)$'
```

The sanitizer invocation uses `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`
and `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`.

**Actual local-match execution (lawfully external retail data):** optimized
Clang/X11 Mantis completes the full interval: the enabled original Mordella
control at Warlord Training, stock recruitment, populated Fleet panel, normal
F7/F8 with one live AI and update count 3767, both social-menu cycles, restored
builder panel and normal close with zero mission audio streams / idle hints.
Private captures of diplomacy, player chat, quick chat and the recruited Admiral
were inspected; the separate 3840x2160 interval also rendered the chat edit
without the reproduced inverted rectangle. Optimized Clang/Wayland Celareon
also completes: original Blanus research control at Citadel, stock recruitment,
Fleet panel, F7/F8 at update 3769 with one AI, repeated social controls and
normal close (3793 rendered modal frames, audio streams zero, hints idle).
Fatal ASan/UBSan Clang/X11 also completes original Mordella recruitment,
Fleet panel, F7/F8 (one AI, update 3769), both social-menu cycles and normal
source close with zero audio streams / idle hints. There is no preceding
invalid-access or UB report. Whole-process LeakSanitizer still exits 1 with
81,085 bytes in 933 allocations, the same total as the earlier builder run;
host/X11 and unloaded-library allocations remain unresolved, without suppression.
This is not a clean whole-process sanitizer pass.

The separate delivered Clang RelWithDebInfo artifact executes the complete
Terran interval on Wayland using its copied runtime resources and private copies
of the genuine configuration/saves: original enabled Halsey control at Academy,
stock recruitment, Fleet panel, F7/F8 at update 3767 with one AI, both complete
social-menu cycles, restored builder panel and normal source close (audio zero,
hints idle). Its argument list contains no checkout paths. This completes the
bounded three-race owner regression; it is not a full match or user acceptance.
All nine genuine save/progress files and the original display configuration
are copied byte-for-byte from the preceding candidate. Test-only windowed
preferences and recruitment fixtures remain in separate private directories.
The package retains the intact original license, modification/dependency
notices, source snapshot/diff and binary/resource/configuration fingerprints.
The player's start command contains no diagnostic arguments. The same packaged
binary also completes the stock TM1 `campaign-builder` interval on Wayland:
62 modal frames, builder panel across display changes, normal save/reload
preserving update 55 / zero AI, and normal close with audio zero / hints idle. The deliberately stopped
invalid HQ diagnostic fixture is not Admiral gameplay evidence. Native Windows Debug/Final,
affected ABI/layout execution and comparative retail/Windows behavior remain
unperformed. Multiplayer transport remains excluded. Full user skirmish matches
and the existing whole-process graphics leak gate remain separate.


## Builder selection at widescreen resolutions (2026-10-20)

**Reported retail-data runtime:** selecting Fabricator, Weaver or Forger freezes
and exits in skirmish and campaign. The local candidate log terminates with
`native App 2D rectangle bounds invalid` immediately after ordinary selection.
There is no recorded game core or host OOM event for this report.

**Reproduced external-data runtime:** GDB stops in the real native rectangle
owner before its fatal diagnostic at 2560x1440. `BuildButton::draw` submits a
green active-technology marker with y0=1248, y1=1244. Its vertical inset uses the
button width divided by three for both axes. Independent horizontal/vertical
HUD scaling makes those icons non-square in pixels and inverts the vertical
line. The shared builder construction panel explains why both campaign and
skirmish are affected. This is an aspect-ratio geometry error, not evidence of
an SDL fullscreen or GPU driver crash.

Native BuildButton now computes vertical marker insets from button height,
retaining horizontal insets from width. The square-icon result and original
Windows calculation are unchanged. No save/module layout, gameplay, build cost,
research state, RNG consumption or renderer assertion changes.

The existing exclusive menu diagnostic gains `--menu-check skirmish-builder`
with the existing `--skirmish-check-race terran|mantis|solarian`. It uses normal
starting ships and `ObjectList` selection/highlight ownership, waits for the
original deferred construction panel to populate, renders that selection across
a display change, runs live AI F7/F8, reselects the builder and renders again.
It requires the real panel to own the selected mission ID and contain build
controls before declaring either interval complete. No diagnostic input reaches
another window, and normal sessions perform none of these diagnostic actions.
This closes a specific gap in the earlier smoke, which never kept a builder
selected through the construction-panel update. The original skirmish/Portal
check also gains these selection intervals. `--menu-check campaign-builder`
performs the equivalent bounded check on the supplied stock campaign mission,
retaining campaign state and its AI count across save/load. It initializes the
original in-game interface transition before direct loading, matching the
briefing Start handler. The initial direct diagnostic omitted that transition
and divided by an uninitialized fog-map extent before selection; the diagnostic
setup was corrected without adding a fog guard or changing ordinary campaign
entry.

**Executed repair validation:** Linux x86_64, GCC 16.2.1 Debug and Clang 22.1.8
Debug, full-media RelWithDebInfo and ASan/UBSan compile. The existing native
shell, layout, render-batcher runtime and material-manager checks execute
successfully in GCC/Clang and focused fatal sanitizers. Exact selection:

```text
^portability\.(native_shell|layout|render_batcher_runtime|game_material_manager(_cr|_missing_end)?)$
```

Actual lawfully external-data optimized execution selects Fabricator and Weaver
through native Wayland at 2560x1440, and Forger through X11 at 3840x2160. Each
retains a populated construction panel across display changes, quicksave/load,
reselection and further rendering, then closes normally with zero mission audio
streams and idle hint ownership. A stock TM1 campaign interval performs the same
sequence, preserving update count 55, single-player state and zero AI owners.
Each bounded interval presents 62 source frames. These intervals reproduce the
reported panel path; they do not imply full-match or full-campaign acceptance of
the new candidate. The real Mantis interval also completes both panel render
checks, fullscreen/window restoration, AI save/load and regular close under
fatal ASan/UBSan without an invalid-access or UB report. Its process then still
fails the existing graphical LSan gate: 81,085 bytes/933 allocations, with the
same host/unloaded-library attribution limits. No suppression or weakened
rectangle assertion is used. Native Windows Debug/Final and affected ABI checks
remain unexecuted. The user should repeat the targeted selections on the fixed
candidate; no new full campaign playthrough is required by this correction.

GDB MCP could not initialize the desktop because its server lacks the runtime
display environment. The reproduction therefore uses ordinary GDB with the
existing isolated graphical launcher environment. Owned debugger sessions are
closed; no user process or save is modified by diagnosis.

## Implementation status (2026-10-20)

The native full-media shell now enters the original main menu for the Terran
campaign entry. Main, Single Player, Campaign, Load, Options, Help, Credits,
and offline Skirmish use their production owners. Training retains its accepted
entry path. Online multiplayer and the Windows editor/menu bar remain excluded.

The user has accepted the complete Terran campaign and its ending. That report
predates these menu changes. Full user acceptance of the new menus and complete
skirmish matches remains separate from the automated integration evidence below.

## Production owners and repairs

| Owner | Native behavior |
|---|---|
| `Menu1`, `Menu_SPGame`, `Menu_help`, `Menu_Credits` | Original front-end navigation, introduction caller, profile entry, credits, and mission return |
| `Menu_igoptions`, `Menu_options`, `Menu_newplayer` | In-game Options access, original tabs and controls, preview, cancellation, persistence, and player management |
| `DrawAgent` | Cached images retain authored block coordinates and derive their current screen coordinates for each draw |
| `NativeShell`, `NativeAppFont` | Window size events update the interface viewport and active layout; cached fonts refresh their pixel height |
| `NativeWindowManager`, `NativeRenderPipeline`, `SdlGpuRenderer` | Native display modes and gamma at the existing window/render boundaries |
| `NativePlayerProfiles`, `NativeWritableUtfArchive`, `NativeUserDefaults` | Separate player directories, bounded directory operations, and settings migration during rename |
| `Menu_mshell`, `Menu_final`, `Menu_map`, `Menu_slots`, `Menu_MapSelect` | Original local setup, supplied/generated maps, slots, three retail races, colors, teams, difficulty, and game settings |
| `Menu_netloading`, `MapGen`, `PlayerBomb`, `Mission`, `MScript` | Local map loading/generation, real starting fleets, original AI initialization, and strict script-free save/load |

The distorted menu had two independent coordinate/lifecycle failures. Image
archetypes cached coordinates converted for an earlier interface resolution.
Later draws reused those coordinates while the controls used the current
resolution. Window resize events also left the native interface viewport at its
old size, which violated the renderer's full-target clear contract. The repair
updates the owning coordinates rather than adjusting individual menu buttons.
An actual 801-by-600 to 1280-by-720 resize then exposed a third stale anchor:
the toolbar scaled its negative top-bar control offsets while retaining its old
bottom-HUD origin. GDB stopped at the font bounds assertion with the command
points pane above the screen. `Menu_tb::setStateInfo` now recomputes that origin,
top-bar placement and toolbar-dependent status heights before reflowing controls.
The font assertion remains strict. Repeated toolbar initialization also exposed
a native HotButton label leak: `InitHotButton` overwrote its owned duplicate
without releasing the previous one. Reflow now frees that label before reuse. The camera pane is also refreshed through
its original full-world toolbar contract.

An additional actual resize interval stopped at a captured viewport larger than
the newly shrunken swapchain. The renderer records its frame extent and retires
a captured frame that fit its original extent but no longer fits a changed
target. Invalid original-frame extents and unchanged-target viewport
validation remains strict; simulation timing and orders are untouched. The
existing GPU lifecycle probe now captures before a shrink, checks stale-frame
retirement, restores the window, and resumes its original readback/presentation
checks. It also requires an oversized viewport on an unchanged target to fail
with the original explicit error. This extends the existing production frame contract rather than adding
a separate renderer or probe.

The original Windows `Menu_nl` destructor calls `CancelProgressAnim`. The first
native local loader omitted that ownership transition. Its netload flag then
kept the next F8 progress animation alive during ordinary rendering. The native
loader now performs the original cleanup on success, rejection, and exceptions.
The existing assertion remains intact.

`MapGen::GetRand` consumes 15-bit fixed fractions. Host libc `rand` supplied
31-bit values and tripped its range assertion. Native map generation now uses
a local Windows CRT recurrence for its own random stream. Gameplay `RandomGen`
and simulation timing remain unchanged. Generated-map equivalence with native
Windows has not been measured.

`MScript` admits an absent script only for a genuine local, non-campaign game
with no script state. Campaign checks and rejection of malformed script records
remain strict. F7/F8 retains the original separate `m0000000.mission` slot;
Terran campaign quicksaves continue to use `f0000000.mission`.

Generated planet footprints visit negative quarter-grid coordinates. Native
`GRIDVECTOR::init` now truncates through a signed integer before the original
byte wrap, avoiding an undefined float-to-byte conversion without clamping the
footprint. The existing math-core check protects the wrap and truncation.
The real skirmish sanitizer run also found the AI alliance lookup shifting by
minus one for neutral objects. It now checks the existing player-ID domain
before indexing the alliance mask; no saved type or AI ordering changes.
A real F8 run also identified the starting-fleet `PlayerBombFactory` allocating
its archetype with `calloc` and deleting it through the default C++ deallocator.
Its native class now pairs that allocation with `free`.

The main-menu and About copyright resources now contains an explicit copyright character,
replacing damaged encoding. Native font coverage includes that glyph. Credits
retain authored positions across resize, consume empty line terminators, and
check line capacity. Editable native names retain the current ASCII contract;
unsupported input is filtered before rendering. A single-character ignore-list
lookup no longer treats a scalar character as a terminated string.

## Options and profile boundaries

Audio levels and mute settings update the original `USER_DEFAULTS::soundState`
contract through the native sound owner. Gamma previews use the render boundary.
Cancel restores live audio, gamma, and cursor preferences. OK persists the
original gameplay/interface settings and effects options. Display dimensions
are read again on process startup.

Native gamma operates in the fragment shader before blending. It is functional
brightness control, not a measured equivalent of the Windows display gamma
ramp. The shader defaults to neutral gamma and preserves alpha.

The native device dropdown now selects Windowed or Fullscreen; the adjacent
renderer label identifies Vulkan. Fullscreen uses the desktop display mode.
Alt+Enter changes mode once per press, including in a mission. OK persists the
mode and restored window size; Cancel leaves display mode unchanged. Layout
refresh preserves staged controls and the selected tab. The SDL window owner
synchronizes completed size changes before publishing geometry for the next
source frame. See [finalization evidence](RETAIL_SINGLEPLAYER_FINALIZATION.md).
Obsolete
DirectDraw/software-device and DirectInput cursor controls are disabled.
Native pointer speed follows the system cursor. Voice chat and multiplayer
remain excluded rather than being represented by active nonfunctional controls.
The CQ2-only Vyrium race is not offered by native retail skirmish.

The first profile aliases the existing flat save directory. Existing campaign
files and `player.gpf` are not moved. Additional profiles use `Players/<name>`.
Rename retains the profile's files and settings. Delete applies to additional
profiles. The imported flat profile remains the migration anchor and cannot be
recursively deleted through the UI.

No persistent game schema, legacy interface vtable, or simulation constant was
widened for this integration. Native Windows Debug/Final and affected module ABI
checks remain unexecuted on this host.

## Validation record

Inputs are either repository-owned synthetic fixtures or the user's lawfully
external installation and fonts. Raw captures, logs, saves, and compiler outputs
remain outside the repository.

- **Compilation:** full-media Debug configurations on Linux x86_64 with
  Clang 22.1.8 and GCC 16.2.1. Optional native fonts, FFmpeg audio, and FFmpeg video
  are enabled. Existing dependency provenance applies; no dependency was added.
- **Automated synthetic execution:** existing `portability.data_root` exercises
  physical directory create/open/rename, rejected traversal/overwrite, profile
  selection, save isolation, settings preservation, and deletion. Existing
  `portability.native_user_defaults` checks settings migration on rename.
- **Automated native lifecycle execution:** existing `portability.native_shell`,
  layout, window-manager, material, and batcher checks cover the affected owners.
  The image check changes both real resolution and the 800/640 interface space,
  including fractional screen coordinates and high-resolution images.
- **External-data runtime:** original main, options, single-player, skirmish,
  and credits dialogs execute bounded source modal loops. The Options diagnostic
  uses the real player-name edit control and original handlers to check typed
  input, preview/Cancel, OK, settings reload, and restoration of the initial
  values. Credits also parse the complete installed text through EOF.
- **External-data gameplay runtime:** fixed-seed generated matches use each of
  the three retail races, rotating the opponent so every race's real AI owner
  executes. A supplied map also starts through the production loader. Live F7/F8
  preserves the simulation update count and the active AI owner, then renders
  again before ordinary quit. These are bounded matches, not victory/defeat
  acceptance.
- **Actual SDL/Vulkan execution:** existing `portability.game_material_gpu` and
  `portability.sdl3_gpu` execute with required GPU availability. Readback checks
  gamma change, invalid-value rejection, exact reset, and the retained material,
  frame, texture, and resize regressions.

Visible external-data captures confirm the original main and About dialogs
and the generated-match pause panel at 801 by 600. The pause panel aligns its
frame, labels, buttons, and original HUD. A separate actual 30-second pause
interval resizes to 1280 by 720 and back to 800 by 600, presents 708 source frames,
and visually confirms both resized panels and HUDs. The exact bounded frame
count is timing-dependent; it is not a performance target.

The CPU selection passes on GCC and Clang. Its instrumented Clang ASan/UBSan
execution passes with
`ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. Exact existing CTest selection:

```text
^portability\.(math_core|data_root|native_user_defaults|native_window_manager|native_shell|layout|render_batcher_runtime|game_material_manager(_cr|_missing_end)?)$
```

The graphical Options transaction run completed its assertions but failed
LeakSanitizer at process exit: 64,381 bytes in 924 allocations. Symbolized
direct stacks include SDL X11/XRandR/keyboard setup, D-Bus, and PipeWire; some
unloaded-module stacks remain unattributed. This is a failing whole-process
leak gate, not a clean sanitizer result. No suppression or disabled leak check
is used to convert it into a pass. The corrected fatal Mantis skirmish interval
also completes live save/reload at update 298 with one AI and normal close, then
fails LSan with 64,941 bytes in 931 allocations. No preceding access, allocator
mismatch, or UBSan failure remains in that bounded interval. Focused CPU checks
are separate evidence. The final fatal 30-second resize interval presents
643 pause frames without preceding access/UB failures. Its LSan result remains
a failure at 81,121 bytes in 933 allocations; the earlier `DuplicateWide` /
`HotButton::InitHotButton` leak stacks are absent after the label ownership
repair. The unresolved stacks prevent a whole-process leak-freedom claim.

A skipped GPU run is not runtime evidence. Whole-process long-session leak
freedom, all hardware/DPI combinations, native Windows ABI, full comparative
determinism, and complete user matches remain open gates.

## Reproduce the bounded diagnostics

Use the same full-media inputs as the existing
[playtest command](LINUX_PLAYTEST_CI.md), a private save/config copy, and
`--mission-start --mission-engine --mission-gpu ... --smoke-ms 10000`.
Replace `--mission-campaign` with one of these explicit diagnostics:

```text
--menu-check main
--menu-check options
--menu-check options-transactions
--menu-check single-player
--menu-check skirmish
--menu-check help
--menu-check credits
--menu-check pause --skip-movies
--menu-check skirmish-start --skirmish-check-race terran
--menu-check skirmish-start --skirmish-check-race mantis
--menu-check skirmish-start --skirmish-check-race solarian
--menu-check skirmish-start --skirmish-check-map mp_Eric1v01.qmission
```

The pause geometry diagnostic opens a generated offline match and explicitly
omits movies. It does not establish movie or campaign-mission acceptance. Diagnostic windows exclude external
input and retain normal source timing. No M3 observer, synthetic gameplay order,
objective override, or progress unlock is used. The interactive candidate uses
none of these diagnostic flags.

## Remaining acceptance and retail work

The handoff checklist covers nested pause/Load/Options/Back, Apply and Cancel,
restart persistence, profile selection, and a complete offline match. Test
victory, defeat, return, multi-AI teams, and normal named saves separately.
No repeat of the entire accepted campaign is required for these checks.

The [refreshed unit and feature audit](NATIVE_UNIT_COVERAGE.md#refreshed-single-player-audit-2026-10-20)
records actual 162/164 factory admission, two missing variant assets, unresolved
source groups, and the remaining wormhole persistence blocker. Retail is not
complete. After this handoff, exercise optional abilities and all race tech
trees, complete the remaining
single-player acceptance, and address the carried lifecycle/compatibility gates.
Packaging remains subject to provenance, license, and retail-data separation.
Multiplayer is a later workstream under the current delivery priorities.
