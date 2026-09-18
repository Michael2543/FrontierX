# Native DACOM Manager

## Purpose and boundary

`FrontierX::NativeComponentManager` is a platform-neutral implementation of the existing
`ICOManager` interface. It hosts statically registered production `EventSystem` and profile-parser
factories in the native graph and now loads explicitly adapted ELF component modules through the
separate dynamic-library boundary.

This manager still does not scan paths, parse a DACOM INI file, provide the legacy DA heap, or
replace the application-global `DACOM_Acquire` entry point. Valid `SetINIConfig` requests return
`GR_NOT_IMPLEMENTED`; null and mutually incompatible arguments retain explicit invalid-parameter
results. Dynamic loading requires the explicit FrontierX module contract and does not execute
legacy Windows `DllMain` registration.

## Preserved manager semantics

The implementation was derived from the registration, creation, enumeration and shutdown paths in
`Code/Libs/Src/DACOM/DACOM.CPP`:

- registrations retain the caller's stable interface-name pointer and take one factory reference;
- higher numeric priorities are visited first;
- a new equal-priority registration is inserted before older entries, matching the legacy
  `priority >= existing.priority` rule;
- creation tries matching factories in that order and continues until one returns `GR_OK`;
- enumeration filters by exact case-sensitive interface name and stops when its callback returns
  false;
- unregistration searches from the end and releases the removed factory reference; and
- shutdown releases all remaining factories in reverse manager order.

Dynamic registrations carry a manager-local module ID. Failed registration is transactional,
duplicate exact-path adds are reference-counted, and a final remove releases attributed factories
before closing the ELF. The required module liveness callback prevents normal remove/shutdown while
module-owned objects remain. If the manager itself is destroyed in that invalid state, it pins the
ELF until process exit rather than invalidating live vtables.

The manager itself has process/owner lifetime, like the historical application-global manager;
`AddRef` and `Release` both return one and never delete it. `CreateInstance` with a null descriptor,
a null requested name, or `ICOManager` returns the manager. The native implementation does not own
or copy interface-name strings, preserving the existing registration lifetime requirement.

The registration vector converts allocation failure into `GR_OUT_OF_SPACE` rather than exposing an
exception. Component instances, callback parameters and factory pointers remain native process
references and are never serialized.

## Executed integration

`frontierx_dacom_core` builds the manager with strict warnings. The existing
`frontierx_event_system_probe` no longer uses a test-only fake manager. It now:

1. registers deliberately rejecting high- and equal-priority factories;
2. registers the production event-system factory through `RegisterTheEventSystem`;
3. verifies filtered enumeration, callback stop and equal-priority insertion order;
4. requests `IEventSystem`, proving fallback through both rejecting factories to production;
5. exercises the real aggregate, interfaces, connection point, queue and direct send;
6. unregisters the test factory and verifies its held reference is released; and
7. shuts down the manager and verifies that all registrations are removed.

GCC and Clang pass this integration through the registered CTest. A focused combined build of the
manager, production event owner and probe passes AddressSanitizer and UndefinedBehaviorSanitizer
with leak detection disabled because LeakSanitizer cannot operate under this environment's process
tracing. Production source regression checks ensure the event probe continues to use this manager
instead of silently returning to a fake.

## Why automation dispatch is separate

The x86 assembly in `Code/Libs/Include/tdispatch.h` implements late-bound, one-argument automation
calls. Current source use is limited to four legacy Viewer tool headers. It is not used by
`DACOManager` registration/factory dispatch and did not block this native manager or the production
event service. It still needs an ABI-defined replacement before those Viewer tools can be native,
but it is not the next game-startup manager dependency.

The move-only local ELF owner and native module contract are documented in
[DYNAMIC_LIBRARY_PORTABILITY.md](DYNAMIC_LIBRARY_PORTABILITY.md). The remaining dynamic-manager
blockers are a real production module adaptation, Windows path/search behavior in `DACOM.CPP`,
profile-driven library bootstrap, and the legacy heap providers including `HeapPass.asm` and Win32
virtual-memory/locking behavior.

## Remaining acceptance gates

- Adapt and validate one real production ELF component, including complete module-owned liveness
  accounting and cross-module allocation ownership.
- Connect the existing native profile document to manager bootstrap without changing Windows INI
  behavior or library ordering.
- Select and validate the native heap policy; do not pretend the current standard allocator is the
  legacy DA heap or cross-module ownership proof.
- Add tests for duplicate registration of the same factory/name, mutation during enumeration,
  allocation failure and shutdown after multiple real factories before widening use.
- Compile the actual `DACOM.CPP` owner or replace it with an explicitly approved native owner that
  preserves its public interface and measured behavior.
- Run clean native Windows Debug/Final rebuilds, ABI/layout checks and cross-module factory,
  callback, heap and unload comparisons.

This target does not create a game executable, load a campaign module, inspect retail data, or
establish binary compatibility between PE DLLs and ELF shared objects.
