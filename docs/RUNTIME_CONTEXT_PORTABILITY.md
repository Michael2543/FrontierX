# Pointer-Width Runtime Contexts and Callbacks

Status: source migrations with isolated Linux probes, 2026-09-17

This document records the remaining high-confidence pointer truncations identified by the
production inventory after the DOSFile, DACOM, and stack-guard work. The affected values are
process-local callback contexts and OS message parameters. They are not persistent IDs, disk
offsets, network fields, or campaign ABI data.

## Particle callback context

The particle manager previously accepted a `DWORD context` through `IParticleInstance`, retained
it in `ParticleSystem`, passed it to `IGamePositionCallback`, and converted it back to an
`EffectInstance*` in five callbacks. `EffectPlayer` initialized that field with `(DWORD)this`.
This is a real cross-module pointer contract rather than a numeric gameplay ID.

`FrontierX/ParticleContext.h` now defines a `std::uintptr_t` runtime context with explicit pointer
encode/decode helpers. The type is used consistently by:

- `IParticleInstance::SetContext` and `Initialize`;
- `ParticleSystem` storage and `GetContext`;
- every `IGamePositionCallback` method; and
- the `EffectPlayer` producer and callback consumers.

Other `DWORD`/`U32` effect target contexts and serialized effect fields were not widened. On the
supported Win32 build the new type remains 32 bits and preserves the virtual call width and vtable
slot order. A native Windows rebuild and cross-DLL runtime test are still required because this is
a source-level public interface change.

The standalone particle-context probe checks pointer identity, null handling, and exact pointer
width under GCC and Clang. A focused AddressSanitizer/UndefinedBehaviorSanitizer run also passed.

## RichEdit stream callback

`DumpView` previously stored `this` in `EDITSTREAM::dwCookie` through `DWORD`, then copied the low
word of a non-static member-function pointer into `pfnCallback` with x86 inline assembly. The
implementation now uses the API's intended contract:

- `dwCookie` receives `reinterpret_cast<DWORD_PTR>(this)`;
- `pfnCallback` points to a type-checked static callback; and
- the callback recovers `DumpView*` from the pointer-width cookie before accessing stream state.

The edit stream is consumed synchronously by `EM_STREAMIN`, so the existing owner lifetime is
unchanged. This removes both the pointer truncation and the member-function-pointer assembly trick.
The code is Windows-owned and cannot be runtime-tested by the Linux graph; the source regression
asserts the exact callback shape until a portable UI owner exists.

## DirectShow notification context

`IMediaEventEx::SetNotifyWindow` defines its instance-data argument as `LONG_PTR`, but `DSStream`
previously supplied `(LONG)this`. The call now uses `reinterpret_cast<LONG_PTR>(this)`, matching
the DirectShow API while leaving the message ID and later `LPARAM` delivery unchanged. DirectShow
remains a Windows-only video path that will eventually be replaced by the documented FFmpeg
backend; this correction keeps the supported Windows path pointer-safe in the meantime.

## Hotkey and legacy-message ABI

The hotkey interface documented a `JOYINFOEX*` in `lParam` while declaring the window, `wParam`, and
`lParam` values as `S32`. The implementation also narrowed `MSG` fields and the pointer passed to
`IEventCallback::Notify`. Fixing only `Modal.cpp` would have hidden the same truncation inside
DAHOTKEY, so the owning message boundary was migrated as one unit.

`FrontierX/LegacyMessage.h` defines:

- `LegacyWindowToken` and `LegacyWParam` as `std::uintptr_t`;
- `LegacyLParam` as `std::intptr_t`; and
- `LegacyMessageId` as fixed-width `std::uint32_t`.

The types now flow through `IHotkeyEvent::SystemMessage`, `IEventMessageFilter`, `HotkeyEvent`,
`HKManager`, the menu/cursor/ruse callers, the joystick debug path, and the HotkeyEditor caller.
Pointer conversion helpers cover window tokens and synchronous pointer payloads. The SDL legacy
input adapter uses the same aliases, making its already-tested compatibility record match the
eventual DAHOTKEY connection boundary rather than a parallel near-copy.

On Win32 the pointer-sized fields remain four bytes, and no vtable slot was added or reordered. On
x86_64 they can carry the complete address. This is a runtime ABI change shared by Trim, Conquest,
DAHOTKEY and tools; clean native Windows builds and behavioral/ABI checks are mandatory before
acceptance.

## Evidence and current inventory

The focused particle-context and legacy-input checks build and execute under GCC and Clang on Linux
x86_64. The particle-context probe executes independently; the legacy-input probe additionally verifies
pointer identity and the widths of window, `wParam`, and `lParam`. Production source regressions
check the particle producer/consumer contract, RichEdit callback shape, DirectShow `LONG_PTR`, and
hotkey pointer-width types.

The current 2026-09-19 lexical inventory now reports:

- 1,266 scanned C/C++ production source and header files;
- 1,318 general legacy-integer casts in 230 files;
- zero high-confidence object/address-to-legacy-integer casts;
- zero measured encoded opaque-handle casts; and
- 441 inline-assembly matches in 18 files.

Zero matches means only that the measured high-confidence lexical patterns are absent. It does not
prove that every width-sensitive expression has been found, that the Windows modules compile, or
that a Linux game runs. General integer casts, native `long`, packed layouts, callbacks, and
unmeasured implicit conversions still require owner-by-owner review.

## Required acceptance work

- Run clean native VS2022 Win32 Debug and Final rebuilds for Conquest, Trim, DAHOTKEY,
  ParticleEffect and all affected consumers.
- Run the parameterized ABI/layout checks against fresh PE outputs and explicitly compare the
  affected interface/vtable surfaces.
- Exercise particle creation, transform/parameter callbacks, mesh lookup, completion and teardown
  against representative retail effects.
- Exercise DumpView `EM_STREAMIN`, DirectShow notification delivery, normal keyboard/mouse input,
  debug joystick input, menu Alt-release synthesis, and hotkey filters.
- Connect the already-tested SDL legacy-message record to the portable event/DACOM adapter only
  after callback ownership and synchronous payload lifetime are characterized.
- Continue the broader inventory: zero in this narrow category is not a substitute for compiling a
  real engine owner or for the remaining platform, renderer, audio, video, and networking work.

No retail media was inspected, copied, or added for this migration.
