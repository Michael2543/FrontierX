#!/usr/bin/env python3
"""Embed a production parser resource in a native object."""

import pathlib
import sys


def main() -> int:
    if len(sys.argv) not in (3, 4):
        print("usage: embed_app_schema.py INPUT OUTPUT [SYMBOL]", file=sys.stderr)
        return 2
    symbol = sys.argv[3] if len(sys.argv) == 4 else "appSchemaBytes"
    if not symbol.isidentifier():
        print("invalid schema symbol", file=sys.stderr)
        return 2
    source = pathlib.Path(sys.argv[1]).read_bytes()
    if not source or len(source) > 4 * 1024 * 1024:
        print("parser resource has an invalid size", file=sys.stderr)
        return 1
    rows = [
        "    " + ", ".join(f"0x{byte:02x}" for byte in source[i:i + 16]) + ","
        for i in range(0, len(source), 16)
    ]
    output = (
        "// Generated parser resource; do not edit.\n"
        f"static constexpr unsigned char {symbol}[] = {{\n"
        + "\n".join(rows)
        + "\n};\n"
    )
    pathlib.Path(sys.argv[2]).write_text(output, encoding="ascii")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
