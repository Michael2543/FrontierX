# Repository publication review

## Decision and scope (2026-11-07)

Publication clearance is **not established**. GitHub repository metadata reports
`Michael2543/FrontierX` as public on this date. This review did not change that
setting, commit, push, rewrite history, or publish a package.

The review covers local reachable Git history through
`28805e99894befbba3fc33a35043234bea60e93d`, tracked working files, selected
third-party notices, and the checked-in Actions workflow. It does not cover
remote-only refs, forks, GitHub release attachments, existing Actions artifacts,
or untracked local files. The initial import is
`f66a257d538a4c476f561b9920a70efc43cd7912`.

The controlling license and [LEGAL.md](../LEGAL.md) require provenance and
third-party distribution review. A successful build, an upstream repository,
or a file's presence in the initial import does not settle those questions.

## Executed inventory

The read-only [publication inventory](../Tools/portability/audit_publication.py)
examined 10,229 tracked files, 34 reachable commits and 11,318 distinct historical
blobs before the cleanup below. Classification is by filename extension and
explicit policy names, not a complete file-format or license detector.

| Review category | Tracked files | Byte-identical to initial import | Changed or added since import |
|---|---:|---:|---:|
| Executables, DLLs and libraries | 130 | 113 | 17 |
| Generated artifacts, including PDB and build logs | 48 | 47 | 1 |
| Audio, models, images and other selected media formats | 5,478 | 5,475 | 3 |
| Selected document/archive formats | 14 | 14 | 0 |

All 5,475 inherited media files match both the initial import and the original
source-release archive supplied by the user's lawful Steam installation.
`Conquest Source Readme.txt` explicitly describes the accompanying resources.
These are verified source-release resources, not unexplained retail additions.

The archive `conquest_frontier_wars_source_code.zip` is 595,411,513 bytes with
SHA-256 `3413ffdacb488e3b8cc38e71ce1d3ae2bec278a1005885ff6784daf11547b5f6`.
The comparison streamed all 11,033 files without extracting or importing them.
At exact case-sensitive paths, 5,791 HEAD files match, 3,820 differ in bytes,
and 618 have no same-path archive entry. The byte differences include text line
endings, so these are not counts of semantic source changes. The controlling
license differs only by CRLF/LF line endings; its text is unchanged. No license
file was modified by this cleanup.

Of 130 tracked binaries/libraries, 93 match the official archive, 36 differ,
and `Code/Libs/Static/release/RPUL.lib` has no same-path entry. The Bink libraries,
Python binaries and named setup/debug DLLs below are among the verified original
archive files. This establishes acquisition provenance; it does not replace
third-party notice and distribution review.

Neither `binkw32.dll` nor `Conquest_Frontier_Wars_Manual.pdf` occurs as a file in
the current tree or any tree reachable from the local refs. Existing policy
exclusions remain useful prevention rules. Bink headers and three import
libraries are present and are a separate review item.

The bounded content scan found zero matches for its GitHub-token, AWS access-key
ID and private-key-block patterns in tracked working files and historical blobs.
It does not detect every credential format, personal information, secrets in
compressed files, or values without recognizable prefixes. Reports contain
paths, hashes and finding types, never matched credential text. Keep reports
private and outside the repository.

## Concrete unresolved items

