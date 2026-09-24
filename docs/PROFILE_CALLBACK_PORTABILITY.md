# Profile Callback and Section-Handle Portability

Status: production parser owner and shared parser-core integration, 2026-09-18

This document records the portable callback slice shared by the production DACOM profile parser
and the native startup configuration reader. It is source-review, compilation, synthetic-execution,
and sanitizer evidence. The production parser now builds as a native static DACOM service. The
native manager also loads the production EventSystem ELF and is owned by the process-global
`DACOM_Acquire` bootstrap. A native `IHeap` now exists and the EventSystem ELF proves one
cross-image owner, but the profile service itself has no dynamic-module proof and no native game
owner exists.

## Owning path

The production path is:

1. `DACOManager::SetINIConfig` selects file, writable-file, or caller-buffer access;
2. `ProfileParser::Initialize` reads or copies the bytes, adds sentinel padding, and calls
   `FrontierX::ProfileBuffer::MarkSections`;
3. `IProfileParser::EnumerateSections` invokes a function callback;
4. `IProfileParser2::EnumerateSections` and `EnumerateKeys` invoke `IProfileCallback`; and
5. callers use the callback-provided section name with `CreateSection` when duplicate section
   names must be distinguished.

`FrontierX::ProfileDocument` uses the same marked-buffer representation for native, read-only
startup configuration. Its `Lines` view now reproduces raw line navigation for native
`SetINIConfig` without exposing DACOM interfaces or borrowed section handles.

## Shared callback contract

`FrontierX/ProfileBuffer.h` now owns the two callback walks in addition to line/section navigation,
key lookup, bounded copying, and section marking.

Section enumeration:

- preserves source order and duplicate section names;
- passes the marked section name and the bytes immediately following the closing bracket;
- returns false for a null storage pointer;
- returns true when an initialized document contains no sections;
- stops immediately and returns false when the callback returns false; and
- never retains the borrowed pointers beyond the synchronous call.

Key enumeration:

- begins after the selected marked section and stops at its boundary;
- preserves source order and duplicate keys;
- trims trailing non-symbol characters from the key while retaining its leading spelling;
- ignores entries whose key field contains `;` or a newline;
- skips ASCII spaces, but not tabs, after `=`;
- strips CRLF from the value;
- retains an empty value as an empty callback string;
- caps callback key and value strings at 255 bytes plus a terminator;
- returns false when no valid key was visited; and
- stops immediately and returns false when the callback returns false.

The focused tests cover successful completion, early stop, an empty document, a section suffix,
comment rejection, punctuation trimming, the 255-byte key/value cap, an empty value, callback
order, and a final key/value without a trailing newline. That final case fixes a source-reviewed
null-pointer bug in the legacy
implementation: its no-newline branch called `strlen` on the null result of `strchr`.

`ProfileDocument::Sections` and `ProfileDocument::Keys` consume these shared walks, while `Lines`
uses the same `GetLine` and `CopyLine` contract as production `ReadProfileLine`. This gives the
portable core multiple concrete owners and prevents native startup and library configuration from
silently diverging from DACOM traversal.

## Production native owner

`frontierx_profile_parser` compiles the real `ProfileParser.cpp`, its `DAComponentAggregate`
factories, inbound interface maps and `IProfileParser`/`IProfileParser2` implementations. Windows
continues to use its existing `CreateFile`/`ReadFile` branch. The non-Windows branch uses binary
standard-library file I/O, rejects sizes that cannot fit the established `U32` buffer contract,
allocates the same leading byte plus seven sentinel bytes, and changes the active document only
after a complete read. A failed native file load therefore retains the prior parser state.

Native case-insensitive matching is explicit ASCII folding, avoiding locale-dependent behavior.
The symbol predicate remains ASCII letters, digits and underscore, matching the measured
`__iscsym` consumer contract without relying on an MSVC extension. Private `__fastcall` decoration
remains on MSVC and is empty elsewhere; no public interface signature changed.

`IProfileParser2::Initialize2` previously encoded a caller buffer size into an out-of-range
`IProfileParser::ACCESS` enum and called `Initialize`. UndefinedBehaviorSanitizer detected the
invalid enum value on Linux. It now calls a typed internal buffer initializer, validates null plus
nonzero-size input, retains the same copied/sentinel representation and avoids changing the public
vtable. The disabled DA-heap allocation macros now qualify their C allocation functions globally,
so a component method named `free` cannot hide the matching deallocator.

## Pointer-width section handles

DACOM section handles are runtime byte offsets encoded in the existing `void *`-shaped `HANDLE`.
They are neither native operating-system handles nor persistent file fields. The legacy
implementation decoded them through `U32` in line lookup, key lookup, and key enumeration, which
would truncate the encoded value on a 64-bit host.

