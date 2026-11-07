#!/usr/bin/env python3
"""Prepare a private Arch Linux preview from the full-media release build.

Explicit allowlist: no saves, fonts, retail archives, Windows binaries or local
configuration. This tool does not approve or publish a distribution.
"""

import argparse
import ctypes
import gzip
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile


ROOT = Path(__file__).resolve().parents[2]
FFMPEG_SHA256 = "cf38e0e28c7e5605942c4a77755349b0145804a397af37eb1fb4c77cb237f635"
LIBRARIES = (("avutil", "61"), ("avcodec", "63"), ("avformat", "63"), ("swscale", "10"))
SHADERS = (".vert", ".frag", "_t4.vert", "_mt.vert")


def digest(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def copy(source, destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source, destination)


def package(args):
    build = args.build.resolve()
    destination = args.output.resolve()
    if destination.exists() or destination.with_suffix(destination.suffix + ".sha256").exists():
        raise ValueError("Choose a new output path; existing packages are never overwritten")
    cache = {}
    for line in (build / "CMakeCache.txt").read_text().splitlines():
        if line and not line.startswith(("#", "//")) and "=" in line:
            key, value = line.split("=", 1)
            cache[key.split(":", 1)[0]] = value
    for key in ("FRONTIERX_BUILD_NATIVE_FONTS", "FRONTIERX_BUILD_FFMPEG_AUDIO", "FRONTIERX_BUILD_FFMPEG_VIDEO"):
        if cache.get(key) != "ON":
            raise ValueError(f"Full-media package requires {key}=ON")
    if cache.get("CMAKE_BUILD_TYPE") not in ("Release", "RelWithDebInfo"):
        raise ValueError("Use an optimized release build")
    source = args.ffmpeg_source.resolve()
    if digest(source) != FFMPEG_SHA256:
        raise ValueError("FFmpeg source archive does not match the reviewed release")
    binary = build / "frontierx_native_shell"
    elf = subprocess.check_output(["readelf", "-d", str(binary)], text=True)
    paths = [line for line in elf.splitlines() if "RUNPATH" in line or "RPATH" in line]
    if len(paths) != 1 or "[$ORIGIN/../lib]" not in paths[0]:
        raise ValueError("Build with CMAKE_BUILD_WITH_INSTALL_RPATH=ON and CMAKE_INSTALL_RPATH=$ORIGIN/../lib")
    revision = subprocess.check_output(["git", "-C", str(ROOT), "rev-parse", "HEAD"], text=True).strip()
    changes = subprocess.check_output(["git", "-C", str(ROOT), "diff", "HEAD", "--", "Code", "CMakeLists.txt", "CMakePresets.json"])
    if changes:
        raise ValueError("Production source/configuration differs from HEAD; record a source revision first")
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="frontierx-package-", dir=destination.parent) as temporary:
        staging = Path(temporary) / "frontierx-linux-x86_64-preview"
        staging.mkdir()
        copy(binary, staging / "bin/frontierx_native_shell")
        subprocess.run(["strip", "--strip-debug", str(staging / "bin/frontierx_native_shell")], check=True)
        (staging / "bin/frontierx_native_shell").chmod(0o755)
        copy(ROOT / "Tools/release/launcher.py", staging / "frontierx")
        (staging / "frontierx").chmod(0o755)
        for suffix in SHADERS:
            name = "sdl3_gpu_probe" + suffix + ".spv"
            copy(build / "generated/shaders" / name, staging / "shaders" / name)
        for name in ("Conquest.ini", "Hotkeys.utf"):
            copy(ROOT / "Code/App/Src" / name, staging / "resources" / name)
        materials = subprocess.check_output(["git", "-C", str(ROOT), "ls-files", "-z", "Data/mat"])
        for name in materials.decode().split("\0"):
            if name:
                copy(ROOT / name, staging / "resources/mat" / Path(name).relative_to("Data/mat"))
        for name in ("Conquest Source License.txt", "MODIFICATIONS.md", "LEGAL.md"):
            copy(ROOT / name, staging / name)
        for name in ("DEPENDENCIES.md", "PUBLICATION_REVIEW.md", "LINUX_PREBUILT.md"):
            copy(ROOT / "docs" / name, staging / "docs" / name)
        install = (ROOT / "docs/INSTALL_LINUX.md").read_text().replace("(../Conquest%20Source%20License.txt)", "(Conquest%20Source%20License.txt)")
        (staging / "INSTALL.md").write_text(install)
        provenance = []
        for name, version in LIBRARIES:
            library = args.ffmpeg_prefix / "lib" / f"lib{name}.so.{version}"
            handle = ctypes.CDLL(str(library.resolve()), mode=ctypes.RTLD_GLOBAL)
            license_function = getattr(handle, name + "_license")
            license_function.restype = ctypes.c_char_p
            if license_function() != b"LGPL version 2.1 or later":
                raise ValueError(f"Unreviewed license configuration for {name}")
            copy(library, staging / "lib" / library.name)
            configuration = getattr(handle, name + "_configuration")
            configuration.restype = ctypes.c_char_p
            # Preserve build switches without embedding the builder's private prefix.
            switches = re.sub(r"--prefix=\S+", "--prefix=/external/ffmpeg-lgpl", configuration().decode())
            provenance.append({"library": library.name, "sha256": digest(library),
                               "license": "LGPL-2.1-or-later", "configuration": switches})
        copy(source, staging / "sources/ffmpeg-9.0.1.tar.xz")
        with tarfile.open(source) as archive:
            for name in ("COPYING.LGPLv2.1", "LICENSE.md"):
                member = archive.extractfile("ffmpeg-9.0.1/" + name)
                if member is None:
                    raise ValueError("Missing FFmpeg notice")
                notice = staging / "notices" / ("FFmpeg-" + name)
                notice.parent.mkdir(exist_ok=True)
                notice.write_bytes(member.read())
        copy(Path("/usr/share/licenses/freetype2/FTL.TXT"), staging / "notices/FreeType-FTL.txt")
        copy(Path("/usr/share/licenses/sdl3/LICENSE"), staging / "notices/SDL-LICENSE.txt")
        # The statically linked TinyXML owner keeps its complete source notices.
        for name in ("tinyxml.h", "tinyxml.cpp", "tinyxmlparser.cpp", "tinyxmlerror.cpp", "tinystr.h", "tinystr.cpp"):
            copy(ROOT / "Code/Libs/Src/XML" / name, staging / "notices/tinyxml-source" / name)
        (staging / "PRIVATE_PREVIEW.txt").write_text(
            "Local installation candidate, not approved for public distribution.\n"
            "Review docs/LINUX_PREBUILT.md and docs/DEPENDENCIES.md before publication.\n"
            "This software is based in part on the work of the FreeType Team.\n")
        versions = "".join(subprocess.check_output(["readelf", "--version-info", str(path)], text=True)
                           for path in [staging / "bin/frontierx_native_shell", *sorted((staging / "lib").iterdir())])
        minimum = {prefix: max(set(re.findall(prefix + r"_([0-9.]+)", versions)),
                              key=lambda value: tuple(map(int, value.split("."))))
                   for prefix in ("GLIBC", "GLIBCXX")}
        manifest = {"status": "private-preview", "source_revision": revision,
                    "platform": "Arch Linux x86_64", "configuration": cache["CMAKE_BUILD_TYPE"],
                    "required_symbol_versions": minimum,
                    "original_binary_sha256": digest(binary), "ffmpeg": provenance,
                    "ffmpeg_source_sha256": FFMPEG_SHA256,
                    "files": {str(p.relative_to(staging)): digest(p) for p in sorted(staging.rglob("*")) if p.is_file()}}
        (staging / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        (staging / "SHA256SUMS").write_text("".join(f"{digest(p)}  {p.relative_to(staging)}\n"
                                                   for p in sorted(staging.rglob("*")) if p.is_file()))
        packed = Path(temporary) / "preview.tar.gz"
        with packed.open("wb") as stream, gzip.GzipFile(fileobj=stream, mode="wb", filename="", mtime=0) as compressed:
            with tarfile.open(fileobj=compressed, mode="w") as archive:
                for path in [staging, *sorted(staging.rglob("*"))]:
                    info = archive.gettarinfo(str(path), arcname=str(path.relative_to(staging.parent)))
                    info.uid = info.gid = 0
                    info.uname = info.gname = ""
                    info.mtime = 0
                    if path.is_file():
                        with path.open("rb") as contents:
                            archive.addfile(info, contents)
                    else:
                        archive.addfile(info)
        checksum = digest(packed)
        # Same-filesystem hard link publishes the complete archive atomically and
        # refuses a concurrent destination; it never exposes a partially copied tar.
        os.link(packed, destination)
        with destination.with_suffix(destination.suffix + ".sha256").open("x") as output:
            output.write(f"{checksum}  {destination.name}\n")
    print(f"Prepared private preview: {destination}\nSHA256: {checksum}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", type=Path, required=True)
    parser.add_argument("--ffmpeg-prefix", type=Path, required=True)
    parser.add_argument("--ffmpeg-source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    package(parser.parse_args())


if __name__ == "__main__":
    main()