| Item | Observed evidence | Required resolution |
|---|---|---|
| Bink SDK | `Code/Libs/Include/bink/` reports 1.5y; all three `binkw32.lib` files match the official source archive | Establish applicable third-party redistribution terms or move SDK acquisition outside a separately prepared public source tree; preserve the Windows VideoSystem dependency |
| Microsoft and other legacy binaries | `Code/App/Src/dsetup.dll`, `dsetup32.dll`, `dbghelp.dll`, and `EBUEula.dll` match the official source archive | Review the applicable distribution terms per artifact, or replace bundled copies with documented external prerequisites |
| Python SDK/runtime | Headers identify Python 2.3.4; Python 2.3 DLLs and import libraries match the official source archive | Verify binary versions and retain the complete applicable license record; header version alone does not identify a DLL |
| HAP debugger | `DbgRemote.cpp` explicitly selects LGPL 2.1 or later and references its included license | Review its distribution obligations alongside the controlling Program terms, or prepare a source subset that excludes the independent tool and its dependencies |
| Vendored sources | TinyXML, Lua, Scintilla, CSizingControlBar and an old FFTW header have distinct notices | Complete the source inventory and preserve the applicable notices; see [dependency findings](DEPENDENCIES.md#source-repository-review-2026-11-07) |
| Rebuilt legacy binaries | 17 paths differ from the initial import; 37 differ from or are absent at the same path in the official archive | Record reproducible source/build identity and dependency provenance, or provide external prerequisites instead of opaque checked-in outputs |
| Generated artifacts | PDB files and legacy build logs remain tracked | Review separately for diagnostic value and embedded paths before removing them; deletion at HEAD does not remove historical copies |

No item in this table is declared unlawful merely because its provenance is
unresolved. It remains a publication blocker under the repository's existing
review policy. External SDL, FFmpeg, FreeType and toolchain packaging is distinct
from publishing this source tree; binary-distribution gates remain in
[DEPENDENCIES.md](DEPENDENCIES.md).

## Completed cleanup

- Removed the three tracked `native_streamer_fixture/` files. All 1,694 bytes
  exactly match the generator in `Tools/portability/native_window_manager_probe.cpp`:
  PCM tone, invalid text input and a truncated IAC header. They are generated
  test output, not retail recordings or required checked-in fixtures.
- Ignored that root output directory, local `.env` files, private-key container
  formats and crash dumps. Example environment files remain reviewable.
- Retained the README refresh and original cover as an attributed external image
  reference. No cover file was added to Git. External hosting is not a grant of
  artwork redistribution rights; do not copy it into a package without a basis.
- Added a repeatable publication inventory and tests for a deleted historical
  token, changed binary provenance, redacted findings and report destination.

The generated fixtures still exist in history. Their source-generated provenance
is established, so that history is not presented as a retail-media exposure.
Unknown resources and Windows dependencies were not deleted to manufacture a
clean inventory or break the supported baseline.

## Reproduce the review

Use a new private output path for each run:

```sh
python3 Tools/portability/audit_publication.py --output /tmp/frontierx-publication-review.json
python3 -m unittest discover -s Tools/portability -p test_audit_publication.py -v
git diff --check
```

Add `--source-archive /external/conquest_frontier_wars_source_code.zip` to include
the exact source-release comparison. Python 3.11 or newer is required for this
optional comparison. The original archive remains private and external.

The inventory deliberately returns **2**, meaning review required. It does not
provide an automatic approval mode. It refuses an output path inside the
repository and refuses to overwrite existing reports. It reads local refs only;
fetch and review the intended publication refs separately before a final audit.

**Validation:** Linux x86_64, Python standard library and local Git. The full
inventory executed with the counts above and zero bounded credential matches.
Three automated tests passed using synthetic data in an isolated temporary Git
repository, including a changed-file source-archive comparison. The real Steam
archive comparison executed separately and produced the results above. Byte
comparison established that all three removed fixtures match their producer.
The existing GCC Debug `frontierx_native_window_manager_probe` also executed in
an empty external working directory with `SDL_VIDEODRIVER=dummy` and
`SDL_AUDIODRIVER=dummy`, regenerated its inputs, and passed the window/audio
lifecycle checks. No game source, persistent schema or build configuration changed.
No new build, GPU, retail playthrough or Windows/ABI execution is claimed.

The checked-in Actions workflow uses manual dispatch, a default-branch condition,
a named environment, pinned actions and read-only contents permissions. It has
no pull-request trigger. The actual hosted environment protections and existing
artifacts were not inspected; do not infer them from YAML alone.

## Next decision

The original source-release comparison is complete. Finish the third-party
distribution records and rebuilt-binary identification above. If the whole tree cannot be
cleared, prepare a separate reviewed source snapshot with explicit external
Windows SDK/runtime requirements. Validate both supported build paths before
publishing that snapshot. Removing files from the next commit alone would not
resolve their exposure through existing history, forks or downloaded copies.

Any visibility change, history rewrite or replacement public repository must be
an explicit maintainer decision against the resulting concrete file manifest.
