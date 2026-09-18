# Third-Party Dependency Record

Status: updated inventory, 2026-09-17

This record covers third-party source used by the native Linux build graph. It does not approve a
release. The unmodified Conquest source license and [LEGAL.md](../LEGAL.md) remain controlling for
the Program Derivative as a whole.

## TinyXML and TinyString

| Field | Verified record |
|---|---|
| Purpose | XML parsing used by the existing `StringTable` implementation |
| Repository path | `Code/Libs/Src/XML/` |
| Imported snapshot | FrontierX baseline revision `f66a257d538a4c476f561b9920a70efc43cd7912` |
| Upstream reference in source | `www.sourceforge.net/projects/tinyxml` and `www.grinninglizard.com` |
| Upstream version or revision | Unknown; no version identifier, import record, tag, or exact upstream revision is present |
| Original attribution | Lee Thomason for TinyXML; Yves Berquin for TinyString, as recorded in the source notices |
| License | Embedded permissive three-clause notices in the six source files; the notices must remain intact |
| Maintenance state | Legacy vendored snapshot with no verified upstream update path |
| Acquisition | Already present in the historical development-source tree; no source was fetched for the Linux target |
| Linux build role | `frontierx_tinyxml` static library and `frontierx_tinyxml_probe` test consumer |
| Existing Windows owner | `Code/Libs/Src/StringTable/StringTable.vcxproj` compiles the same four implementation files |
| Distribution status | Blocked pending exact upstream-version provenance and a package-level license review |

FrontierX modifications on 2026-09-11 remove unused unconditional Windows includes, scope an MSVC
warning pragma, correct LP64 format handling for `unsigned long`, remove meaningless scalar return
qualifiers and null-`this` checks, pair an array allocation with `delete[]`, and remove an unused
parser variable. The modified source files retain their original notices and carry a FrontierX
modification marker.

Compile and automated-check evidence: GCC 16.2.1 and Clang 22.1.8 build the static library with
target-scoped `-Wall -Wextra -Wpedantic -Werror`. The linked CTest parses a representative document,
decodes an entity, preserves attributes, round-trips the LP64 maximum `unsigned long`, and rejects
mismatched closing tags.

## SDL3

| Field | Verified record |
|---|---|
| Purpose | Native Linux window lifetime and event/input backend prototype |
| Upstream | [libsdl.org](https://www.libsdl.org/) and `github.com/libsdl-org/SDL` |
| Local version | 3.4.16, reported by `/usr/lib/pkgconfig/sdl3.pc` and `libSDL3.so.0.4.16` |
| License | SDL's permissive zlib license; the local package license text was not added to this repository |
| Acquisition | Existing system installation; no source or binary was fetched or vendored by FrontierX |
| Linux build role | Optional (`FRONTIERX_BUILD_SDL3`) private backend dependency of `frontierx_sdl3_platform` and direct test dependency of `frontierx_sdl3_window_probe`; enabled by the checked-in presets |
| Distribution status | Not approved for FrontierX distribution; system-package provenance and license notice packaging still require review |

Compile and automated-check evidence on 2026-09-16: CMake `PkgConfig` found SDL3 3.4.16; GCC and
Clang linked the backend and probes; CTest created a hidden window, verified injected resize,
focus, keyboard, copied UTF-8 text, mouse and quit events, and checked the isolated legacy-input
compatibility records using the dummy video backend. A separate runtime smoke opened a visible
window, pumped events and closed through the local Wayland driver. This does not establish native
Windows input parity, renderer integration, package portability or retail compatibility.

## POSIX dynamic loader

| Field | Verified record |
|---|---|
| Purpose | Local ELF module ownership and exact symbol lookup for native DACOM and future campaign owners |
| API | `dlopen`, `dlsym`, `dlclose`, and `dlerror` through CMake's `${CMAKE_DL_LIBS}` |
| Acquisition | Host C library/runtime; no source or binary was fetched or vendored |
| Linux build role | Private dependency of `frontierx_dynamic_library` and its synthetic shared-object probe |
| Distribution status | System runtime dependency; final minimum-runtime and packaging policy remain to be recorded |

The probe builds a repository-owned synthetic ELF fixture and verifies local eager loading, exact
symbol lookup, failure state, move-only ownership, explicit DACOM registration and liveness-gated
unload under GCC, Clang and focused
sanitizers. It does not load or redistribute retail, SDK or third-party modules. See
[DYNAMIC_LIBRARY_PORTABILITY.md](DYNAMIC_LIBRARY_PORTABILITY.md).
