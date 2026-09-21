# Production Hotkey Event Portability

## Scope

The Linux CMake graph now compiles and executes the existing
`Code/Libs/Src/DAHOTKEY/HKEvent.cpp` service. This is the production DAHOTKEY event processor, not
a replacement implementation. It loads the fixed-width hotkey table through the public
`IFileSystem` interface, tracks keyboard, mouse and joystick state, and sends configured symbols
through its real DACOM `IEventCallback` connection point.

This slice does not compile the hotkey recorder/editor in `HkeyRec.cpp`, obtain joystick samples
from SDL, or construct the game. It proves the service boundary between portable legacy-message
records, a read-only native UTF `IFileSystem`, the production event queue and the hotkey callback
contract. The checked-in source hotkey tables are not evidence from a retail installation.

## Public declaration boundaries

`Filesys.h` retains the Windows declarations on `_WIN32`. Its non-Windows path includes
`FrontierX/LegacyFileTypes.h`, which supplies only the Win32-shaped types and constants required by
the unchanged public `IFileSystem` vtable. Persistent lengths, offsets and results remain explicit
32-bit values. `HANDLE` is pointer-width because it is a process-local runtime token. The serialized
callback typedef now uses the shared `COMAPI` calling-convention macro instead of spelling a
compiler keyword.

`FrontierX/LegacyJoystick.h` defines a 52-byte, fixed-width value matching the fields consumed from
`JOYINFOEX`. It is an internal compatibility record, not a claim that the WinMM API exists on
Linux. `FrontierX/LegacyInput.h` owns the virtual-key and lock-toggle values shared by the SDL
adapter and hotkey service. Windows continues to use its SDK definitions.

The persistent `DAHOTKEY_STRUCT` remains eight bytes. Compile-time assertions protect that record
and its one-byte modifier state. No ID, symbol, table field or on-disk word was widened for LP64.

## Data validation and ownership

The loader accepts the established version `0x101` and three named children: `Version`,
`Binary data`, and `Keyboard text`. It now rejects the input before exposing a usable aggregate
when any of these invariants fails:

- the version child is not exactly one 32-bit word or contains another version;
- the binary header is truncated, the record stride is not exactly eight bytes, the count/stride
  product overflows, or the child has missing or trailing bytes;
- the text header is truncated, the text count differs from the hotkey count, a non-empty table has
  zero-width slots, the count/width product overflows, or the child size is not exact; or
- a fixed-size text slot has no NUL terminator.

The aggregate initializes all runtime state explicitly rather than depending on zero-filled heap
memory. Hotkey records and text are allocated and released through the process-local `IHeap` when
`DA_HEAP_ENABLED` is active. Failed creation is transactional: the aggregate destructor returns
any allocation made before the error. Public virtual-key and text queries reject out-of-range
indices, and native callback paths reject missing message payloads.

These checks describe this native reader's accepted runtime input. They do not redefine the retail
archive format or certify arbitrary retail data. A native Windows comparison must establish
whether any shipped table contains tolerated trailing data or other undocumented variants before
the stricter checks can be treated as cross-platform parity.

## Native read-only UTF adapter

`FrontierX/NativeUtfFileSystem.h` exposes a narrow owner for the existing `IFileSystem` contract.
It validates a UTF 1.01 container with the shared metadata reader, loads requested children by
exact case, and exposes sequential reads, size, position, beginning-relative seek and close.
Writes, mappings, enumeration and directory mutation fail explicitly. Runtime child handles are
pointer-width encoded table tokens starting at one; they are not pointers and never enter the UTF
bytes. Closed tokens are rejected and reusable slots own their payload bytes.

The adapter deliberately calls the bounded `ReadUtfEntry` path for each opened child. This
revalidates the container and avoids mapping compiler-owned structs onto disk. It currently rejects
entries whose stored and logical sizes differ because no decompressor has been established. It is
a sufficient read-only adapter for hotkey startup, not a complete replacement for DOSFile,
`BaseUTF`, `SharedUTF`, asynchronous I/O or writable UTF behavior.

## Executed evidence

`frontierx_hotkey_event_probe` registers the real hotkey and EventSystem factories with
`NativeComponentManager`. It first creates the real aggregate from a memory-backed implementation
of the public `IFileSystem` vtable, then opens the checked-in `Hotkeys.utf` and `DBHotkeys.utf`
through `NativeUtfFileSystem`. The probe verifies:

- exact version, record and text loading;
- keyboard `A`, left mouse button and joystick button-one press symbols;
- key and hotkey state clearing on release;
- callback suppression while disabled without losing state tracking;
- bounded text lookup and virtual-key range rejection;
- inbound joystick delivery through `IEventCallback`;
- shared-heap block growth while the aggregate is live and exact baseline restoration after final
  release; and
- rejection and cleanup for invalid record width, truncated binary data, mismatched text counts,
  unterminated text and an overflowing record count;
- read, size, seek, close, stale-token, missing-child and read-only behavior through the native UTF
  adapter;
- successful parsing and first-label access for both checked-in source tables; and
- the complete synchronous input and queued output path: a legacy key message sent by the
  production EventSystem reaches the hotkey inbound callback, and the translated symbol returns
  through `EventSystem::Notify`, `Post` and `ISystemComponent::Update`.

The real tables contain duplicate hotkey symbols. Their sanitizer run exposed overlapping text
compaction through `memcpy`; both compaction helpers now use `memmove`, which is defined for the
actual overlapping ranges.

GCC and Clang compile the owner and probe with warnings as errors. A focused
AddressSanitizer/UndefinedBehaviorSanitizer run passes with leak detection disabled because the
desktop execution environment runs under `ptrace`; explicit heap enumeration checks the owner's
allocation balance. The source-regression suite checks the portable boundary, size validation,
shared allocator, UTF adapter, shell composition and registered CTest.

The native shell accepts an explicit `--hotkeys FILE` bring-up path. It registers the production
factory, opens the table through the native UTF adapter, subscribes the hotkey input to raw
EventSystem delivery, subscribes hotkey output back to the EventSystem queue, calls the real system
update once per shell iteration, and explicitly removes both subscriptions before teardown. The
option is explicit because native package/resource placement has not been decided; no source-tree
or retail-install path is embedded as policy.

## Remaining acceptance gates

The following work remains before hotkey processing can be described as game-ready:

1. Build the supported VS2022 Win32 Debug and Final configurations and run ABI/vtable checks for
   `IFileSystem`, `IHotkeyEvent`, `IEventCallback`, `HKEVENTDESC` and `DAHOTKEY_STRUCT`.
2. Compare representative retail hotkey data and keyboard-text localization against the Windows
   service, including duplicate symbols, modifier ordering, press/release pairs and malformed data.
3. Select and package the normal and debug hotkey resources for native game startup without
   embedding a developer source-tree path or treating retail media as distributable content.
4. Add an SDL joystick owner that produces the documented fixed-width compatibility value. No
   joystick polling or hotplug path exists yet.
5. Decide how focus reacquisition obtains current modifier and lock-key state. Native startup
   currently clears the service state; it does not query a global keyboard snapshot.
6. Extend the read-only UTF adapter only for concrete runtime consumers; mappings, compressed
   payloads, enumeration, asynchronous access and writable containers remain unsupported.
7. Port or isolate `HkeyRec.cpp` only when a concrete runtime/editor consumer requires recording;
   it is not part of this event-processing slice.

Until those gates close, this work is evidence for one production service owner and not evidence
of a native game, retail input parity, or complete DAHOTKEY module support.
