# Native unit and shared-component coverage

## Executed original catalogue admission (2026-10-15)

**Lawful external retail-input runtime, Linux x86_64 GCC16.2.1, full-media Debug:**
the exclusive original-factory diagnostic preflights all164 race-tagged records,
then loads142 through original ARCHLIST with their named dependencies. Results:
Terran32/38ships and25/26platforms; Mantis18/25ships and24/25platforms;
Celareon18/25ships and25/25platforms. Exit6 is an expected **failed completeness
result**, not a passed all-unit gate. The20flagships remain rejected as described
below. The other two failures request authored model files absent from the supplied
external OBJECTDIR: the Terran Defense Battery and Mantis Antenna Beacon variants.
No other gametype record names them as a dependency; this does not establish
unuse by scripts/numeric callers. No different model is substituted and neither
record is removed from the denominator. Source diagnostics identify the actual
missing asset at its original platform owner.

The first actual diagnostic teardown crashed because BuildShip kept a native
SCRIPT_SET_ARCH pointer token inS32. Its private runtime field now uses the owning
token type; a repeated run retires the catalogue and returns6 without that crash.
A second audit fixes the same truncation at ILauncher::InitLauncher: its dedicated
parameter alias retains exact original S32 on Windows and SCRIPT_SET_ARCH natively,
including all overrides. Owner assertions pin width and the exact method type;
Windows signed-long versus int mangling is preserved. No saved field is widened.

Construction shuttles/drones share OC_SPACESHIP but use their original192-byte
BT_BUILDERSHIP_DATA rather than the race-tagged base ship record. The diagnostic
recognizes those source subtypes and checks that extent before leaving them to
the construction dependency owners; it never reads a nonexistent mission-race
field. Rejected extents and thrown factory errors remain reported failures.

The first fatal UBSan catalogue interval then rejects an actual diagnostic bug:
non-archetype archive leaves were read through an OBJCLASS enum before checking
their discriminator. The diagnostic now inspects that first word as rawU32, and
only recognized unit families acquire typed views. The existing native-shell
lifecycle check exercises the observed out-of-range discriminator, all three
compact construction subtypes and a selected race-tagged ship. No enum sanitizer
or archive assertion is disabled; the failed interval is not external-library
noise. The corrected instrumented retry remains recorded separately.

The final strict original-family Clang ASan/UBSan interval reaches the same142/164
counts and teardown after correcting TextureLibrary raw-MIP new[]/delete[],
Wormhole/MultiCloak/Cloak launcher calloc/free pairs and the analogous native
AreaEffectBolt owner pair found by source audit. No preceding invalid-access,
UBSan or allocator mismatch remains in that bounded interval. Final LSan still
**fails3124bytes/sixallocations**, with DBus allocation frames and unresolved
unloaded callers; there is no suppression or whole-process clean/growth claim.
The earlier failed font-identity startup interval did not load this catalogue
and is distinct from the final successful admission/failed-leak interval.

This run creates no ship instances and starts no simulation. It does not prove
building, boarding, activation, combat, media appearance or saving every unit.
Actual broad instance/ability cycles, post-signature-change admission, targeted
sanitizers and affected Windows Debug/Final/moduleABI remain separate gates.
The initial tables below preserve the pre-expansion source-inventory snapshot;
newly linked source owners are tracked in the later integration notes.

## Inventory snapshot: 2026-10-15

This is **source review and read-only external metadata inspection**, not an
all-unit gameplay pass. The user requested systematic coverage of Terran,
Mantis and Celareon ships, buildings and heroes after the selected TM2 fixes.
The source calls the third race `M_SOLARIAN`; that identifier remains unchanged.
The campaign acceptance ledger remains in
[NATIVE_TERRAN_CAMPAIGN.md](NATIVE_TERRAN_CAMPAIGN.md).

The inspected lawful external game-type archive contains **164 race-tagged ship
and platform archetypes**: Terran 64, Mantis 50 and Celareon 50. These are stored
archetype records, including campaign variants and commander variants, **not
counts of distinct player-buildable roster entries**. No retail record contents,
asset-name dump, saves or private runtime paths are reproduced here.

