# Native Settings Storage

Status: storage, fixed-width schema, path policy, legacy value/MRU adapter, native lifecycle owner,
and shell-integrated `IUserDefaults` DACOM component implemented and tested; native game startup,
portable UI services and Windows comparison pending

## Source-reviewed Windows contract

`Code/App/Src/UserDefaults.cpp` owns the supported Windows implementation. Retail builds use
`HKEY_CURRENT_USER\Software\Fever Pitch\Conquest: Frontier Wars\1.00`; debug and demo builds alter
the final product/version names. The public `IUserDefaults` interface exposes two registry value
classes:

- `REG_SZ` values for the player name, render-device identifier, remembered directories and MRU
  paths; and
- `REG_BINARY` values for display dimensions, pixel depth, arbitrary callers, and the complete
  `USER_DEFAULTS` blob.

The generic getters return zero for missing or wrong-type values and `0xffffffff` when a supplied
buffer is too small. Successful Win32 string reads report the registry byte count, including its
terminating NUL. Supplying a null string to the named setter deletes that value. MRU keys use
`<resource-id>-MRU<position>` and retain at most four entries.

The source-reviewed lifecycle is not a single shutdown save:

- construction establishes defaults and publishes the live `USER_DEFAULTS` pointer;
- `Conquest.cpp` calls `LoadDefaults` immediately after creating the component;
- the options menu reloads stored defaults when it establishes its reset baseline;
- confirmed options, editor/cheat/UI toggles and multiplayer setup store immediately; and
- the component destructor stores once more before clearing the published pointer.

`LoadDefaults` first attempts `User Defaults-<player-name>` when `CQPlayerName` exists. A missing
player blob resets to defaults while preserving the current hardware-cursor bit; an invalid player
blob is deleted and lookup continues to the global `User Defaults` value. A missing global value
leaves the current in-memory defaults unchanged, while a wrong type, size, or version resets them.
Opening the registry key is the operation that determines the method's success result.

The complete defaults blob is accepted only when its byte count equals `sizeof(USER_DEFAULTS)` and
`dwVersion` is 11. That representation contains native `bool`, enum and bitfield layout and is tied
to the supported Win32 build. It must not become the native Linux disk schema. Player-specific
copies use `User Defaults-<player-name>`; incompatible player blobs are deleted, while incompatible
global blobs reset the in-memory defaults.

File dialogs, current-directory restoration, window placement, DirectPlay-era installation data,
script data and the live global `USER_DEFAULTS` object are separate responsibilities in the same
legacy implementation. The portable storage primitive does not claim to implement them.

## Legacy value and MRU adapter

`FrontierX::LegacyPreferenceValues` maps the value-oriented subset of `IUserDefaults` onto
`PreferenceStore` without importing Windows registry or file-dialog APIs. Its executable contract
matches the source-reviewed behaviors needed by current callers:

- binary and string types never coerce into one another;
- missing or wrong-type reads return zero;
- a short destination returns `0xffffffff`;
- successful string reads include the terminating NUL in their byte count;
- an empty string therefore returns one, while an empty binary value returns zero;
- a null named-string setter removes the value and reports success;
- MRU keys use the exact decimal `<resource-id>-MRU<position>` spelling;
- positions zero through three are valid, insertion shifts entries toward position three, and
  positional removal compacts until the first gap; and
- removal by value deletes every exact, case-sensitive match but preserves the legacy method's
  always-false return value.

The adapter improves only memory safety outside the observable valid-call contract: a zero-capacity
string read no longer writes through `buffer[-1]`, and null output is rejected. Store validation
can also reject invalid keys, values beyond the native format limit, or a full container; the
Windows implementation ignores individual `RegSetValueEx` failures after opening/creating the key.
Those failure-path differences must be resolved at the live application boundary, not hidden.

The real `IUserDefaults` declaration now uses the existing `COMAPI` macro and exact `DACOM.H` case.
MSVC therefore retains its prior calling convention and import/export decoration, while GCC and
Clang compile the unchanged interface order through `frontierx_dacom_interface_probe` and execute
it through `frontierx_native_user_defaults_probe`. This does not establish cross-compiler ABI
parity with the supported MSVC module.

## Native settings owner and player precedence

`FrontierX::NativeUserSettingsOwner` is the first owning native lifecycle boundary.
`FrontierX::StartupPreferences` composes path selection, container loading and this owner into one
transactional startup result shared by the headless checker and native shell. Neither executable
calls the global schema decoder directly.

