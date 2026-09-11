---
description: "Use when creating or changing FrontierX CMake, Visual Studio projects, build scripts, dependency integration, compiler flags, or build presets."
applyTo:
  - "**/CMakeLists.txt"
  - "**/*.cmake"
  - "CMakePresets.json"
  - "Directory.Build.props"
  - "**/*.{sln,vcxproj,props,targets}"
  - "**/build*.{sh,cmd,ps1}"
  - "**/Build*.{sh,cmd,ps1}"
  - "scripts/build/**"
---

# FrontierX Build Instructions

- Add CMake and Ninja support incrementally alongside the supported VS2022 Win32 build.
- Start with a small target whose sources and dependencies are understood; do not generate one
  monolithic target for the entire legacy tree as the first change.
- Use target-scoped sources, include directories, compile definitions, options, and link libraries.
- Do not add machine-specific absolute paths, local retail-install paths, or checked-in SDK files.
- Keep build products outside source directories and update `.gitignore` for new output roots.
- Separate Win32, Linux x86_64, Debug, and retail-like configurations explicitly. A configuration
  name is not a Git branch.
- Preserve required static-runtime and allocator constraints on the existing Windows targets unless
  an ownership audit proves a change safe.
- Treat warnings about pointer truncation, packing, calling conventions, and format strings as port
  defects rather than suppressing them globally.

## Dependencies

- Record source repository, pinned revision or release, license, maintenance state, and
  redistribution rationale before integrating a dependency.
- Prefer system packages or a reproducible dependency mechanism; do not commit fetched build trees.
- Do not adopt archived `misyltoad/dxvk-native` as a production dependency. A maintained native
  D3D9-to-Vulkan route needs a separately validated prototype and decision record.
- DXVK-compatible D3D9 does not provide D3DX9. Inventory and replace D3DX effects, texture helpers,
  shader compilation, and math utilities explicitly.
- Never fetch or package retail assets, the DirectX SDK, Bink binaries, or other proprietary
  redistributables.

## Validation

- Run the narrowest configure/build target after each build-system change.
- Preserve an available Windows validation path while adding Linux targets.
- A generated build system is not complete until configure and compile commands succeed in a clean
  build directory.
- Document only commands that were actually run, including required host platform and prerequisites.