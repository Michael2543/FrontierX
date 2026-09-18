# Native Event-System Owner

## Scope

`Code/Libs/Src/DAHOTKEY/EventSys.cpp` is the first existing DACOM service owner compiled and
executed by the Linux CMake graph. It is not a replacement implementation: the native target builds
the production `EventSystem` class, its aggregate factory, inbound interface map, outbound
`IEventCallback` connection point, queue, synchronous dispatch and `Peek` implementation.

This slice establishes a real engine-service closure above the previously isolated
`FrontierX::EventQueue` characterization core. It does not link the complete DAHOTKEY module, the
game, SDL input, the DACOM loader or a DA heap provider.

## Source-reviewed contract

The existing component has four externally visible behaviors:

- `Post` appends a fixed-width message and a non-owning native pointer to a pending FIFO;
- `Update` walks that FIFO, including entries appended by callbacks during the same update;
- the batch completed by the most recent update remains visible through `Peek` until the next
  update; and
- `Send` calls subscribers synchronously and never enters the posted-message queue.

The connection point keeps the established 32-bit public subscription handle. The migrated
connection template implements it as a deterministic non-pointer token; event parameters remain
process-local pointers and are never serialized.

## Portability changes

The owner no longer includes `windows.h`; the only former Windows-specific field in this source,
the `Peek` index, is the existing fixed-width `U32`. Direct includes now use the exact on-disk
`DACOM.H` and `SYSTEM.H` spelling required on case-sensitive filesystems.

The constructor explicitly initializes every queue, cursor and initialization member. The legacy
code relied on its class-specific zeroing allocator, which made correctness depend on a hidden
allocation side effect and complicated direct native execution. Explicit initialization preserves
the resulting values without changing object layout.

The class-specific allocation macro now declares standard in-class `operator new` and
`operator delete` functions without an MSVC-only extra class qualification. Its disabled-heap
`DA_HEAP_CALLOC` branch was also completed so non-DA-heap builds have a valid zeroing allocation
expression.

Queue nodes are created by `DA_HEAP_MALLOC`. Their three destruction loops previously used C++
`delete`, which mismatched `malloc` in the portable configuration and bypassed the selected DA heap
in heap-enabled builds. `DA_HEAP_FREE` now pairs with the selected allocator in both configurations.
AddressSanitizer caught the portable mismatch before this correction.

Finally, DACOM outbound member-map offsets now use `FrontierX::MemberOffset` rather than forming a
member address through a null owner pointer. The public offset representation remains
`std::size_t`; no layout or interface signature changed. UndefinedBehaviorSanitizer caught the old
null-member expression while initializing the real event callback map.

## Automated evidence

`frontierx_event_system` compiles the production source as a native static library.
`frontierx_event_system_probe` uses the platform-neutral `NativeComponentManager`, then exercises
the real registration and aggregate factory path. It verifies:

- factory registration and `AGGDESC` creation;
- priority-ordered enumeration and fallback after a higher-priority factory rejects creation;
- `IEventSystem` and `ISystemComponent` queries through the production inbound map;
- aggregate and event-system initialization behavior, including rejection of the unimplemented
  multithreaded flag;
- outbound connection-point lookup, subscription token allocation and removal;
- posted FIFO delivery and non-owning parameter identity;
- delivery of a callback-time post later in the same `Update`;
- completed-batch `Peek` behavior and retirement on an empty update; and
- immediate subscriber delivery through `Send`.

The SDL legacy-input probe additionally creates this owner through `NativeComponentManager` and
uses `DispatchLegacyInputToEventSystem` to deliver resize plus paired immediate-Unicode/character
records to a real `IEventCallback`. This proves synchronous payload lifetime across the adapter and
connection point. The separate `SdlEventPump` integration target now drains an injected SDL batch
through that same path, filters a foreign-window event, preserves resize/text/quit order and reports
shutdown observations. See [SDL_EVENT_PUMP.md](SDL_EVENT_PUMP.md) for its exact contract and limits.

The native manager's scope and dynamic-loader gates are documented separately in
[DACOM_MANAGER_PORTABILITY.md](DACOM_MANAGER_PORTABILITY.md).

GCC and Clang build and pass the registered test with strict warnings. A focused Clang build with
AddressSanitizer and UndefinedBehaviorSanitizer also passes when leak detection is disabled because
LeakSanitizer cannot operate under this environment's process tracing. The separate public DACOM
interface/connection probe also passes the same sanitizers after the member-offset change.

Reproduce the focused checks from the repository root:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_event_system_probe
ctest --test-dir out/build/linux-gcc-debug -R portability.event_system --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_event_system_probe
ctest --test-dir out/build/linux-clang-debug -R portability.event_system --output-on-failure
```

## Remaining gates

This evidence does not establish a native game event loop. The following work remains:

- connect the proven SDL batch pump to the native application owner and isolate or adapt the
  Windows-specific `IWindowManager` lifecycle without posting temporary pointers;
- define the native application owner and update cadence without changing simulation ordering;
- compile the remaining DAHOTKEY hotkey recorder and persistence paths;
- adapt a production component to the native module contract and add profile-bootstrap and heap
  policies; the separate Viewer-only automation dispatch still needs isolation;
- compare callback mutation, aggregation, heap ownership and shutdown behavior with fresh native
  Windows Debug and Final builds; and
- run the required Windows ABI and cross-module checks before accepting parity.

No retail data, renderer, audio, networking or game simulation path is exercised here.
