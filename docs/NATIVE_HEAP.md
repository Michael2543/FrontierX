# Native Heap Bootstrap

## Scope

FrontierX now has a native, process-local implementation of the existing `IHeap` interface. It is
registered before the profile factories in the process-global `DACOM_Acquire` owner, and the public
`InitializeDAHeap`, `HEAP_Acquire`, `SetDefaultHeapMsg`, and `HEAP` entry points are available to
native targets. Both the static production EventSystem and its ELF variant use the native heap.
The ELF is the first proven cross-image consumer. That one closure does not establish that arbitrary
future modules have correct allocation ownership.

The supported VS2022 Win32 path is unchanged. `HeapObj.h` now spells its calling conventions with
the existing `COMAPI` and `DACOM_CDECL` portability macros. Those macros still expand to
`__stdcall` and `__cdecl` under MSVC and to no decoration for the current non-MSVC native ABI. The
`IHeap` method order, parameter types, return types, descriptor fields, and `U32` metadata remain
unchanged.

## Ownership and initialization

`NativeDacomBootstrap.cpp` constructs one process-lifetime `NativeComponentManager`. During that
construction it registers the native heap first and both production profile-parser factories
afterward. `InitializeDAHeap` obtains this same manager through `DACOM_Acquire`, rejects negative or
LP64 values wider than `U32`, and submits a normal `DAHEAPDESC`. The manager therefore resolves the
heap through the same factory path used by other DACOM services instead of assigning an unrelated
test singleton.

The native heap is deliberately process-lived. `AddRef` and `Release` both return one, matching the
other application-global native owners. Repeated `InitializeDAHeap` calls retain the same object,
update the last flag word, and add the requested initial size to a saturating `U32` diagnostic
capacity. The `growSize` field is accepted and preserved at the interface boundary but does not
currently drive an arena-growth algorithm.

## Implemented behavior

The implementation uses the platform C allocator for bytes and a synchronized ordered map for
native-only bookkeeping. It provides:

- allocation, byte-filled allocation, reallocation, and release;
- exact ownership checks that reject null and foreign pointers without freeing them;
- block size, message, and fixed-width owner metadata;
- default allocation messages and per-block message replacement;
- stable snapshot enumeration with `DAHEAPFLAG_ALLOCATED_BLOCK`;
- stored error-handler accessors;
- last-create flags, configured size, allocated-byte accounting, and diagnostic available/largest
  capacity; and
- one shared object returned through factory creation, `HEAP_Acquire`, and the public `HEAP`
  variable.

Bookkeeping is protected by a recursive mutex because an enumeration callback is allowed to call
back into `IHeap`. Reallocation extracts the existing map node before calling `realloc`, so a moved
allocation can be re-keyed without allocating a second bookkeeping node. If allocation or metadata
construction fails, the operation returns failure and retains the previous tracked state.

`GetAvailableMemory` and `GetLargestBlock` are diagnostics, not allocation limits. The allocator
does not refuse a request merely because it exceeds the configured descriptor size, and it does
not model fragmentation. This distinction prevents the synthetic metric from being presented as
the Win32 virtual-memory heap's capacity behavior.

## Explicitly unsupported behavior

The native owner does not reproduce the COMHeap assembly and Win32 virtual-memory implementation:

- the private `malloc_pass_through`, `realloc_pass_through`, and `calloc_pass_through` vtable slots
  return null and are not native public entry points;
- debug-fill, best-fit, verification-disable, private-heap, and expansion-message flags are recorded
  but do not alter allocation behavior;
- `HeapMinimize` is a no-op;
- the stored error handler is not yet invoked for allocation failures or invalid pointers;
- no guard pages, corruption scan, allocation-site return address, or Win32 lock primitive is
  emulated; and
- only the production EventSystem ELF has a measured host-heap allocation/release closure; other
  components still require owner-specific cross-module proof.

