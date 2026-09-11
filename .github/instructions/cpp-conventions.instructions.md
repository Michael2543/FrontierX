---
description: "Use when editing FrontierX C or C++ code. Covers platform isolation, retail layouts, 64-bit safety, ABI boundaries, determinism, and legacy ownership rules."
applyTo: "**/*.{c,cc,cpp,cxx,h,hh,hpp,hxx,inl}"
---

# FrontierX C and C++ Conventions

- Preserve the local style of the owning module. Avoid unrelated formatting and modernization.
- Keep new code portable C++ unless it is inside an explicit platform or renderer backend.
- Do not add direct Win32, POSIX, Linux, X11, Wayland, or Vulkan calls to game logic.
- Prefer adapters around existing engine interfaces over replacing call sites throughout the game.
- Match include-path spelling exactly to the on-disk filename.

## Layout and Pointer Width

- Before changing a struct, determine whether it is serialized, parsed from retail data, used in a
  save or packet, generated into schema data, or shared across DLL/module boundaries.
- Preserve persistent field widths and enum values. Do not replace a 32-bit retail field with
  `size_t`, `long`, or a native pointer.
- Use fixed-width integers for new disk/wire structures and add `sizeof`, `offsetof`, and final-field
  end checks where layout is contractual.
- Scope packing with push/pop directives. Never change ambient packing for following headers.
- Never cast a pointer or interface address to `U32`, `S32`, or `DWORD`. Use a native pointer,
  `uintptr_t`, or a typed handle table, then keep that runtime representation out of serialized
  layouts.
- Audit callback signatures, varargs, pointer arithmetic, and `long` whenever code moves from Win32
  to LP64 Linux.

## ABI and Ownership

- Rebuild and validate every affected module when changing a public interface or shared type used
  by `Conquest`, `Mission`, `Trim`, `Globals`, `ZBatcher`, or campaign scripts.
- Do not assume successful linking proves ABI compatibility.
- Preserve DACOM/COMHeap allocation ownership. Do not switch allocation families, CRT ownership, or
  cross-module destruction paths as incidental cleanup.
- Keep COM-style `AddRef`/`Release` behavior and engine ownership explicit when introducing RAII.

## Determinism

- Do not enable `-ffast-math`, `/fp:fast`, FMA contraction, or unordered parallel simulation work.
- Do not change random-number calls, update ordering, timing quantization, or container iteration in
  a platform-only change.
- Guard division and float-to-integer conversions when NaN, infinity, or zero denominators could
  affect simulation flow.
- Do not copy GeneralsX-specific deterministic math wrappers. Add FrontierX math policy only after
  cross-build evidence identifies an actual divergence.

Keep comments succinct and in English. Explain only non-obvious compatibility, ownership, or
platform constraints.