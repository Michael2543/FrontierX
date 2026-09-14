# Prominent Notice of Modifications

This repository is a modified version of the *Conquest: Frontier Wars* Program. The entire Program
Derivative, as a whole, and every recipient are subject to the exact terms of the unmodified
[Public License for Conquest: Frontier Wars Program and Source Code](Conquest%20Source%20License.txt),
copyright Fever Pitch Studios, December 2013.

This notice records modifications; it does not amend or add to the license. Git history contains the
detailed file-level record.

| Relevant date | Author | Modification notice |
|---|---|---|
| 2022-12-25 | Revan67 | Imported the source release, converted initial projects to Visual Studio 2022, and established the public repository. |
| 2026-03-25 through 2026-05-26 | Revan67 | Ported the Windows and DirectX-era build to current Visual Studio tooling; added compatibility shims, reproducible data-generation support, diagnostics, and initial runtime restoration fixes. |
| 2026-08-20 through 2026-08-30 | Revan67 | Restored retail ABI and data layouts; repaired renderer, movement, combat, explosion, construction, and mission behavior; rebuilt engine components; added campaign projects, verified build/deploy tooling, crash analysis, tests, and project documentation. |
| 2026-08-30 | Revan67 | Added the legal-compliance policy, distribution checklist, interoperability-research boundaries, third-party provenance warnings, and automatic deployment of required legal notices. |
| 2026-09-11 | Michael2543 | Established the FrontierX native-Linux-port derivative, documented its scope and architecture constraints, added AI-agent instructions, and excluded third-party files with unresolved redistribution provenance from the new project history. |
| 2026-09-11 | Michael2543 | Corrected agent instruction activation and shared scope; documented Linux readiness milestones; parameterized and hardened the native-Windows ABI diagnostics; added CMake/Ninja Linux x86_64 layout, TinyXML, and standard-C++ platform timing targets with linked tests; made legacy integer aliases LP64-safe while preserving their Windows ABI types; isolated the retail mission storage layout; and ported the existing TinyXML sources away from unconditional Win32 dependencies. |
| 2026-09-12 | Michael2543 | Extended the portable timing contract with high-resolution monotonic time and deterministic elapsed-time clamping; migrated the modal UI frame clock and progress-animation clock and sleep; wired the shared implementation into the VS2022 `Trim` project; and added a read-only exact-case startup data-root inspector with deterministic CLI, environment, and fallback selection tests. |

| 2026-09-13 | Michael2543 | Added a read-only Windows build-declaration inventory with synthetic tests, measured the supported project-reference closure, and selected the profile-parser core for the next isolated Linux investigation. |

| 2026-09-13 | Michael2543 | Extracted DACOM profile-buffer section marking and navigation into a shared portable core, retained the Windows wrapper, and added GCC/Clang characterization tests for line and section boundaries. |

| 2026-09-13 | Michael2543 | Shared profile key lookup and bounded output copying between DACOM and the Linux probe, preserved caller-selected comparison semantics, guarded zero-capacity output, and expanded characterization tests. |

| 2026-09-13 | Michael2543 | Added an owning native ASCII profile reader with bounded file loading, section enumeration, shared-core lookup and synthetic file/ownership/error tests. |

| 2026-09-13 | Michael2543 | Added a native headless startup path checker integrating configuration and data-root selection, with explicit exit codes and synthetic process tests. |

| 2026-09-13 | Michael2543 | Added bounded UTF 1.01 header inspection to the native headless checker and synthetic malformed-header process tests. |

| 2026-09-13 | Michael2543 | Added bounded UTF tree/name/payload-extent validation and corrected absolute header free-entry offsets from source evidence, with malformed-tree and free-chain tests. |

| 2026-09-13 | Michael2543 | Added bounded native UTF entry reads by virtual path with same-stream validation and synthetic payload checksum checks. |

Future material modifications must add a dated and attributed entry here before distribution.