Player-specific settings use a new native namespace:

```text
Players.<lowercase hexadecimal UTF-8 player-name bytes>.<schema key>
```

Hex encoding keeps arbitrary non-NUL player-name bytes distinct while producing valid deterministic
`PreferenceStore` keys. The 63-byte limit matches the production owner's 64-byte stack buffer,
including its terminator.

The load order follows the source-reviewed valid-data behavior:

1. Read the typed `CQPlayerName` value.
2. If a named player schema exists and is valid, use it without reading the global schema.
3. If the player schema is absent, reset to native defaults, preserve the current hardware-cursor
   bit, and return without falling back to global settings.
4. If the player schema is invalid, remove only that encoded player namespace and try global
   settings.
5. If no player name exists, try global settings directly.
6. A missing global schema leaves the owner's current settings intact; a valid one replaces them
   transactionally.

The native policy deliberately treats an invalid global schema or an overlong player name as an
error for the headless checker. The Windows code normally reports a successfully opened registry
key after resetting an invalid global blob. Worse, an invalid player blob followed by a missing
global blob can leave bytes written by `RegQueryValueEx` in the live struct. Reproducing corrupted
compiler-owned bytes would violate the transactional native schema; native Windows comparison must
decide the final user-visible recovery policy before game integration.

Saving first writes the global schema into a copy of the store, then writes the selected player's
schema when a valid player name exists, and publishes the copy only if both conversions succeed.
This models the two production writes without exposing a half-updated in-memory store. File-system
save durability and application save timing remain separate gates.

Startup diagnostics report `settings-source: defaults`, `global`, or `player` without printing the
player name or settings path. Headless process tests exercise all three sources plus invalid
player-to-global fallback; native-shell tests cover first-run, explicit valid, missing and malformed
container paths.

## Portable container

`FrontierX::PreferenceStore` stores printable-ASCII keys with either an opaque byte value or a
non-NUL string value. It deliberately retains binary/string type information so a future adapter
can reproduce wrong-type failures instead of silently coercing values.

Limits are part of the file-validation contract:

| Item | Limit |
|---|---:|
| Complete file | 1 MiB |
| Entries | 4,096 |
| Key | 255 bytes, printable ASCII `0x20` through `0x7e` |
| Value | 64 KiB |

Empty binary and empty string values are valid. Embedded NUL bytes are allowed only in binary
values. Missing files report `NotFound` and do not alter the current store. Invalid, truncated,
oversized, duplicate-key, unknown-type, or trailing-byte input reports `Invalid` and likewise
preserves the current state. I/O failures are distinct from format failures.

## Linux path selection

