# Linux prebuilt candidate (2026-11-07)

## Scope

Prepare a relocatable local Arch Linux x86_64 preview with an optimized native
shell, shaders, original source-release material/profile resources, a user
launcher, notices and reviewed shared-LGPL FFmpeg libraries. Keep retail data,
fonts, player settings and saves external. Player-facing steps are in
[installation instructions](INSTALL_LINUX.md).

This is a private review artifact, not an approved public download. The existing
[dependency review](DEPENDENCIES.md) still requires resolution of distribution
obligations, including LGPL/Conquest compatibility, before public upload. A
successful package test does not waive those requirements. This work does not
create a GitHub release, commit, push, or claim cross-distribution compatibility.

## Build and package

The existing accepted candidate uses private absolute paths. The prebuilt uses
a separate build directory, relative ELF library lookup, remapped source paths,
and a package-relative launcher instead. Its launcher uses a distinct preview
save/config namespace and leaves existing playtests untouched.

Configure against the reviewed external shared-LGPL dependency prefix:

```sh
cmake --preset linux-clang-debug -B out/build/linux-release \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DFRONTIERX_BUILD_NATIVE_FONTS=ON \
  -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON \
  -DFRONTIERX_BUILD_FFMPEG_VIDEO=ON \
  -DCMAKE_PREFIX_PATH=/external/ffmpeg-lgpl \
  '-DCMAKE_INSTALL_RPATH=$ORIGIN/../lib' \
  -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
  "-DCMAKE_CXX_FLAGS=-ffile-prefix-map=$PWD=. -fdebug-prefix-map=$HOME=."
cmake --build out/build/linux-release --target frontierx_native_shell -j 3

python3 Tools/release/package_linux.py \
  --build out/build/linux-release \
  --ffmpeg-prefix /external/ffmpeg-lgpl \
  --ffmpeg-source /external/ffmpeg-9.0.1.tar.xz \
  --output /tmp/frontierx-linux-x86_64-preview.tar.gz
```

Packaging requires Python 3.11 or newer, Git, `readelf`, `strip`, and the local
Arch SDL/FreeType notice files. It uses an explicit file allowlist, checks full-media
configuration and the reviewed FFmpeg source hash, checks the four loaded library
license strings, and refuses to overwrite existing packages. Production source
must match HEAD; launcher/packaging changes are recorded by packaged file hashes.
This does not replace a full input-to-binary reproducibility audit.

The archive includes a source revision, original and stripped binary hashes,
per-file checksums, decoder configuration with the private prefix replaced by a
placeholder, the original FFmpeg source archive, and license/modification notices.
The ELF requirements include GLIBC 2.44 and GLIBCXX 3.4.32. System SDL3, FreeType,
the C/C++ runtime and GPU drivers are external; none is copied from the host.

The packager deliberately marks the artifact `private-preview`. Public release
automation and a download button must wait for approval of the actual contents,
dependency notices and supported-platform evidence. Older-Linux compatibility
requires a separate baseline build and testing, not merely a renamed archive.

## Validation

**Compilation:** Linux x86_64, Clang 22.1.8, separate `linux-release`
RelWithDebInfo build, SDL 3.4.16, FreeType 2.14.3 and the reviewed FFmpeg 9.0.1
prefix. The full native shell target builds. Existing `portability.layout` and
`portability.native_shell` CTests execute and pass with the external decoder
prefix available to the loader. No production source was changed.

**Synthetic launcher checks:** two Python tests execute repeated configuration,
failure preservation, relative font-map paths, duplicate rejection, package
relocation, argument ordering and separate save/config paths. Run:

```sh
python3 -m unittest discover -s Tools/release -p 'test_*.py' -v
```

**Actual packaged artifact:** extract into an external directory, rename the
package to a path containing spaces, configure lawfully external Steam data and
the user's existing strict font map, then execute `check` and `play --dry-run`.
All packaged manifest hashes and `SHA256SUMS` entries verify. The archive scan
finds no proprietary fonts, retail databases, mission saves or Windows binaries.
The complete bundle, including the decoder libraries, requires GLIBC 2.44;
checking only the executable would have understated this as 2.43.

**External-data GPU runtime:** the relocated package executes the existing
`--menu-check main --smoke-ms 1500` path with isolated preview settings and saves
on Wayland. Original menu output records 21 frames, timer completion and exit 0.
The initial attempt fails before window creation because the tool environment
has neither `DISPLAY` nor `WAYLAND_DISPLAY`; a standalone SDL diagnostic reports
no video device. Selecting the existing Wayland display resolves that host
condition. No game assertion or rendering check is disabled.

These checks establish local package startup and relocation. They do not establish
a clean-machine installation, audible media quality, complete gameplay, hardware
coverage, Windows/ABI or old-distribution compatibility. Known whole-process leak
and broader single-player acceptance gates remain separate. Hosted release/CI
execution and public dependency-distribution approval remain unperformed.
