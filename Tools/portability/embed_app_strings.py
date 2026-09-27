#!/usr/bin/env python3
"""Build native App string data from the source Globals.rc RT_STRING records."""

import pathlib
import re
import sys

from embed_sol_t_strings import extract


def main() -> int:
    if len(sys.argv) != 4 or sys.argv[1] not in ("--rc", "--include"):
        print("usage: embed_app_strings.py (--rc|--include) INPUT OUTPUT", file=sys.stderr)
        return 2
    source = pathlib.Path(sys.argv[2])
    output = pathlib.Path(sys.argv[3])
    try:
        if sys.argv[1] == "--include":
            emit(source, output)
            return 0
        source_text = source.read_text(encoding="cp1252")
        blocks = re.findall(r"(?m)^STRINGTABLE\s*\nBEGIN\n.*?^END\s*$", source_text, re.S | re.M)
        if not blocks:
            raise ValueError("Globals.rc has no string tables")
        output.write_text(
            '#include "resource.h"\n#include "winres.h"\n'
            "LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US\n"
            "#pragma code_page(1252)\n" + "\n".join(blocks) + "\n",
            encoding="cp1252",
        )
    except (OSError, UnicodeError, ValueError) as error:
        print(f"App string resource: {error}", file=sys.stderr)
        return 1
    return 0


def emit(resource: pathlib.Path, output: pathlib.Path) -> None:
    strings = extract(resource.read_bytes())
    lines = ["// Generated from Globals.rc RT_STRING; do not edit."]
    for string_id, codepoints in sorted(strings.items()):
        contents = ", ".join(f"0x{point:04x}" for point in [*codepoints, 0])
        lines.append(f"static const wchar_t appString{string_id}[] = {{{contents}}};")
    lines.append("static const NativeAppStringEntry appStringEntries[] = {")
    lines.extend(f"    {{{string_id}U, appString{string_id}}}," for string_id in sorted(strings))
    lines.append("};")
    output.write_text("\n".join(lines) + "\n", encoding="ascii")


if __name__ == "__main__":
    raise SystemExit(main())