Classification uses `BASIC_DATA::objClass`, the original spaceship/platform
subtype and `MISSION_DATA::race`. Both base archetypes place their 72-byte
mission metadata after the model filename. Names and filename prefixes are not
substitutes for those fields. Neutral/helper records and fighters require
separate treatment below.

The tables compare original `CreateArchetype` discriminants with the actual
native-shell object list in the generated Ninja graph after vertical/cloak
launcher integration. **Present** means that production source is included in
that link closure. It does not prove successful factory registration, complete
dependency loading, creation, save/load, combat or presentation. The initial
Missile Cruiser failure demonstrates why base-owner presence alone is
insufficient.

## Ships, commanders and campaign variants

| Source family | Terran | Mantis | Celareon | Production owner | Native source closure |
|---|---:|---:|---:|---|---|
| Gunboat | 27 | 13 | 13 | `Gunboat.cpp` | Present; dependency coverage varies |
| Fabricator | 1 | 1 | 1 | `Fabricator.cpp` | Present |
| Harvester | 1 | 1 | 1 | `HarvestShip.cpp` | Present |
| Supply ship | 1 | 1 | 1 | `SupplyShip.cpp` | Present |
| Minelayer | 0 | 1 | 1 | `MineLayer.cpp` | Present |
| Troopship | 1 | 1 | 1 | `Troopship.cpp` | Missing |
| Flagship/admiral | 6 | 7 | 7 | `FlagShip.cpp` | Missing |
| Recon probe | 1 | 0 | 0 | `ReconProbe.cpp` | Missing |
| **Race-tagged ships** | **38** | **25** | **25** | | |

The 20 flagship/admiral records must remain in scope: their original fleet
ownership, authored commander bonuses, UI and persistence cannot be replaced by
ordinary Gunboats. CQ2 formations/command kits are absent from their retail
records and cannot substitute for those behaviors. Conversely, scripted named actors can use ordinary
Gunboat archetypes. The gunboat counts include their stored variants; a catalogue
check must not identify heroes solely by `SSC_FLAGSHIP` or exclude variants by
name. Mission-specific hero behavior also requires its original campaign script;
the linked training/TM1/TM2 scripts do not establish coverage of later scripts.

The archive additionally contains 26 race-neutral ship records: 14 Gunboats,
nine build ships and one each of spider drone, Terran drone and the reserved
torpedo family. Their production consumers include construction and scripted
objects. They are not extra playable-race roster entries. `BuildShip.cpp`,
`SpiderDrone.cpp` and `TerranDrone.cpp` are present; no original factory claimant
for the inspected `SSC_TORPEDO` record was found. That unresolved record must not
be called supported, retail-required or safely unused without caller evidence.

There are also 11 `BT_FIGHTER_DATA` records. They have no matching
`MISSION_DATA::race` field and must be attributed through carrier/wing references.
`Fighter.cpp` and `FighterWing.cpp` are present. Flight, launch/retraction,
weapons, kamikaze behavior and save/load still need their actual callers; a
three-race filter on ship/platform mission metadata does not cover fighters.

## Buildings and platforms

| Source family | Terran | Mantis | Celareon | Production owner | Native source closure |
|---|---:|---:|---:|---|---|
| Build platform | 12 | 11 | 9 | `BuildPlat.cpp` | Present |
| Build/supply platform | 1 | 1 | 1 | `BuildSupPlat.cpp` | Present |
| Gun platform | 4 | 4 | 10 | `GunPlat.cpp` | Present; launcher dependencies vary |
| Jump platform | 1 | 1 | 1 | `JumpPlat.cpp` | Present |
| Refinery/research platform | 5 | 5 | 3 | `RefinePlat.cpp` | Present |
| Repair platform | 3 | 2 | 1 | `RepairPlat.cpp` | Present |
| Sell platform | 0 | 1 | 0 | `SellPlat.cpp` | Missing |
| **Race-tagged platforms** | **26** | **25** | **25** | | |

One additional neutral general-platform record maps to the currently missing
`GeneralPlat.cpp` owner. Research, extensions, docking, resource capacities,
construction queues and destroyed/reloaded associations remain original-owner
requirements even for families whose base factory is present.

## Shared launchers and weapons

The external archive contains 134 launcher records across 13 launcher subtypes
and 71 weapon records across 21 weapon subtypes. These totals include records
whose reachability has not been established. Source/link review identifies the
following missing groups:

