# DACOM Runtime Pointer-Width Readiness

Status: isolated source migration and native helper probe, 2026-09-17

This document records the first pointer-width migration inside the shared DACOM template layer.
It covers interface-map offsets, connection-point owner recovery, connection handles, and dispatch
property addresses. It does not claim that DACOM, its loader, its heap, or the game builds on
Linux.

## Problem and ownership

The public templates previously recovered owners and interface members by converting object
addresses to `U32`, performing arithmetic, and converting the result back to a pointer. That is a
real address truncation on Linux x86_64, unlike DOSFile's deliberately encoded small indices. The
affected owners were:

- `TComponent.h`: interface-map base/member offsets and the reference-entry marker;
- `TConnContainer.h`: connection-point member lookup from the containing component;
- `TConnPoint.h` and `TCPoint.h`: owner lookup plus pointer-derived `U32` subscription handles;
- `TResource.h`: CRTP owner recovery; and
- `tdispatch.h`: CRTP owner recovery and absolute/static property addresses.

These templates are shared by `Conquest`, `Mission`, `Trim`, `Globals`, `ZBatcher`, and engine
libraries. Their native Windows rebuild and ABI checks are therefore an acceptance gate, not an
optional follow-up.

## Runtime offset contract

`FrontierX/DacomRuntime.h` defines `DacomRuntimeOffset` as `std::size_t`. DACOM interface-map and
dispatch-address records now use pointer-width runtime storage where they can contain an address
or an in-memory offset. Persistent file, save, campaign, and network structures are unchanged.

The interface-reference marker remains the high bit of the runtime offset type:

- Win32: bit 31, preserving the existing four-byte field and marker value;
- Linux x86_64: bit 63, leaving the complete non-marker offset range below it.

CRTP base owners are recovered with `static_cast`, which lets the compiler apply the correct
multiple-inheritance adjustment. Member connection points use byte offsets from their containing
component and recover that component through the shared pointer-width helper. The isolated probe
compares the calculated secondary-base offset with a real multiply inherited object and verifies
member-to-owner round trips.

The static interface-map architecture still needs a base-subobject offset without a live object.
The helper retains the legacy non-null synthetic-address technique, but performs it entirely in
`std::uintptr_t` rather than narrowing through `U32`. GCC and Clang plus AddressSanitizer and
UndefinedBehaviorSanitizer accept the characterized cases. This is not a proof for arbitrary
virtual inheritance; the production maps and a native Windows ABI oracle remain authoritative.

## Connection handle contract

`IDAConnectionPoint::Advise` and `Unadvise` expose `U32` handles. Changing that signature would
break the established cross-module ABI, while continuing to return a connection-node address
would truncate pointers on a 64-bit host. Each private connection node therefore now stores an
explicit `ConnectionToken` (`std::uint32_t`).

Token allocation has these observable rules:

1. zero is never issued;
2. the lowest unused positive token is selected deterministically;
3. removing a connection makes its token available for reuse;
4. exhaustion returns zero internally and `Advise` reports `GR_OUT_OF_SPACE`; and
5. `Unadvise` searches by token rather than reconstructing a pointer.

Node addresses remain native pointers and never cross the public interface. The node structures
are process-private heap objects, not persistent or wire layouts. On Win32 the pre-existing fields
keep their order; the token is appended after them. Native Windows runtime validation must still
exercise subscriber ordering, repeated advise/unadvise, token reuse, destruction, and heap
ownership across every affected module.

## Dispatch scope

Static property addresses in `tdispatch.h` now use `std::uintptr_t`; instance-property offsets use
byte-pointer arithmetic; and the CRTP owner is obtained with `static_cast`. The dynamic method-call
path still decodes member-function pointers and builds calls with 32-bit MSVC inline assembly.
Consequently, the dispatch template is **not portable yet**. This change removes the measured
address truncations without claiming that invocation works under GCC, Clang, or a 64-bit ABI.

## Portable public interface and component-map header

