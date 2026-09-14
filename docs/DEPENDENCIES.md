# Third-Party Dependency Record

Status: initial inventory, 2026-09-11

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