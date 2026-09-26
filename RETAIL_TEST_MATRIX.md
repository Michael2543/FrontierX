# Retail-Parity Test Matrix

This is the acceptance ledger for Priority 1: restore behavior before modernization. Compare the
Debug and Final builds with the retail reference and record evidence, not impressions.

## Automated baseline

| Check | Debug | Final |
|---|---:|---:|
| Full `Win32` solution rebuild | PASS (2026-08-30) | PASS (2026-08-30) |
| Required EXE/DLL outputs present | PASS | PASS |
| All 18 retail campaign DLLs rebuild | PASS | PASS |
| Deployed binaries match build SHA-256 | PASS | NOT DEPLOYED IN LATEST CHECK |
| Isolated deployment directory | PASS | PASS |
| Original retail campaign imports resolve against fresh modules | PASS (2026-09-25; 153 `Mission`, 2 `Trim`) | PASS (2026-09-25; same) |
| Retail/build `Mission.dll` export-set difference (descriptive) | MEASURED (4 retail-only; 28 build-only) | MEASURED (same counts) |

Warnings are not a passing fidelity signal. In particular, the linker still reports `/FORCE`,
runtime-library conflicts, and shared intermediate directories.
The 2026-09-25 import checks used all 18 original campaign DLLs from a lawfully installed copy
and fresh Debug/Final builds. They did not launch the game or establish runtime parity; see
[WINDOWS_BUILD_VALIDATION.md](docs/WINDOWS_BUILD_VALIDATION.md).
The retail binary and restored CQ2-derived source are different versions; equal export sets are
not an acceptance criterion. The import check measures whether the original campaign DLLs can
resolve the symbols they actually request.
On a physical Windows 11 host with an NVIDIA GPU, the restored Final build reached a visible
main menu and Quick Battle setup through its built D3D9 renderer and a separate test directory
pointing at lawfully installed Steam data. The early Quick Battle `Start` attempts ended the
process. Targeted repository DLL replacements removed two earlier crashes in installed retail
support DLLs, but those exits had no new Windows Application Error event. See
[WINDOWS_BUILD_VALIDATION.md](docs/WINDOWS_BUILD_VALIDATION.md).
The restored Final build later opened the first Terran campaign briefing with the repository's
matching databases and campaign DLLs. On a German-locale Windows host, its `SCRIPT01.dll` had
only English (US) resources; an English-group fallback in the Mission script string owner let
`TM1_Briefing` update and display its objectives. This is a supported Windows runtime observation,
not retail gameplay parity. Early briefing `Start` attempts removed the visible window inside
`CQE_ENTERING_INGAMEMODE`. The dispatcher trace completed four callbacks and stopped in the
`PlanetFactory` callback. Subsequent isolated tests progressed through repository `Engine`,
`RenderMgr`, `PolyMesh` and, after staging repository `TextureLibrary`, displayed a game scene.
At the 2026-09-25 physical-host Final runtime check of the first Terran mission, the space
background and HUD were visible and the view scrolled, but no units appeared and the mission did
not progress. The user separately reported two visible ships in the Celareon campaign earlier
that day, without a matching trace or build hash. These observations confirm scene entry but
neither playable missions nor retail parity. The temporary
`DataPath=DataOverlay;Data` override and lawful-data junctions apply only to that isolated test
composition. See [WINDOWS_BUILD_VALIDATION.md](docs/WINDOWS_BUILD_VALIDATION.md).
The independent repository-owned render fixture subsequently executed in the supported restored
Final D3D9 process and its 64-by-64 Windows P6 passed a documented comparison against the visible
Linux SDL/Vulkan fixture. This is a cross-backend **synthetic runtime** check. It does not compare
the restored game with retail missions, art, timing or behavior; see
[RENDERER_BACKEND_PROTOTYPE.md](docs/RENDERER_BACKEND_PROTOTYPE.md).
An interactive retail startup attempt on the 2026-09-25 Windows 11 VM reached the logo; the user
reported a visible menu, but the agent did not capture a stable menu or game scene. The VM lacked
a sound device, and a temporary no-sound INI
then exposed a graphics-card incompatibility with the Red Hat VirtIO GPU DOD adapter. The same
error occurred with the documented software renderer selected. This is a VM-specific retail
runtime blocker, not evidence of a restored Debug or Final regression; details and cleanup are
in [WINDOWS_BUILD_VALIDATION.md](docs/WINDOWS_BUILD_VALIDATION.md).

