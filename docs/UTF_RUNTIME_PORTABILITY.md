# UTF Runtime-Handle and Serialized-Call Portability

Status: source migration completed on 2026-09-17; native Windows validation pending

This note covers the runtime-only portability changes in `UTF.cpp` and `UTFShare.cpp`. It does not
change the persistent UTF 1.01 schema, widen directory fields, or claim that the Windows DOSFile
implementation has a native Linux replacement. The Linux UTF inspector remains a separate,
read-only implementation.

## Encoded handle model

Both UTF implementations expose opaque `HANDLE` values that actually identify slots in linked
groups of four runtime records. Find handles are one-based; child handle zero identifies the root.
The read-only implementation additionally sets bit `0x40000000` on mapping tokens. These values are
process-local indices, not native OS handles and not serialized file fields.

The old lookup code narrowed opaque values to `U32`, `DWORD`, or signed `LONG`. Mapping creation
also returned a real native parent mapping handle through the serialized method's `LONG` result,
even though the caller used the `hMapping` member instead. On a 64-bit Windows build that return
would truncate a real handle.

The implementations now decode and encode tokens with `FrontierX::EncodedHandleValue`, backed by
`std::uintptr_t`. Find lookup explicitly rejects zero before subtracting its one-based bias.
Mapping flags are applied and removed at pointer width. The serialized mapping callback returns
only success/failure; the real mapping handle remains native-width in `hMapping`.

## Explicit serialized-call contexts

Source review found a broader x86-only assumption in both production UTF paths. Several public
methods passed the address of their first argument to `SerialCall`; the callback then read later
arguments by indexing forward through that address. This assumes adjacent stack parameters and is
invalid for register-based x86_64 calling conventions.

Explicit context objects now carry:

- read and write arguments in both UTF implementations;
- SharedUTF file-size and get/set-time arguments;
- SharedUTF find-first and find-next arguments;
- SharedUTF move and set-attributes arguments.

The same audit found that the base `DOSFileSystem::SetFilePointer` path already had a correct
`SEEK_STRUCT`, but left it commented out in favor of the adjacent-parameter shortcut. The method
now uses that existing structure. A repository-wide search finds no remaining DOSFile comment or
callback decode that claims later arguments can be recovered from the first parameter's address.

SharedUTF serial results are converted to opaque handles only after rejecting negative failure
values. Internal loops explicitly encode their numeric slot before calling handle-based helpers.
This keeps the legacy public signatures and serialization order while removing parameter-layout
dependence.

## Evidence and limits

The production inventory reports zero legacy-width encoded-handle casts across `IFF.cpp`,
`UTF.cpp`, and `UTFShare.cpp`; a focused source regression test enforces that result. The generic
encoded-handle probe covers pointer-width round trips, direct and offset limits, underflow, and the
invalid sentinel under GCC, Clang, AddressSanitizer, and UndefinedBehaviorSanitizer.

The Linux CMake graph does not compile `Dosfile.cpp`, `UTF.cpp`, or `UTFShare.cpp`. Unlike IFF,
these files are active items in `DOSFile.vcxproj`, so accepting these changes requires clean native
Windows builds of all six DOSFile configurations that matter to the baseline, followed by UTF
behavior tests. Required
coverage includes child and find handle allocation beyond the first four-slot group, invalid and
zero handles, root and child mappings, overlapped and positioned reads/writes, size/time calls,
enumeration, move/attribute operations, sharing failures, and handle exhaustion. Retail runtime
claims still require lawful retail data and the acceptance matrix.