`ProfileParser.cpp` now uses `FrontierX::EncodeHandleValue` and
`FrontierX::DecodeEncodedHandle`. On Win32 the representation and public interface width remain
unchanged. On Linux x86_64 the runtime offset can use the full pointer-width carrier. No disk,
save, campaign, or network schema was widened.

The public `IProfileParser.h` include now matches the on-disk `DACOM.H` spelling. Its callback and
virtual method declarations use `COMAPI`, which expands to the existing MSVC calling convention on
Windows and to an empty portable decoration elsewhere. The native DACOM interface probe compiles
the real header and invokes an `IProfileCallback` implementation with a pointer context.

## Evidence

### Option defaults without a parser interface (2026-09-23)

The production `IProfileParser_Utility.h` option helpers are used by the App-side
`RenderBatcher::init`. Source review and the linked synthetic batcher process showed that
`opt_get_u32` returned an uninitialized local when `ICOManager::QueryInterface` did not provide
`IProfileParser`; the float, string and vector helpers had the same conditional-default pattern.
Each helper now writes its supplied default before the interface lookup. The numeric helpers also
use a character terminator (`'\0'`) when truncating parsed text. The existing
`frontierx_render_batcher_runtime_probe` checks all four defaults with the real native manager and
no parser interface. GCC 16.2.1 and Clang 22.1.8 focused builds and CTests, plus the focused
Clang ASan/UBSan process, passed on Linux x86_64 using synthetic values. This is not a Windows
runtime comparison or a test of parsing a present key. Native Win32 Debug/Final rebuilds and
cross-module behavior checks remain open because the inline helpers are shared with Windows
callers.

The following checks passed on Linux x86_64 on 2026-09-17:

- GCC and Clang strict-warning builds of the profile buffer, profile document, and public DACOM
  interface probes;
- the focused `profile_buffer`, `profile_document`, and `dacom_interface` CTests under both
  presets;
- AddressSanitizer and UndefinedBehaviorSanitizer runs of all three focused paths; and
- source-regression checks, including rejection of reintroduced `U32` section-handle
  decoding and verification that the production parser calls the shared enumerators.

The production-owner probe additionally uses `NativeComponentManager` to register both real
factories, creates the `IProfileParser2` aggregate, queries both parser interfaces, and verifies
typed buffer initialization, callback completion/stop, duplicate sections, case-insensitive
section/key lookup, line and key enumeration, native file loading, writable empty-file creation,
missing-file failure and transactional state retention. GCC and Clang strict-warning builds pass, as does the combined
AddressSanitizer/UndefinedBehaviorSanitizer run.

This slice expands the existing profile probes rather than adding another executable; the focused
GCC, Clang and sanitizer executions above are its component evidence.

## Reproduction

From the repository root:

```text
cmake --build --preset linux-gcc-debug --target \
  frontierx_profile_buffer_probe frontierx_profile_document_probe \
  frontierx_dacom_interface_probe
ctest --preset linux-gcc-debug \
  -R 'profile_(buffer|document)|dacom_interface' --output-on-failure

cmake --build --preset linux-clang-debug --target \
  frontierx_profile_buffer_probe frontierx_profile_document_probe \
  frontierx_dacom_interface_probe
ctest --preset linux-clang-debug \
  -R 'profile_(buffer|document)|dacom_interface' --output-on-failure

PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover \
  -s Tools/portability -p 'test_platform_inventory.py' -v
```

## Remaining acceptance gates

Before treating this as a complete production integration:

- run clean VS2022 Win32 Debug and Final rebuilds of DACOM and every affected consumer;
- compare section/key enumeration, callback stop results, duplicate handling, 255-byte truncation,
  empty values, and final unterminated lines against the supported Windows build;
- exercise callback re-entry through `CreateSection` for duplicate section names;
- validate section-handle behavior with native Windows ABI/layout checks;
- compare native file-open/share/write behavior and ASCII case folding against the Windows owner;
- compare the shared raw-line traversal now used by native `SetINIConfig` with the production
  parser on Windows, then decide whether the manager should aggregate the production parser rather
  than retain the smaller owning-document bootstrap boundary;
- validate DA-heap-enabled allocation ownership on native Windows and decide the Linux shared-heap
  policy before crossing dynamic module boundaries; and
- retain the existing Windows registry-backed `IUserDefaults` behavior until its production
  defaults, read/write timing, value formats, and failure behavior have an explicit adapter.

No retail media was read, and no runtime compatibility with a retail installation is claimed.