The initial native policy follows the
[XDG Base Directory Specification 0.8](https://specifications.freedesktop.org/basedir/0.8/):

1. An explicit `--preferences FILE` path wins and may be relative to the working directory.
2. A non-empty, absolute `XDG_CONFIG_HOME` selects
   `$XDG_CONFIG_HOME/frontierx/preferences.dat`.
3. Otherwise an absolute `HOME` selects `$HOME/.config/frontierx/preferences.dat`.
4. Relative XDG or HOME values are invalid as base directories. A relative XDG value is ignored so
   the HOME fallback can apply; if neither absolute base exists, startup reports its executable's
   preferences failure code.

`LoadStartupPreferences` applies this policy before data-root validation in both startup
executables. A missing default file is normal first-run state and yields an empty store. A missing
explicit file is an error so a mistyped diagnostic path is not silently ignored. Output identifies
only the selection source, load status, entry count and settings source; it does not disclose the
user's resolved path. Both consumers are read-only and do not create the application directory.
The headless checker retains exit code 6 for unusable preferences; the native shell uses exit code
5 because its existing codes 1 through 4 already identify argument, data/configuration, component
and window failures.

## Native `IUserDefaults` component

`frontierx_native_user_defaults` implements the unchanged public `IUserDefaults` vtable as a native
DACOM service. The native shell registers and creates it through `NativeComponentManager`, keeps
the live `IUserDefaults::GetDefaults()` pointer valid for the process lifecycle, performs an
explicit shutdown store, then releases it before manager teardown.

The component maps every `NativeUserSettings` field and all 31 explicit flag bits to the legacy
`USER_DEFAULTS` representation in memory. `dwSize` and version 11 are populated for legacy callers,
but the compiler-owned struct and bitfields are never written to disk. Stores convert back into the
fixed-width schema first; non-finite scroll rates or invalid fog values fail rather than serializing
undefined state.

Named binary/string and numeric-resource string operations use `LegacyPreferenceValues`. Their
set/delete and MRU mutation calls write a copied store through same-directory replacement before
publishing it in memory, so a failed write is transactional. `StoreDefaults` writes both global and
selected-player schemas and the component destructor repeats the source-reviewed shutdown store.
Missing parent directories are created under the selected settings path using the process umask.
Explicit fsync, concurrent-writer arbitration and temporary-file recovery remain open durability
policy.

The following Windows-coupled methods have safe, explicit native failure behavior until their own
owners exist:

- input/output file dialogs clear the supplied result and return false;
- document/viewer-backed user and script editors return false;
- installation-registry lookup returns no value; and
- `UbiEula` returns false, so missing native EULA presentation can never be mistaken for acceptance.

These failures make the interface executable for non-UI startup consumers without claiming that
the game can yet enter every menu/tool path. The production `CreateUserDefaults` function still
constructs the Win32 implementation; a native game-startup owner must select the new component
without altering that supported Windows path.

## Native user-default schema

`FrontierX::NativeUserSettings` replaces the raw compiler-owned `USER_DEFAULTS` blob with 25
required version-1 entries. Every scalar occupies an explicitly decoded four-byte binary value.
Signed values use their 32-bit two's-complement bit pattern; `Camera.ScrollRate` stores the bits of
an IEC 60559 32-bit float. No native struct is copied to disk.

| Key or key group | Encoding | Source-reviewed default |
|---|---|---:|
| `UserDefaults.Schema` | U32 | 1 |
| `Window.MainX`, `Window.MainY`, `Window.Width`, `Window.Height` | S32 each | 0 |
| `Window.ShowState` | U32 | 5 (`SW_SHOW`) |
| `Viewer.Width`, `Viewer.Height` | S32 each | 100 |
| `Network.MinLatency`, `Network.PacketLossPercent` | U32 each | 0 |
| `Input.MouseSpeed` | S32 | 0 |
| `Camera.ScrollRate` | float32 bits | 1.0 |
| `Sound.Music.Muted`, `Sound.Music.Volume` | U32 boolean, S32 | false, 6 |
| `Sound.Effects.Muted`, `Sound.Effects.Volume` | U32 boolean, S32 | false, 9 |
| `Sound.Comms.Muted`, `Sound.Comms.Volume` | U32 boolean, S32 | false, 7 |
| `Sound.Chat.Muted`, `Sound.Chat.Volume` | U32 boolean, S32 | false, 9 |
| `Video.GammaCorrection` | S32 | 0 |
| `Simulation.GameSpeed` | S32 | 0 |
| `Simulation.FogMode` | S32: normal -1, explored 0, none 1 | -1 |
| `UserDefaults.Flags` | U32 explicit mask | `0x02822000` |
| `Simulation.MaxProjectiles` | U32 | 20 |

The flag word does not inherit compiler bitfield ordering. Its version-1 assignments follow the
source declaration order explicitly:

| Bit | Meaning | Bit | Meaning |
|---:|---|---:|---|
| 0 | Window mode | 16 | No hints |
| 1 | No auto-save | 17 | Right-click option |
| 2 | No tooltips | 18 | Information highlights |
| 3 | No frame limit | 19 | No supplies |
| 4 | Position tool uses world coordinates | 20 | No damage |
| 5 | Editor mode | 21 | Show grids |
| 6 | Draw hotrects | 22 | Cheap movement |
| 7 | No status bar | 23 | Hardware renderer |
| 8 | Visibility rules disabled | 24 | Constant update rate |
| 9 | Multiplayer chosen | 25 | Hardware cursor |
| 10 | Join chosen | 26 | Lock diplomacy |
| 11 | Part dialog visible | 27 | Spectator mode allowed |
| 12 | Cheats enabled | 28 | Spectator mode active |
| 13 | Sectormap rotates | 29 | Subtitles |
| 14 | No auto-target | 30 | High network bandwidth |
| 15 | No winning conditions | | |

Bits 31 and above are invalid. The default mask sets bits 13, 17, 23 and 25, matching
`UserDefaults::init`. All fields are required once `UserDefaults.Schema` exists; absent schema means
first-run defaults, while a missing field, wrong type/size, non-finite scroll rate, unknown flag,
invalid fog value or unsupported version rejects the whole settings conversion without modifying
the caller's previous state. Unknown unrelated preference keys are preserved.

Headless startup performs this conversion after the container validates. Its summary reports
`user-defaults: loaded` or `user-defaults: absent`; invalid native defaults use preference exit
code 6. This proves process-level schema loading, not integration with the live game singleton.

## `FXSET001` byte format

All integers are unsigned 32-bit little-endian values decoded byte by byte. Native structs are
never mapped onto file bytes.

| Offset | Size | Meaning |
|---:|---:|---|
| 0 | 8 | ASCII signature and format version `FXSET001` |
| 8 | 4 | Entry count |
| 12 | variable | Entries in ascending bytewise key order |

Each entry is encoded as follows:

| Relative offset | Size | Meaning |
|---:|---:|---|
| 0 | 4 | Key byte count |
| 4 | 4 | Type: 1 binary, 2 string |
| 8 | 4 | Value byte count |
| 12 | key byte count | Key bytes, without a terminator |
| following | value byte count | Value bytes, without an implicit terminator |

Sorted `std::map` traversal makes identical stores serialize identically. Saving first writes and
closes `<target>.frontierx-tmp` in the target directory and then renames it over the target. This
avoids exposing a partially written target to another process on the tested Linux filesystem. It
is not yet a power-loss durability guarantee: file and directory `fsync`, concurrent-writer policy,
permissions, and stale-temporary-file recovery remain to be specified with the final settings-path
policy.

## Verification

The registered `frontierx_preference_store_probe` covers:

- missing-file behavior without state loss;
- string, binary and empty-value round trips;
- stable serialization from two saves and replacement of an existing target without a stale
  temporary file;
- the signature, entry count and explicit little-endian bytes;
- invalid keys, null binary input and embedded string NUL rejection;
- removal behavior; and
- invalid signature, unknown type, duplicate key, trailing data and truncated values without state
  replacement.

Reproduce with either checked-in compiler preset:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug
ctest --preset linux-gcc-debug

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug
```

Both presets pass all twenty-eight CTests. The preference probe additionally covers legacy binary/string
type checks, exact counts, short buffers, deletion, four-entry MRU shifting, lookup, compaction,
duplicate removal and the historical removal return value. Focused GCC builds using
AddressSanitizer and UndefinedBehaviorSanitizer also passed with leak detection disabled because
LeakSanitizer is incompatible with the host's ptrace-based execution environment.

The native-settings probe additionally covers prefixed round trips, global and player selection,
the missing-player reset, hardware-cursor preservation, namespace-scoped invalid-player cleanup,
global fallback, invalid-global rejection, dual-copy store, oversized player names, default and
explicit shared-startup selection, transactional error preservation and redacted diagnostic names.
The headless process suite constructs `FXSET001` fixtures for player, global, missing-player and
invalid-player cases. The native-shell process suite exercises default first-run selection, an
explicit valid empty container, missing or malformed explicit files, live component creation and a
persisted fixed-width shutdown schema. `frontierx_native_user_defaults_probe` separately executes
the real vtable across defaults exposure, binary/string short-buffer rules, deletion, numeric keys,
MRU behavior, immediate writes, complete schema store/reload, player deletion, reset, singleton
lifetime and explicit rejection of unsupported UI/EULA operations. Focused Clang
AddressSanitizer/UndefinedBehaviorSanitizer runs pass with leak detection disabled for the host
ptrace environment.

## Integration gates

Before this component can serve the native game:

1. Select the tested component from a real native game-startup owner while retaining the supported
   Win32 `CreateUserDefaults` and registry implementation.
2. Connect shutdown/save failure reporting to the future application owner; define explicit fsync,
   concurrent-write and temporary-file-recovery policy.
3. Connect the now-portable MRU manipulation to a native file-picker boundary while keeping UI,
   current-directory restoration and path-separator behavior outside storage.
4. Port or isolate the document/viewer editors, installation metadata and EULA presentation before
   enabling callers that depend on those methods.
5. Keep the current registry-backed implementation for supported Windows builds and run Debug and
   Final regression builds after any shared-interface integration.
6. Extend process coverage from the shell to a real game-startup save/restart before claiming
   persistent native settings work in the game.

This format is new FrontierX configuration data. It is not a retail file format and does not import,
copy, or redistribute retail media.