| Group | Missing original source or unresolved claimant | Consequence / next evidence |
|---|---|---|
| Troopship/capture | `Troopship.cpp`, then its pod/boarding dependencies | Three race-tagged hulls lack their base factory. Follow real creation and boarding callers. |
| Hero/fleet | `FlagShip.cpp` and its fleet/command-kit/UI consumers | All 20 flagship/admiral variants need their retail fleet/bonus/UI owners; CQ2 command kits are not a substitute. |
| Probe/scouting | `ReconProbe.cpp`, `DumbReconProbe.cpp` | Terran probe and named dependency closure require both ship/projectile paths. |
| Defensive/area weapons | `Aegis.cpp`, `AreaEffectBolt.cpp` | Referenced by Terran unit dependency closure; creation alone will not execute activation/damage. |
| Celareon special systems | `WormholeLauncher.cpp`, `MultiCloakLauncher.cpp`, `Synthesis.cpp` | Named dependencies of race-tagged units; restore original special-operation and save owners. |
| Additional weapon/wing records | `Repulsor.cpp`, `Swapper.cpp`, `KamikazeWing.cpp` | Missing source closure. Numeric special-ability and mission callers still need review; absence from named-reference traversal does not establish nonuse. |
| Economy/general platform | `SellPlat.cpp`, `GeneralPlat.cpp` | Mantis sell platform and neutral general platform lack their base factory. |
| Unresolved legacy discriminants | `LC_SHIPLAUNCH` (two records), `SSC_TORPEDO` (one neutral record) | No source factory claimant found. Determine original caller/provenance before implementing, excluding or substituting anything. |

Named-reference traversal is a conservative aid to ordering work. It finds
references stored as archetype names and follows them recursively, but is not
proof that numeric IDs, research unlocks, special abilities, mission-created
objects or filenames are complete. Its per-record results remain private.
It must not be used to label an unvisited record as CQ2-only or unused.

The remaining groups have these narrower source-review findings:

| Discriminant | Records | Actual dependency/caller evidence | Next action |
|---|---:|---|---|
| `WPN_REPULSOR` | 2 | One has a single special-turret parent, itself without a named parent; both refer to a contact blast. Original `Repulsor.cpp::CreateArchetype` loads its model and optional contact-blast archetype and preloads sound. | Retain as an unresolved special-weapon group. Port its real owner if a catalogue/mission/special-operation caller requires it; then exercise push behavior and persistence. |
| `WPN_SWAPPER` | 1 | No named parent found. Original `Swapper.cpp` has its factory and sound preload; its factory does not load another archetype. | Resolve special-operation and numeric callers before assigning playable coverage. |
| `LC_KAMIKAZE_WING` | 1 | No named parent found. Original `KamikazeWing.cpp` loads the configured fighter archetype. | Resolve its carrier/ability caller and preserve the separate kamikaze wing behavior; ordinary `FighterWing.cpp` is not a substitute. |
| `LC_SHIPLAUNCH` | 2 | No named parent or original factory claimant found; `DShipLaunch.h` retains the data definition. | Determine the production owner or establish provenance/reachability before implementation. |
| `SSC_TORPEDO` | 1 neutral | No named parent or original factory claimant found; spaceship/save headers retain data and save structures, and `SpaceShip.cpp` has a commented template instantiation. | Resolve the missing production implementation and launcher relationship; a schema is not an executable ship owner. |

No named-reference ancestry from the 164 selected unit records reaches these
five groups in the inspected archive. This only bounds that inspection; it does
not settle mission creation, numeric lookup or dormant/alternate data. Records
are not classified as unused or as a different game's content from their names.

Present launcher families include Turret, vertical launch, air defense, fighter
wing, FancyLaunch, cloak, reconnaissance, ping and Talorian launch. Present
weapon families include missile/bolt/autocannon, beam/arc, plasma bolt,
GattlingBeam, tractor, mimic, zealot, stasis, repellent cloud, repulsor wave,
mass disruptor and destabilizer. Their inclusion establishes source closure
only. Optional materials, animation, audio, UI, projectile effects and actual
factory admission remain separate checks.

## Conservative integration batches

