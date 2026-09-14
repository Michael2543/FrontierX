# Windows build declarations and next portable slice

Evidence: source review and automated inventory, 2026-09-13. No MSBuild execution or retail run.

## Reproduce

From the repository root, using Python 3 (standard library only):

```text
python3 Tools/portability/build_inventory.py
PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s Tools/portability -p 'test_build_inventory.py' -v
```

Both commands passed on Linux. The first writes JSON to stdout; redirect diagnostic output outside
source control if needed. Five synthetic tests cover solution edges, conditional declarations,
reference cycles, missing references, unresolved project paths and references outside the root.

The scanner starts with Conquest.sln, ConquestCampaign.sln and the literal engine project list in
Build-Conquest.ps1. It follows ProjectReference declarations recursively. It preserves conditions,
source items, include/library directories, additional link inputs, imports, custom commands and
outputs. Cycles do not cause infinite traversal; each project appears once.

This is a union across configurations, not an evaluated MSBuild graph. Excluded source items remain
counted. Imported props/targets, macro expansion, command execution and implicit library edges
are not evaluated. Directory.Build.props and environment-specific SDK imports still need explicit
review when translating a target. A successful scan does not prove that a source path exists with
exact case, that a project builds, or that a linked DLL was rebuilt.

## Measured closure

The supported entry points reach **29 projects**, **12 distinct ProjectReference edges** and
**8 solution dependency edges**. The closure contains five application projects, eighteen campaign
projects and six library projects. StringTable is not in this explicit closure; its existing TinyXML
ownership does not prove that the supported wrapper rebuilds it.

| Library in explicit closure | Declared source items | Boundary found by source review |
|---|---:|---|
| COMHeap | 1 | Allocation boundary; preserve existing CRT/DACOM ownership |
| DACOM | 5 | Component loading, heaps and profile parser; separate services before porting |
| MathLib | 1 | MathLib.cpp acquires I3DMathEngine through DACOM; not a standalone math implementation |
| RPUL | 3 | Win32 device enumeration, render interfaces and text utilities |
| MeshManager | 4 | Link declarations include DACOM, MathLib, COMHeap and D3DX9 |
| D3DRenderPipe | 5 | Renderer and its dependent engine libraries; unsuitable as first isolated service |

Globals declares only one ClCompile source item but also custom generation steps for Data.cpp and
sfxdata.xmf. A source-count ranking would therefore hide both generation and runtime dependencies.

## Selected next candidate: profile parsing core

The next engine-owned slice to investigate is the in-memory parsing core of
Code/Libs/Src/DACOM/ProfileParser.cpp. It has concrete startup consumers in DACOM.CPP and
Code/App/Src/Conquest.cpp and an existing IProfileParser boundary. Its section-marking and navigation core now has a shared header and Linux probe (2026-09-13);
key lookup and output copying are also shared; an owning portable reader and enumeration remain.

Before extracting it:

1. Characterize section/key matching, line endings, comments, duplicate entries, missing keys and
   output-buffer truncation from the existing implementation with synthetic input tests.
2. Separate byte parsing from CreateFile/ReadFile, DACOM interface registration and heap ownership.
3. Classify section handles: the implementation casts HANDLE to U32 for buffer offsets. Do not
   blindly widen stored offsets or treat them as native object pointers.
4. Link the actual extracted implementation into a Linux test consumer. Preserve the Windows
   adapter and require native Windows regression/ABI validation before accepting that integration.

No replacement parser, generic configuration framework, math-policy change or new dependency was
introduced by this inventory. The existing four CTests passed again under both GCC and Clang using
the documented presets on 2026-09-13; these were incremental builds with no compilation work needed.
