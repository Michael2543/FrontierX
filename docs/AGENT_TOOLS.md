# Local agent development tools

These are optional host development tools, not game dependencies or distributed
FrontierX components. The 2026-10-17 setup uses manual Codex MCP registration without
hooks. `AGENTS.md` defines when agents should use them. No automatic commit, push,
gameplay change or retail-data upload is part of this setup.

## Installed versions and provenance

| Tool | Pinned version | Upstream and license | Role |
|---|---|---|---|
| uv | 0.13.1 | [astral-sh/uv](https://github.com/astral-sh/uv), MIT/Apache-2.0 | Isolated Python tools |
| Serena | `serena-agent==2.0.0` | [oraios/serena](https://github.com/oraios/serena), MIT | C++ symbols and references through local clangd |
| GDB MCP | 0.4.0, commit `cf2dccf5bf3b6f4ba5f4c76908c75a708d2a23b4` | [BeaCox/gdb-mcp](https://github.com/BeaCox/gdb-mcp), MIT | Structured native debugger sessions |
| REA | `rea-agents@6.5.0` | [morluto/rea](https://github.com/morluto/rea), MIT with third-party notices | Targeted native binary analysis |
| Ghidra | 12.1.4 PUBLIC, 20260921 | [NSA/Ghidra](https://github.com/NationalSecurityAgency/ghidra), Apache-2.0 with third-party notices | REA native analysis provider |

The tools are installed unmodified outside the repository. Their licenses do not
change the controlling game license. No tool binaries or dependencies are bundled
with FrontierX. The uv archive SHA256 was verified as
`eadd9d27c134ab5ac1ffd4473b97a741407f3ea238288f03121fc4deddf90220`;
the Ghidra ZIP as
`ddac49f903da9d5bac833e5cc79395098b9c33cfd3279be5f31bd00387d2d4db`.

## Host configuration

User-level `~/.codex/config.toml` registers `serena`, `gdb` and `rea` with absolute
executable paths. The previous file is backed up beside it with a
`before-frontierx-tools` timestamp. Existing MCP servers and plugin settings are
preserved. Start a new Codex session after setup so the servers are discovered.
`codex mcp list` verifies registration; an enabled entry alone is not a runtime test.

- Serena and GDB MCP live in separate uv environments under
  `~/.local/share/uv/tools`; entry points are in `~/.local/bin`. Both use managed
  Python 3.13.16. Serena uses the existing `/usr/bin/clangd`.
- Serena starts in `codex` context with `no-memories`, tool interface, and dashboard
  disabled. The project is explicitly selected and trusted only at its exact local
  path. Local `.serena/project.yml` selects `cpp` and the system clangd.
- Ignored root `compile_commands.json` points to the full-media GCC build database
  under `~/.local/share/frontierx/builds/p1b-video-gcc`. Repoint this symlink to the
  intended configured build when the active configuration changes, then restart
  Serena. Do not merge incompatible macro configurations. Regenerate the chosen
  CMake database when targets change. Native Windows coverage remains separate.
- GDB MCP starts with `--max-sessions 4 --output-limit-chars 12000`. The client
  enables 25 common debugging tools, including lifecycle, core loading, breakpoints,
  watches, thread/frame selection and inspection. Unsafe mode stays disabled.
  Release 0.4.0 has no `--tool-profile` option despite newer upstream documentation.
- REA lives under `~/.local/share/frontierx-agent-tools/rea`, installed with
  `--ignore-scripts --save-exact`. It uses existing Node 26.8.1 and Ghidra 12.1.4
  under the same tools directory. `JAVA_HOME=/opt/android-studio/jbr` selects the
  existing JDK 25.0.3. Its client exposes 23 native inspection/lifecycle tools rather
  than the complete 207-tool catalog. Ghidra is selected explicitly; the tool timeout
  is 450 seconds. Analysis remains local, but MCP results are visible to the agent
  and its model provider; local execution alone is not an offline privacy guarantee.

RenderDoc MCP was reviewed but not installed: its packaged setup targets Windows,
and its Linux source build needs a separate RenderDoc source/runtime setup. Revisit
for a concrete GPU-capture blocker. No additional planning framework was installed.

## Validation and limits

Validation records below describe tool integration on Linux x86_64, not additional
game capabilities. No game code was changed by this setup. Windows builds, module
ABI, retail compatibility and full mission playtests were not performed here.

- **Serena, automated source inspection:** all 23 MCP tools were discovered;
  `NativeGameSystem.cpp` produced a symbol outline and the
  `FrontierX/StartNativeGameSystem` definition. Reference lookup found its actual
  `NativeShell.cpp` caller after the background index populated. An independent
  `clangd --check=Code/Launcher/NativeGameSystem.cpp --log=error` exited 0.
- **GDB MCP, automated synthetic execution:** GDB 17.2 stopped the existing Clang
  debug `frontierx_encoded_handle_probe` at `main`, returned source/backtrace/locals,
  evaluated `sizeof $pc` as 8, continued to its successful handle-semantics output
  and normal exit, then closed the session. The release's safe expression filter
  rejects `sizeof(void*)`; this was not bypassed. Core-dump loading and `rr` were
  not exercised; `rr` is not installed.
- **REA/Ghidra, generated synthetic execution:** compiled and ran a small C ELF
  fixture, then recovered its named `7*x+13` function through both the REA CLI and
  an actual MCP `open_binary` / `procedure_pseudo_code` / `close_binary` sequence.
  No retail binary or asset was used. MCP initialization and catalog discovery also
  passed. Other REA providers and target formats remain untested.

Reusable host checks are retained outside source control. From the repository root:

```sh
~/.local/share/uv/tools/serena-agent/bin/python \
  ~/.local/share/frontierx-agent-tools/checks/serena_smoke.py
~/.local/share/uv/tools/gdb-mcp/bin/python \
  ~/.local/share/frontierx-agent-tools/checks/gdb_smoke.py \
  ~/.local/bin/gdb-mcp "$PWD/out/build/linux-clang-debug/frontierx_encoded_handle_probe"
~/.local/share/uv/tools/gdb-mcp/bin/python \
  ~/.local/share/frontierx-agent-tools/checks/rea_smoke.py
```

These checks need ordinary host cache access and, for GDB, ptrace permission.
They retain diagnostic output under `/tmp`. The REA check creates fresh synthetic
input. Exact installed Python dependency manifests and the npm lockfile remain
beside the host tools; they are not game distribution manifests.

## Maintenance

Keep versions pinned until a concrete tool defect or required capability justifies
an update. Preserve upstream notices and review changed startup options before
updating; the published README may describe unreleased features. Re-run the focused
tool checks after changing versions or host paths. Use `uv tool list` and the local
REA package manifest to inspect installed versions. Use `codex mcp get NAME` to
inspect a single registration without changing it.

If an MCP server is not exposed in the current session, use the installed CLI or
existing source/debugging tools and state the limitation. Do not treat absence of
MCP exposure as proof that installation failed. To disable a server, set its
`enabled = false` in the user configuration and start a new session.