1. Finish the concrete TM2 blockers first: original Missile Cruiser factory
   dependencies, missile flight, objective-menu ordering and genuine
   construction/save/reload. Preserve the user's bounded feedback acceptance;
   do not turn it into whole-campaign acceptance.
2. Add the original catalogue diagnostic to the existing native shell and use
   its failures to order shared work. Keep it a separate diagnostic process,
   without mission creation, objectives or progress. Retain all required media
   and ordinary asset validation.
3. Integrate missing ordinary unit owners in dependency groups: troops/boarding,
   probes/scouting, defensive/area weapons, then the remaining race-specific
   special launchers/weapons and economy owners. Use real catalogue failures and
   production callers to refine that order; do not create substitute units.
4. Integrate hero/fleet ownership as its own batch, including original fleet
   groups, formations, command kits, construction and UI. Reuse the existing
   fleet/packet boundaries and keep their Windows/module obligations explicit.
5. Re-run the full catalogue after every batch, then select actual production
   scenarios for build, orders, special actions, combat, save/load and repeated
   close/re-entry. A loaded archetype is not a working unit. Later mission
   scripts and full campaign play remain subsequent integration/acceptance work.

## Catalogue diagnostic contract

The concrete regression is the genuine TM2 construction failure in
`BuildPlat.cpp`: its ordinary completed-unit creation reached a missing factory
because Missile Cruiser launcher dependencies were absent, despite the Gunboat
owner already being linked. Existing synthetic schema/layout and factory checks
do not traverse every actual external unit's recursive dependency graph. The
existing native shell therefore gains the exclusive `--unit-catalog-check`
path, with the original enumeration/load work in `NativeUnitCatalog.cpp`, rather
than a separate probe executable or an alternate unit implementation. A failed
load identifies an integration blocker; a successful load permits subsequent
real creation/use testing and does not remove that gate.

Use the existing `IArchetypeList::EnumerateArchetypeData` callback
(`ObjList.h` / `ObjList.cpp`), which provides name, data and byte size from the
original game-type owner. `IGeneralData` is the separate general/UI database;
it is not the unit-catalogue enumerator.

For ships and platforms, validate the record extent before accessing its
source-defined base and mission metadata. Include all three race IDs and all
their variants. `ObjectList` exposes the archive's raw archetype bytes; neither
enumeration nor `LoadArchetype` performs automatic schema correlation. A base
extent check alone does not protect a factory that reads a larger derived type.
The diagnostic preflights exact known subtype extents for the complete selected
set before invoking any factory. Unknown subtypes and mismatched extents fail
the diagnostic; they remain in its selected-unit denominator and invalid-record
count and are never passed to their factories. Other validated records still
exercise the real `LoadArchetype` path after the complete preflight, so one
unsupported family does not conceal unrelated dependency failures. A production owner's
explicit, validated old-format adapter is required before admitting a known
older record. These unit checks do not themselves validate all recursively
loaded component records.

Use `ARCHLIST->LoadArchetype` to exercise the real recursive
factory/material/mesh/launcher dependencies, with original reference ownership
and cleanup. Record base type, race, success/failure, dependency failure and
whether registration/asset loading actually executed. Keep detailed private
names/errors outside source control and CI artifacts.

Do not call `CreateInstance` merely to make a missing unit appear supported.
The diagnostic must not set objectives, completion bits, player resources or
campaign state. Run it exclusively and exit before playable mission startup:
archetype initializers have not all been proved RNG-free, so diagnostic asset
loading must not influence an ordinary gameplay session. A successful catalogue
is **factory/asset admission evidence**, not creation, combat, save compatibility
or complete graphics/audio acceptance.

## Evidence and open gates

This snapshot executed a read-only UTF metadata walk over the lawful external
game-type archive, classified original class/subtype/race fields, reviewed
original `CreateArchetype` bodies and compared their sources against the
generated native-shell Ninja object list. It found 25 race-tagged ship/platform
records whose base factory source is absent from that closure, before considering
missing recursive components. No media or archive records were copied into the
repository. No all-unit runtime check was executed for this inventory.

The existing focused layout/schema checks cover selected integrated owners;
they do not validate every record listed above. Original VLaunch deliberately
has no `ISaveLoad` interface, whereas CloakLauncher persists its eight-byte
record; do not invent a parallel vertical-launcher save owner.

