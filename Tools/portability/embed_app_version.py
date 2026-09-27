#!/usr/bin/env python3
"""Extract the native build version from the source Conquest.rc VERSIONINFO."""

import pathlib
import re
import sys


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: embed_app_version.py Conquest.rc OUTPUT", file=sys.stderr)
        return 2
    try:
        source = pathlib.Path(sys.argv[1]).read_text(encoding="cp1252")
        versions = re.findall(r'(?m)^\s*VALUE\s+"FileVersion",\s*"([0-9]+(?:\.[0-9]+){3})"', source)
        if len(versions) != 1:
            raise ValueError("expected exactly one FileVersion value")
        components = [int(part) for part in versions[0].split(".")]
        if any(part > 255 for part in components[:3]) or components[3] > 0xffffffff:
            raise ValueError("FileVersion component exceeds source conversion range")
        packed = sum((part & 0xff) << shift for part, shift in zip(components, (24, 16, 8, 0)))
        pathlib.Path(sys.argv[2]).write_text(
            "// Generated from Conquest.rc FileVersion; do not edit.\n"
            f'constexpr char NativeAppVersionString[] = "{versions[0]}";\n'
            f"constexpr U32 NativeAppBuildVersion = 0x{packed:08x}U;\n",
            encoding="ascii",
        )
    except (OSError, UnicodeError, ValueError) as error:
        print(f"App build version: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