## Runtime smoke tests

| Area | Test | Debug | Final | Retail comparison / evidence |
|---|---|---:|---:|---|
| Startup | Intro/menu reaches stable idle | TODO | TODO | |
| Mission | Start a known retail mission | TODO | PARTIAL | First Terran mission enters a scrollable background/HUD scene; no units or mission progress |
| Rendering | Player ships use correct authored orientation | PASS | TODO | Level and forward in Mission 1 |
| Visibility | Enemy units become visible under retail conditions | PASS | TODO | Earlier report was a unit-identification mistake |
| Combat | Weapons, impacts, damage and destruction complete | PARTIAL | TODO | Combat completes; projectile visual axis remains wrong |
| Explosion | Mesh shatter completes without access violation | PARTIAL | TODO | Representative combat passed; broader soak required |
| Construction | HQ and available orbital structures complete | PASS | TODO | Includes ship production; sentry slot unavailable in test |
| Campaign | Opening combat advances to Fabricator | PASS | TODO | Final-wave/beacon transition still pending retest |
| UI | Selection, orders, build menus and HUD behave | PARTIAL | TODO | Several layout/parity defects remain |
| Save/load | Save and reload the same mission state | TODO | TODO | |
| Audio/video | Music, SFX and cinematics use retail assets | TODO | TODO | |
| Campaign | Complete representative mission from each campaign | TODO | TODO | |

For crashes, retain the build configuration, save/mission, exact reproduction steps, debugger
stack, and—when practical—a TTD trace. For visual differences, capture the same unit, camera,
heading, zoom and mission in retail and rebuilt versions.

## Immediate investigation order

1. Retest the Mission 1 final-wave live-count fix through the beacon-recovery transition.
2. Trace the shared hardpoint/effect transform used by projectiles and engine lights.
3. Restore planet diffuse material and depth/occlusion behavior.
4. Broaden startup and explosion/debris soak coverage.
5. Repeat the same acceptance path in the Final configuration.

### Ship-axis evidence and A/B switch (2026-08-30)

The shipped CMP files are UTF 1.01 containers. Direct inspection of representative Terran,
Celareon, and Mantis root vertex lists shows that most ships are authored with their longest hull
axis on local Z. Examples include `Ttakai.cmp` (Z 225.3 vs. Y 30.5), `TSteele.cmp` (Z 294.7 vs.
Y 45.8), `ssLegionare.cmp` (Z 1142.7 vs. Y 398.0), and `Mthripid.cmp` (Z 508.4 vs. Y 262.0).
This is consistent with a missing authored-Z-forward to game-planar-forward conversion.

`SpaceShip::renderSpaceShip` now honors the existing `CQEFFECTS.bFastRender` option:

- Fast Render on: sequel-era `IMeshInstance::Render` path.
- Fast Render off: legacy `ENGINE->render_instance` path.

This is an A/B diagnostic, not yet the final orientation fix. Compare the same ship and camera in
both modes before changing transforms or mesh data.

Result: Fast Render on and off produced the same approximately 90-degree nose-down orientation
without a crash. The final renderer branch is therefore not the source of the rotation. The next
test restores the dormant `transform.rotate_about_i(90*MUL_DEG_TO_RAD)` conversion in
`SpaceShip::initSpaceShip`, matching the active conversion in `Platform::initPlatform` and the
measured Z-forward authored ship meshes.

Result: restoring the initialization conversion produced no visible change because subsequent
mission/spawn placement replaces the complete gameplay transform. The next controlled test keeps
Fast Render off and passes the same +90-degree correction through the legacy engine's intended
`RF_TRANSFORM_LOCAL` render modifier, leaving gameplay physics unchanged.

Result: the local +90-degree render correction made ships level, proving the missing model-axis
conversion, but mapped the authored bow toward gameplay aft. Preserve the corrected local-up axis
and reverse forward with an additional 180-degree rotation around model-local Y. This also reverses
local right, preserving a proper right-handed rotation rather than mirroring the mesh.
