# Native Dynamic-Library and DACOM Module Boundary

## Scope

`FrontierX::DynamicLibrary` is the narrow Linux ELF loading boundary in the CMake graph. Two
existing engine areas require this capability: DACOM loads component providers, while the
campaign/script path loads mission modules. `FrontierX::NativeComponentManager` now consumes the
boundary for explicitly adapted native DACOM modules. Campaigns, renderer modules and diagnostic
loaders are not consumers of this slice.

The generic boundary provides ownership and exact symbol lookup. It does not decide module naming,
search paths or ABI compatibility. DACOM adds its own registration and unload policy above that
boundary; a PE DLL is not thereby an ELF counterpart.

## Generic loader contract

The class is move-only and owns at most one local module handle. On Linux it opens with
`RTLD_NOW | RTLD_LOCAL`: undefined symbols fail at open time, and module symbols are not injected
into the global resolution scope. A second `Open` on an owned handle fails instead of silently
unloading or replacing it. `Close` is explicit and idempotent; destruction closes any remaining
handle.

`Symbol` requires an open library and a nonempty, exact symbol name. It clears stale `dlerror`
state before lookup and checks the post-lookup error rather than assuming a null address alone is
an error. Failed open and lookup operations leave ownership unchanged. Error text is returned to
the caller; the loader does not print paths or diagnostics itself.

The interface returns a raw symbol address because converting it to a function type is ABI- and
owner-specific. The probe performs the POSIX-supported conversion through `memcpy` only after
checking pointer sizes on the supported Linux x86_64 target. Production owners must define and
test their precise exported entry-point types before calling a symbol.

`Abandon` is a narrow failure-safety operation used only by manager destruction. If a module
reports that objects are still live, the manager deliberately leaves the ELF mapped rather than
run `dlclose` and invalidate their vtables. This leaks the loader reference until process exit; it
is not a normal unload path.

## Native DACOM module contract

An adapted native component module exports these exact C symbol names:

```text
GENRESULT FrontierXRegisterComponents(ICOManager *manager)
BOOL32 FrontierXCanUnloadComponents()
```

The symbol names have C linkage, but the registration parameter is the existing C++ `ICOManager`
interface. The contract is therefore limited to modules built for the same compiler/toolchain ABI;
it is not a stable C ABI and is not suitable for retail campaign DLLs.

`AddLibrary` uses local eager loading, requires both symbols, attributes every registration made
during the registration call to a unique module ID, and rolls back those registrations if the call
fails, throws, or succeeds without registering a factory. The manager rejects recursive module
loading during registration. A repeated add of the same exact filename increments a manager
reference count without registering twice.

`RemoveLibrary` balances that reference count. On final removal it first asks the module whether
any module-owned instances or other executable references remain. A false response leaves the
module and registrations intact. A true response causes reverse registration removal while the ELF
is still mapped, followed by `dlclose`. `ShutDown` preflights every module before changing state,
releases all registrations in reverse order, and closes modules in reverse load order.

The module owns the accuracy of `FrontierXCanUnloadComponents`: it must cover instances, callbacks,
threads and every other reference that may execute module code. The manager cannot infer these
lifetimes from the legacy `IDAComponent` interface. Loading/removal is a single-threaded bootstrap
contract. Filenames are compared exactly; canonicalization, case folding, wildcard discovery and
Windows DACOM INI search order are not implemented.

## Automated evidence

CMake builds `frontierx_dynamic_library_fixture` as a small ELF shared object exporting a plain C
value function and the native DACOM entry points. `frontierx_dynamic_library_probe` verifies:

- null and nonexistent paths fail without acquiring ownership;
- lookup while closed and lookup with an empty name fail;
- the fixture opens locally and cannot be replaced by a second open;
- its exact exported symbol resolves and returns the expected value;
- a missing symbol fails without closing the module;
- move construction and move assignment transfer unique ownership;
- explicit and repeated close operations succeed;
- missing/null modules fail without changing manager state;
- a module factory is attributed once across repeated adds and creates a real DACOM object;
- one remove balances one repeated add without unloading;
- final removal and manager shutdown refuse to unload while a fixture object is alive;
- manager destruction pins a busy module so an escaped object's final `Release` remains valid;
- successful final removal makes the factory unavailable; and
- the same ELF can be loaded again after a complete unregister/close cycle.

GCC and Clang build and pass the registered test with strict warnings. A focused Clang build of the
fixture, loader, manager and probe also passes AddressSanitizer and UndefinedBehaviorSanitizer with
leak detection disabled because LeakSanitizer cannot operate under this environment's process
tracing.

Reproduce the registered checks:

```text
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug --target frontierx_dynamic_library_probe
ctest --test-dir out/build/linux-gcc-debug -R portability.dynamic_library --output-on-failure

cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug --target frontierx_dynamic_library_probe
ctest --test-dir out/build/linux-clang-debug -R portability.dynamic_library --output-on-failure
```

## Remaining DACOM integration gates

- Adapt one real production component module to the explicit entry points and implement a complete
  module-owned liveness count; the current evidence uses a synthetic fixture only.
- Define deterministic filename/search ordering and `[Libraries]` bootstrap without copying
  Windows case-insensitive wildcard behavior blindly.
- Select and validate cross-module heap/allocation ownership before returning production objects
  across ELF boundaries.
- Exercise dependency order, a failing multi-registration module, close failure and concurrent-use
  rejection before broadening the loader.
- Compare reference balancing, registration failure and shutdown behavior with fresh native
  Windows DACOM evidence.

## Campaign integration gates

Retail campaign modules are ABI-linked to application types and globals. A successful `dlopen`
does not prove that an ELF campaign module is compatible. The campaign owner must separately define
its C export surface, shared structure/vtable layout, global-state boundary, build graph and unload
lifetime. All 18 shipped campaign modules and affected `Conquest`, `Mission`, `Globals`, and script
targets require coordinated ABI/layout and behavioral checks.

No existing Win32 `LoadLibrary` call has been replaced by this slice, and no production ELF
component, retail module or retail data was loaded.
