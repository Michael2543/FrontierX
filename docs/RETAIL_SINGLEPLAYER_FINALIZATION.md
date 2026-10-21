# Single-player retail build finalization

## Authorized scope (2026-10-20)

The follow-up named-save loading regression and its owner repair are recorded in
[frontend save loading](NATIVE_SINGLEPLAYER_MENUS.md#loading-an-existing-save-from-the-frontend-2026-10-20).

The subsequent user-requested Admiral/diplomacy/chat integration is tracked in
[the focused owner and runtime record](NATIVE_SINGLEPLAYER_MENUS.md#skirmish-admirals-diplomacy-and-chat-2026-10-20).
Its bounded checks do not close full-match or multiplayer acceptance.

Follow-up user testing exposes a widescreen construction-panel abort when
selecting builders in both campaign and skirmish. The
[builder-selection repair and actual runtime evidence](NATIVE_SINGLEPLAYER_MENUS.md#builder-selection-at-widescreen-resolutions-2026-10-20)
supersede the previous candidate for this path. Full-match acceptance remains open.

The user requests a finalized Linux single-player retail build including
fullscreen, excluding multiplayer. Preserve the accepted complete Terran
campaign, original gameplay, genuine saves, external media and Windows baseline.
The preceding menu/skirmish candidate is integration evidence, not final retail
acceptance. Existing Windows/ABI, determinism and host leak gates are not waived.

## Delivery checklist

| Item | Current evidence / remaining work |
|---|---|
| Borderless desktop fullscreen | Original Options and Alt+Enter execute in the optimized build; Apply/Cancel, repeat filtering, window-size restoration and preference writes pass; isolated persisted-mode startup renders inspected 3840x2160 frames |
| Options during resize | Preserve staged slider/push values, audio enable state, display selection and active tab across layout refresh; transaction test extended |
| Completed display transitions | Synchronize the SDL window operation and publish its geometry before the next source frame clear; actual modal and live-mission fullscreen/windowed transitions pass in Clang Debug and optimized execution |
| Wormhole persistence | Original 112-byte schema retained; versioned native sidecar preserves fractional launcher state and both active portal strips/placement; real Portal order and full mission quicksave/reload preserve both portal visuals byte-for-byte |
| Unit/ability audit | Keep the 162/164 catalogue failure and unresolved optional groups visible until reachability/provenance is resolved; never substitute missing models or drop failed records |
| Native lifetime faults | Recheck focused fatal sanitizers and real UI/gameplay intervals; retain whole-process leak failures until attributed and repaired |
| Optimized artifact | Prepare a separate full-media Clang RelWithDebInfo configuration, then execute the real menu, campaign checkpoint and local AI save/load paths |
| Packaging | Preserve complete controlling license, modification/dependency notices and exact source/configuration hashes; reference external retail data and preserve user saves |
| User acceptance | Complete original-menu navigation and full local matches, including victory/defeat and teams; pending tests are not passes |

No multiplayer implementation, public publication, commit or push is included.
Raw external data, saves, binary analysis, captures and diagnostics remain outside
the repository. Final build readiness must state the actual completed evidence
and any remaining release blocker.

## Production contracts and regression checks

**Source review:** the original Wormhole launcher native Save rejected every
attempt and Load returned false. A Portal is part of the shipped single-player
feature set, so this was a selected integration blocker. GunPlat delegates its
launcher persistence through the normal mission object save/load path. Preserve
the 112-byte `WORMHOLE_LAUNCHER_SAVELOAD`; add a separate 1304-byte version-1
sidecar for fractional damage/time and two 644-byte portal visual records.
No runtime pointer, OBJMAP link or texture handle enters either record. Restore
the original portal placement/strips without invoking a random-consuming Update.
Reject unsupported versions, wrong sizes and invalid state before mutation.
Legacy empty launcher saves contain no recoverable active operation and remain
unsupported; this is not a claim of new Windows-retail save compatibility.

The existing exclusive `--menu-check skirmish-start` is extended, not replaced
by another probe. Its isolated factory/storage stage compares the original
schema and sidecar exactly and rejects a truncated/unknown-version sidecar.
Its live stage creates the stock Portal through original `MScript::CreatePart`,
sends `OrderDoWormhole` through original local packet/operation owners, and runs
full `QuickSaveFile`/`QuickLoadFile` while both portals exist. It compares both
visual states independent of object-list insertion order. Later it repeats the
existing AI/update-count save/load check and closes normally. Normal sessions
never spawn diagnostic objects. This proves the integrated persistence path;
it does not replace research/construction/combat acceptance for every ability.

The existing native window/layout checks cover mode restoration and fixed-width
sidecar validation. Original Options transaction checks now cover staged values
and selected tab across reflow, Cancel, actual display Apply and persisted mode,
then Alt+Enter down/repeat/up/return. Real in-mission frames render between mode
changes. These checks address the observed stale geometry on the first frame
after a mode change; compilation alone could not cover it.

The fullscreen startup check then exposed SDL's short synchronization timeout.
The window boundary retries confirmation within a two-second deadline and still
fails explicitly if completion is not confirmed. Geometry publication happens
only after confirmation. No failed operation is treated as success. Sequential
isolated startup at the saved fullscreen preference renders 3840x2160 Options.
Visual inspection also exposed Button2's cached archetype dimensions: text and
hit rectangles retained the creation-time size while shape draws re-scaled.
Native Button2 now queries the live shape dimensions, preserves authored margin
units, tests zero-extent sentinels before conversion, and computes text/hit panes
in current pixels. The existing Options diagnostic compares real draw-agent
dimensions, hit rectangles and font extents in both modes. Actual 4K captures
show complete centered OK/Cancel text. Windows drawing remains unchanged.

A later fatal UBSan run of that same real Portal/fullscreen path reaches
`Platform::TestHighlight` -> `RenderMgr::get_instance_projected_bounding_polygon`
-> `ComputeConvexHull` and reports S32 determinant overflow for off-screen
projected coordinates (29345 multiplied by 437592). Native hull orientation and
collinear distance ordering now use exact wide integer intermediates for the
full S32 input domain. This is render selection geometry, not simulation or a
save/ABI change; the Windows implementation remains unchanged. The existing
material-manager check now submits normal, large projected and full-range
coordinate rectangles through the production hull and requires exactly the
four corners with unchanged input, including collinear points. Earlier fixture
coordinates were too small to expose this failure. The fatal sanitizer run of
that regression passes; the live rerun is recorded separately.

The next live run exposes the adjacent `RectIntersects` real-to-ideal conversion
overflow for projected x=-4734111. Native BaseObject now widens multiplication
before division, retaining the existing integer rounding and raster algorithm.
The existing native shell lifecycle check exercises the real intersection
owner at 3840x2160 with that coordinate, checks its converted value, an actual
intersection and a miss, then restores all screen globals. Its fatal sanitizer
check passes. No global coordinate macro or simulation conversion is changed.

## Executed evidence (2026-10-20)

Linux x86_64, Clang 22.1.8 and GCC 16.2.1, SDL 3.4.16, existing strict font map
and reviewed external shared-LGPL FFmpeg 9.0.1 configuration. Full-media GCC and
Clang Debug builds and the Clang ASan/UBSan graph compile. Focused automated
checks execute with:

```sh
ctest --test-dir "$BUILD" --output-on-failure \
  -R '^portability\.(math_core|data_root|native_user_defaults|native_window_manager|native_shell|layout|render_batcher_runtime|game_material_manager(_cr|_missing_end)?)$'
```

The focused fatal sanitizer run uses `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`
and `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1` and passes. Inputs are
repository/synthetic fixtures except the configured external font. The full
graphical skirmish storage/AI interval reports no invalid access, allocator-pair
or UB error before teardown, but **fails LSan** with 69,929 bytes/927 allocations
in host libraries and unresolved unloaded callers. A subsequent live Portal run
also exposed a display synchronization timeout under instrumentation; its
full-process check is not a pass. No suppression or disabled sanitizer is used.

Actual lawfully external-data Clang Debug execution passes the live Portal
mission reload, fullscreen/window restoration, then a normal AI reload retaining
update count 36 and one AI. The optimized run repeats those paths, retaining
update count 62 and one AI. Original Options Apply/Cancel/reflow and Alt+Enter
repeat checks execute in the optimized build. Normal menus are visually
inspected after compositor-controlled resize; this is not an exact 800x600
pixel comparison when the tiling compositor overrides the requested size.

`Tools/portability/audit_retail_singleplayer.py` executes read-only against the
four external archives, including 41 nested maps and map path components, and
the source tree. Both named variant models are absent; neither has a named
external reference. This does not prove numeric/script nonuse. The catalogue
remains 162/164 with failed completeness. GeneralPlat/Repulsor/Swapper/
KamikazeWing and earlier ship-launch/torpedo questions remain open source-scope
questions; none is silently declared complete or irrelevant.

**Final sequential runtime after repairs:** Clang full-media ASan/UBSan executes
the stock Portal order, exact full-mission portal restoration, frames across
fullscreen/window restoration, later AI save/load retaining update count 36 and
one AI, and regular close with mission audio streams zero and idle hint owners.
There is no invalid-access, allocation-pair or UBSan report before teardown.
The process still **fails LSan**, 81,085 bytes/933 allocations, including known
X11/DBus/PipeWire paths and unresolved unloaded-library callers. This remains an
open gate, not a clean sanitizer pass. Focused fatal checks still pass.
The final optimized Options transaction also executes through native Wayland:
1261x688 window -> 2560x1440 fullscreen -> restored window, including Cancel,
staged reflow, button geometry, persisted settings and Alt+Enter repeat handling.
The optimized native Wayland live match also executes the stock Portal order,
exact mission restoration, fullscreen/window restoration and later AI reload
(update count 37, one AI), then exits normally with zero mission audio streams
and idle hints. These are bounded generated-map checks, not complete matches.

## Local handoff and remaining gates

The local `retail-singleplayer-20261020` candidate uses the separate optimized
build and copies genuine saves/progress/configuration without modifying its
predecessor. It includes the intact controlling license, modification and
dependency notices, exact source patch/files, configuration and binary/shader
hashes. Repository-owned material/startup/hotkey resources and the existing
checked launcher are copied into its own runtime resource root. Launch does
not depend on a mutable working checkout. Retail archives, fonts and reviewed
shared decoder libraries remain external; no proprietary media or manual is
packaged. Generated diagnostic runtime directories are not copied as saves.

This is a local single-player release candidate. Public dependency provenance
and package approval remain blocked as recorded in `DEPENDENCIES.md`. Native
Windows Debug/Final and affected module ABI checks, comparative determinism,
whole-process leak attribution, the unresolved catalogue/optional groups and
full user skirmish/research/ability/teams/victory/defeat acceptance remain open.
Multiplayer is excluded, not implemented. The accepted Terran campaign is
preserved; a final fullscreen/reload/menu check on this optimized candidate is
still requested from the user. None of those pending checks is recorded as a pass.