Those differences are acceptance gates, not silent compatibility claims. Persistent files, saves,
maps, campaign ABI, and network layouts are unaffected; heap pointers and bookkeeping remain
process-local.

## Validation evidence

`frontierx_native_heap_probe` exercises public bootstrap rather than constructing the owner
directly. It verifies shared identity, descriptor flags and size, zero-filled allocation, data
preservation across reallocation, block metadata, enumeration, foreign-pointer rejection,
diagnostic accounting, release, and repeated initialization. The global DACOM probe separately
proves that profile creation still works through the same owner, and the native-shell process test
checks that startup and shutdown remain intact.

The static EventSystem probe separately enumerates heap blocks around real aggregate creation,
queue use and final destruction. Its exact baseline is restored. The SDL legacy-input integration
and native shell link the same heap-enabled static owner, so the fallback no longer silently uses a
different C-allocation family from the configured ELF owner.

The dynamic-library probe initializes the host heap, loads the production EventSystem ELF with
`RTLD_NOW | RTLD_LOCAL`, creates its real aggregate, posts a message that allocates a queue node,
and observes both allocations through the host's `GetAvailableMemory`. After all interfaces and
the aggregate are released, the exact pre-creation capacity is restored before `dlclose`. The ELF
intentionally leaves `HEAP` undefined for host resolution. Both native hosts export their heap
symbol through CMake's executable-export contract.

Because the native shell also carries a static EventSystem fallback, loading the same production
source a second time originally exposed duplicate RTTI globals under AddressSanitizer. The module
now uses an ELF version script: only `FrontierXRegisterComponents` and
`FrontierXCanUnloadComponents` are global definitions, while all C++ implementation, RTTI and ASan
ODR indicators are local. The undefined `HEAP` reference remains global and resolves to the host.
This is part of the tested module ABI, not merely symbol-table tidying.

The focused GCC and Clang builds pass with strict warnings. The heap, global bootstrap, and native
shell also pass AddressSanitizer and UndefinedBehaviorSanitizer with leak detection disabled because
LeakSanitizer cannot operate under this environment's process tracing. These are synthetic native
tests; no retail installation was read and no native game executable was run.

```text
cmake --build --preset linux-gcc-debug --target \
  frontierx_native_heap_probe frontierx_native_dacom_bootstrap_probe frontierx_native_shell
out/build/linux-gcc-debug/frontierx_native_heap_probe
ctest --test-dir out/build/linux-gcc-debug \
  -R 'portability\.(native_heap|native_dacom_bootstrap|native_shell)$' --output-on-failure

cmake --build --preset linux-clang-debug --target \
  frontierx_native_heap_probe frontierx_native_dacom_bootstrap_probe frontierx_native_shell
out/build/linux-clang-debug/frontierx_native_heap_probe
ctest --test-dir out/build/linux-clang-debug \
  -R 'portability\.(native_heap|native_dacom_bootstrap|native_shell)$' --output-on-failure
```

## Remaining acceptance gates

1. Repeat the allocation/release and hidden-C++-symbol proof for the next production ELF owner;
   do not generalize the EventSystem result to callbacks, threads, or different destruction paths.
2. Decide whether native production requires descriptor capacity enforcement, a dedicated arena,
   debug fills, corruption checking, and error-handler callbacks; encode each accepted behavior in
   tests rather than inferring it from the Win32 implementation.
3. Replace or prove unnecessary the private COMHeap pass-through assembly contract for every native
   caller before enabling allocator interposition.
4. Run clean native Windows Debug and Final rebuilds, ABI/vtable checks, and behavioral comparisons
   for repeated creation, metadata, enumeration, error handling, and cross-module ownership.
5. Connect another dependency-ordered game or renderer owner only after its allocation and
   destruction paths are explicit.

Until these gates pass, the correct claim is a tested native `IHeap` bootstrap with one production
cross-ELF owner—not a complete COMHeap port and not a complete Linux game port.
