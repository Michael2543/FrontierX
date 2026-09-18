# IFF Runtime-Handle and Serialized-Call Portability

Status: source migration completed on 2026-09-17; native Windows validation pending

This note records the x86_64 portability work in `Code/Libs/Src/DOSFile/IFF.cpp`. It is source
review and isolated-contract evidence, not proof that DOSFile or the game runs on Linux. The IFF
implementation is still part of the Windows-owned legacy project and is not compiled by the Linux
CMake graph. Source review of `DOSFile.vcxproj` also shows `IFF.cpp` excluded from all six current
Win32 configurations, so an ordinary DOSFile rebuild does not compile this file.

## Problem ownership

IFF keeps fixed-size child, form, and enumeration tables. Its public `HANDLE` values are not native
kernel handles in these paths: they are small runtime table indices encoded in an opaque pointer
type. Mapping tokens use the range beginning at `MAX_CHILD_HANDLES`; the underlying parent mapping
remains a real native handle owned by the parent filesystem.

The old code repeatedly narrowed these tokens through `DWORD` or `LONG`. That happens to preserve
small indices on Win32, but it makes the token contract implicit and mixes it with real OS handles.
The persistent IFF chunk fields remain 32-bit; widening those fields would be a format change and is
not part of this migration.

A second, independent x86 assumption was more serious. `ReadFile`, `WriteFile`, `FindFirstFile`,
and `FindNextFile` passed the address of their first parameter to a serialized callback, whose
implementation then indexed forward as though every other parameter followed contiguously on the
stack. Register-based x86_64 calling conventions provide no such layout. Form indices were also
smuggled through `void *` callback contexts.

## Implemented contract

`FrontierX/EncodedHandle.h` owns pointer-width token conversion through `std::uintptr_t` and offers
two checked operations:

- `TryDecodeHandleIndex` accepts only values below a table limit.
- `TryDecodeOffsetHandleIndex` subtracts a mapping-token base without underflow and then applies
  the same bound.

IFF now uses those operations, or an equivalent pointer-width comparison, before indexing child or
find tables. Child creation and lookup explicitly encode table indices. The serial interface's
legacy `LONG` result carries only the small IFF token or `-1`; it never carries the real parent file
mapping handle. `CreateMapping_S` consequently returns success/failure while the native handle
stays in the `hMapping` member.

Explicit context structures now carry all read, write, and find parameters. Form-name and
form-release callbacks receive pointers to actual `DWORD` context objects. This removes dependence
on adjacent argument storage while preserving the public filesystem signatures and serialized-call
ordering.

The migration also closes two table-safety defects found during review:

- `FindClose` now returns immediately when a find token is out of range instead of continuing to
  index `ffhandles`.
- exhausted child-handle allocation returns the invalid sentinel instead of reserved index zero.

No persistent structure, IFF byte order, chunk alignment rule, form traversal order, or public
DACOM interface was changed.

## Executable evidence

The isolated encoded-handle probe is built with strict warnings by both Linux presets. It covers
pointer-width round trips, ordinary and offset table bounds, underflow rejection, unchanged output
on failure, and the all-bits-one invalid sentinel. On 2026-09-17 these commands passed:

```text
cmake --build --preset linux-gcc-debug --target frontierx_encoded_handle_probe
ctest --preset linux-gcc-debug -R encoded_handle --output-on-failure
cmake --build --preset linux-clang-debug --target frontierx_encoded_handle_probe
ctest --preset linux-clang-debug -R encoded_handle --output-on-failure
```

The production-source inventory now reports zero encoded-handle casts in `IFF.cpp`. The subsequent
UTF/SharedUTF migration reduced the complete DOSFile encoded-handle category to zero. The general
legacy-integer count fell to 1,350 occurrences in 234 files; this is an audit metric, not a count of
confirmed defects. See [UTF_RUNTIME_PORTABILITY.md](UTF_RUNTIME_PORTABILITY.md) for that follow-up.

## Required acceptance work

Before accepting the production integration, first establish whether IFF is a required retail
consumer and add an explicit native Windows compile/test target if it is. A normal current
`DOSFile.vcxproj` build cannot provide that evidence because the file is excluded. The validation
must exercise child open/create/truncate, read/write with and without overlapped state, enumeration,
nested-form release, mapping of root and child data, invalid handles, and table exhaustion against
known retail-compatible data. Run the repository's Windows ABI checks wherever an enabled module
boundary requires them.

A later Linux DOSFile target must test the same behavior through a portable parent file backend.
The read-only UTF inspector and this token probe do not replace that target and do not certify
retail data compatibility.
