# Stack-Local Debug Guard Portability

Status: shared pointer-width helper and isolated native probe, 2026-09-17

Several mission/object wrapper types enforce a debug-only lifetime rule: volatile temporary
references must be created near the active stack frame rather than stored in long-lived objects.
The original checks read the 32-bit x86 `esp` register with MSVC inline assembly and narrowed
`this` to `U32`. That code neither compiles on x86_64 GCC/Clang nor preserves an address on a
64-bit host.

## Migrated owners

The six checks are in three independently used headers:

- `Include/MGroupRef.h`: one mission group-reference guard;
- `MPart.h`: one mission-part handle guard; and
- `Objwatch.h`: four volatile `OBJPTR`/`OBJPTR2` guards.

This satisfies the project's requirement for at least two concrete consumers before introducing a
shared boundary. `FrontierX/StackAddress.h` contains no OS API and does not enter persistent
layouts.

## Preserved contract

`IsAddressNearCurrentStack` places a byte marker in its own active stack frame, converts both
addresses to `std::uintptr_t`, computes an overflow-safe absolute distance, and requires that
distance to be strictly less than the caller's limit. Every migrated caller retains the historical
limit of 8,196 bytes and its existing debug assertion message where one existed.

The absolute difference deliberately supports either stack-growth direction. The original
unsigned subtraction only behaved as intended when the checked object had the expected address
ordering relative to `esp`. The new check preserves the intended near-stack heuristic while
removing that x86-specific assumption.

This remains a heuristic, not a proof that an address belongs to the thread's complete stack
mapping. It is appropriate for the existing short-lived debug assertion and avoids adding Linux,
Win32, pthread, or virtual-memory queries to mission/game headers. It does not weaken or catch and
ignore a failed assertion.

No object fields changed. In particular, the retail-script ABI assertion that `MGroupRef` is 92
bytes remains intact. No save, campaign, network, or runtime object layout was widened.

## Evidence

The `frontierx_stack_address_probe` target checks ascending and descending distances, the exclusive
limit boundary, identical addresses, and a real nearby local object. It compiles with strict
warnings and passes under GCC and Clang. A focused GCC AddressSanitizer plus
UndefinedBehaviorSanitizer run also passes (with leak detection disabled because LeakSanitizer is
incompatible with the execution environment's tracing).

The production inventory regression rejects a return to x86 `esp` assembly or pointer-to-legacy-
integer casts in all three migrated headers. After this change, the lexical inventory reports:

- 1,233 scanned production source/header files at this slice;
- 1,322 general legacy-integer casts in 226 files at this slice;
- 442 inline-assembly matches in 19 files, down from 448 in 22 files; and
- five high-confidence pointer/address truncations in four App files, down from 11 in seven files.

The focused probe and source-regression checks pass under GCC and Clang on Linux x86_64. These are
compile and synthetic runtime results; the production headers are not yet part of a Linux game target.

## Remaining gates

- Rebuild the affected Conquest, Trim, Mission, Globals, and script-module closures in native
  VS2022 Win32 Debug and Final configurations.
- Exercise the debug assertions in representative stack-local and deliberately invalid lifetime
  scenarios on Windows.
- Keep `MGroupRef` and every campaign ABI-linked layout assertion enabled.
- Compile the owning mission/object closure under Linux once its surrounding Win32 and DACOM
  dependencies are isolated.
- Do not replace this bounded diagnostic heuristic with serialized flags or persistent pointers.

The five candidates that remained at this slice belonged to `DSStream.cpp`, `DumpView.cpp`,
`EffectPlayer.cpp` (two), and `Modal.cpp`. Their subsequent owner-specific migration reduced the
current measured category to zero; see [RUNTIME_CONTEXT_PORTABILITY.md](RUNTIME_CONTEXT_PORTABILITY.md).