`DACOM.H` previously stopped every non-Windows translation unit with `#error`, even though its core
interfaces require no OS implementation. Its import/export and calling-convention macros are now
empty outside MSVC while retaining the existing `__declspec` and `__stdcall`/`__cdecl` behavior on
Windows. Its direct `TYPEDEFS.H` and `Results.h` includes now match the on-disk case.

`TComponent.h` likewise uses the exact `DACOM.H` spelling, standard `<cstring>` lookup, explicit
dependent-base names, and portable map-function decoration. Its optional debug reference traces
remain active when the pre-existing trace macros are visible; the header is now self-contained
when they are not. No virtual destructor was added: `IDAComponent` deliberately retains its legacy
vtable ABI, and the probe suppresses only the compiler warning for that characterized ownership
pattern.

`frontierx_dacom_interface_probe` includes the real production headers. It checks fixed-width DACOM
aliases, direct `IDAComponent` query/reference behavior, and an instantiated `DAComponent` inbound
interface map. It also instantiates the production `ConnectionPointContainer` and `ConnectionPoint`
templates and executes interface lookup, point-to-owner recovery, two subscriptions, enumeration,
unsubscription, and deterministic reuse of the lowest free token. GCC, Clang, AddressSanitizer,
and UndefinedBehaviorSanitizer execute this path. This is the first native execution of the public
component/map/connection-template layer, not of `DACOM.CPP`, the loader, heap providers, the second
application-owned `TCPoint` implementation, cross-module callbacks, or a game-owned component.
The subsequent native `EventSystem` target is the first production DACOM service-owner closure;
see [EVENT_SYSTEM_PORTABILITY.md](EVENT_SYSTEM_PORTABILITY.md).
`NativeComponentManager` now hosts that factory through the real `ICOManager` interface with
priority ordering, fallback, enumeration, reference ownership and shutdown. It is deliberately a
native manager; see [DACOM_MANAGER_PORTABILITY.md](DACOM_MANAGER_PORTABILITY.md).

## Reproduce the current evidence

From the repository root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target \
  frontierx_dacom_runtime_probe frontierx_dacom_interface_probe
ctest --preset linux-gcc-debug -R 'dacom_(runtime|interface)' --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target \
  frontierx_dacom_runtime_probe frontierx_dacom_interface_probe
ctest --preset linux-clang-debug -R 'dacom_(runtime|interface)' --output-on-failure

PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover \
  -s Tools/portability -p 'test_platform_inventory.py' -v
```

Both complete preset builds passed all 24 CTests on Linux x86_64 on 2026-09-17. A focused GCC
build with AddressSanitizer and UndefinedBehaviorSanitizer also passed after leak detection was
disabled because LeakSanitizer cannot operate under this environment's process tracing. The
production regression test rejects reintroduced pointer-to-legacy-integer casts in the six
migrated headers and verifies that connection handles use tokens rather than node addresses.

Immediately after this DACOM slice, the lexical production inventory reported 11 high-confidence
object/address-to-legacy-integer casts in seven App files, down from 18 in twelve files. The
subsequent stack-guard migration reduced the current count to five in four files. No candidate
remains in the public DACOM templates. Lexical absence is not compile or runtime proof.

## Outstanding acceptance gates

Before this migration can be accepted as a complete DACOM integration:

- run clean VS2022 Win32 Debug and Final rebuilds for all affected modules;
- run the parameterized native-Windows ABI and layout checks against fresh PE binaries;
- add native Windows behavioral tests for interface lookup, cross-module callback/heap ownership,
  and both connection-point implementations;
- adapt a production component to the synthetic-tested ELF module contract, add profile-bootstrap
  and heap policies, then compile another game- or renderer-owned component;
- continue normalizing exact-case include spellings in each selected owner closure without creating
  case-colliding files that are unsafe on Windows filesystems;
- replace or isolate the x86-only automation method-call assembly before porting the four legacy
  Viewer owners that currently reference it;
- characterize DA heap ownership, component loading, `Send`, and connection callbacks; and
- compare behavior through a lawful retail startup and representative module interactions.

Until those gates pass, the evidence is limited to source review, isolated compilation, synthetic
execution, sanitizers, and lexical regression checks. There is still no native Linux game target.
