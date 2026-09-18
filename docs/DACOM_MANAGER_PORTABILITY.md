# Native DACOM Manager

## Purpose and boundary

`FrontierX::NativeComponentManager` is a platform-neutral implementation of the existing
`ICOManager` interface. It hosts statically registered production `EventSystem` and profile-parser
factories in the native graph and loads explicitly adapted ELF component modules through the
separate dynamic-library boundary. The production EventSystem is the first non-synthetic module.

This manager still does not scan paths or reproduce the legacy COMHeap implementation. The separate
native bootstrap now exports the application-global `DACOM_Acquire` entry point, owns one manager,
registers one process-local `IHeap` first, and registers both production profile-parser factories
exactly once. Its read-only `SetINIConfig` path parses an
explicit file or caller string and applies an ordered `[Libraries]` list. Writable configuration
returns `GR_NOT_IMPLEMENTED`; null, unknown and mutually incompatible flags retain explicit
invalid-parameter results. Dynamic loading requires the explicit FrontierX module contract and
does not execute legacy Windows `DllMain` registration.

## Native configuration policy

The native manager reuses the bounded owning `ProfileDocument` parser. It accepts at most 1 MiB of
ASCII without embedded NUL bytes and compares the `Libraries` section name with locale-independent
ASCII folding. Raw lines preserve the production parser's section navigation and CR/LF boundary
behavior. Blank lines and lines beginning with `#` or `;` after leading spaces are ignored. A
leading `-` removes a module. Quoted filenames may contain spaces; unquoted filenames end at the
first space, tab or carriage return, matching the source-reviewed `parseFilename` rule.

File-based relative module names resolve against the configuration file's directory. String-based
relative names resolve through the process working directory. Paths are normalized lexically but
not canonicalized or case-folded. Every entry must name one exact module: native configuration
does not copy the legacy missing-section `*.dll` scan, the `DACOM` environment search list, or
Windows wildcard ordering. A missing `[Libraries]` section therefore returns `GR_DATA_NOT_FOUND`.

Configuration operations are applied in source order. If one add/remove fails, already applied
operations are reversed before the error is returned. A successfully applied document is not
applied twice. Successful manager shutdown resets that state so a later bootstrap can configure
again. Writable-file semantics remain unimplemented because pretending that read-only loading is
writable would violate the public contract.

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
The same fail-safe applies when module registration fails after creating live executable state.

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

`frontierx_native_dacom_bootstrap_probe` calls the public `DACOM_Acquire` symbol twice, proves
pointer identity, queries `ICOManager`, enumerates one factory for each production profile
interface, then creates and executes a real `IProfileParser2` aggregate through that global owner.
The native shell now acquires this same owner rather than constructing a parallel local manager.
GCC, Clang and focused AddressSanitizer/UndefinedBehaviorSanitizer executions pass.

The dynamic-library probe additionally covers string and file configuration, quoted paths,
configuration-relative resolution, repeated configuration, removal directives, malformed quotes,
multi-module rollback, shutdown/reconfiguration and real instance creation through the configured
fixture factory. It also creates the production EventSystem from a configured ELF, executes its
queue, rejects unload while that aggregate lives, then unloads after final release.

## Why automation dispatch is separate

The x86 assembly in `Code/Libs/Include/tdispatch.h` implements late-bound, one-argument automation
calls. Current source use is limited to four legacy Viewer tool headers. It is not used by
`DACOManager` registration/factory dispatch and did not block this native manager or the production
event service. It still needs an ABI-defined replacement before those Viewer tools can be native,
but it is not the next game-startup manager dependency.

The move-only local ELF owner and native module contract are documented in
[DYNAMIC_LIBRARY_PORTABILITY.md](DYNAMIC_LIBRARY_PORTABILITY.md). The process-local heap contract
and its explicit differences from COMHeap are documented in [NATIVE_HEAP.md](NATIVE_HEAP.md). The
remaining dynamic-manager blockers are further dependency-ordered production modules, Windows
path/search behavior in `DACOM.CPP`, cross-ELF allocation ownership, and the unimplemented legacy
behavior in `HeapPass.asm` and the Win32 virtual-memory/locking provider.

## Remaining acceptance gates

- Adapt the next dependency-ordered production ELF component, including its callbacks, threads and
  allocation ownership rather than assuming the EventSystem's instance counter is universal.
- Connect additional native game-startup owners through the tested global manager without changing
  the supported Windows INI behavior or ordering.
- Validate allocation and destruction through a production ELF boundary; the process-local native
  heap is neither the legacy COMHeap implementation nor cross-module ownership proof.
- Add tests for duplicate registration of the same factory/name, mutation during enumeration,
  allocation failure and shutdown after multiple real factories before widening use.
- Compile the actual `DACOM.CPP` owner or replace it with an explicitly approved native owner that
  preserves its public interface and measured behavior.
- Run clean native Windows Debug/Final rebuilds, ABI/layout checks and cross-module factory,
  callback, heap and unload comparisons.

This target does not create a game executable, load a campaign module, inspect retail data, or
establish binary compatibility between PE DLLs and ELF shared objects.