On 2026-10-15, isolated GCC Debug-compatible compilation of the existing
`frontierx_layout_probe` and `frontierx_view_parser_probe` succeeded with the
new flagship, troopship/pod, reconnaissance, wormhole, multi-cloak and synthesis
contracts. Running the layout probe and the parser's existing
`--app-schema Code/App/Src/data.i` mode succeeded. This synthetic execution checks
fixed source data/save extents, final wormhole target-array entry, flagship
formation/command-kit arrays, IDs, fractional timers and packed state flags
through the production schema correlator. It neither creates units nor advances
objectives, progress or RNG. The inspected external flagship data is 744 bytes
per record versus the canonical 1384-byte current source schema. This is a
concrete factory-input compatibility blocker: the source flagship owner cannot
safely read the larger type from those raw bytes. Its original owner's explicit
old-format adaptation is required; `ARCHLIST` supplies no automatic correlation.
The synthetic same-schema check does not establish retail flagship
compatibility. The other
inspected reconnaissance/wormhole/multi-cloak/synthesis archetype extents match
their source definitions.

An isolated GCC source-fixture execution of the catalogue's subtype preflight
accepted exact extents and rejected one-byte-short/long records for each of its
15 recognized ship/platform families. This ran without `ARCHLIST`, factories or
mission startup. The actual external catalogue remains a separate host check;
the current flagship mismatch is a failure, not an excluded unit or a pass.

An actual sanitizer catalogue attempt exposed that enumeration also returns
non-archetype leaves: reading their first word as `OBJCLASS` triggered UBSan
before classification. The diagnostic now reads class and subtype words as
raw `U32` values before any enum view. It also distinguishes the three compact
construction-shuttle subtypes, which have no race metadata, from full ships.
The existing native-shell lifecycle check exercises a non-archetype word,
compact shuttle records, invalid raw ship/platform subtypes and a correctly
race-tagged Gunboat. An isolated Clang UBSan execution passes, including invalid
subtypes retained as failing records. The isolated ASan/UBSan attempt reached
normal test completion but its leak phase was unavailable under the sandbox's
ptrace restriction; that is not a leak pass. Full host sanitizer catalogue
execution remains separately recorded by the integration runner.

That catalogue path also reached the production TextureLibrary 1.6 loader and
exposed its raw-TGA `new[]`/`delete` mismatch. The owner now uses `delete[]`.
The existing material-manager probe gains a synthetic indexed TGA stored directly
in `MIP0`, distinct from the previous nested indexed/RGB565 fixtures. Three
load/release cycles check exact 2-by-2 RGBA upload bytes, the original loader's
row mapping, palette colors and removal of cached texture ownership. Isolated
GCC compilation/linking and full probe execution pass; the affected Clang and
sanitizer graph runs remain the integration runner's next check. No source
fixture contains retail data.

The existing layout check also exercises the bounded 744-byte retail flagship
decoder with a synthetic byte fixture: valid scalar/ability/toolbar fields,
null and short/long input, invalid hotkey and each ability byte, NaN/infinity
in all 13 floating fields, and unchanged output on rejection. Standalone GCC
and the configured Clang layout compile/run pass. Decoding these fields is
storage-contract evidence only: the retail bonus/ability semantics differ
from the released CQ2 flagship owner, so all 20 retail flagship variants remain
unsupported until those original behaviors are implemented. They cannot be
made playable by mapping unrelated CQ2 command kits or dropping abilities.

Source review of the newly integrated WormholeLauncher/Blast boundary also
finds an explicit persistence blocker. The released launcher has no working
`Load` and the portal effect has no saved-state owner. Native `Save` now fails
explicitly rather than reporting success with missing state. Consequently any
ship carrying that launcher blocks a complete mission save, including an idle
launcher; this is not limited to an active portal. Archetype admission and the
effect's texture ownership do not establish functional save/load coverage.

GCC/Clang builds, targeted sanitizers, actual catalogue admission, actual unit
creation/use/reload, complete presentation and user acceptance must be recorded
per integration batch. Native Windows Debug/Final and affected cross-module
ABI/layout checks remain required and unexecuted here. Full M3 comparison stays
separate. Counts and source presence must be refreshed after factory integration;
historical missing-owner rows must not silently become runtime passes.
