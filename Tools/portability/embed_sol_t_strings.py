#!/usr/bin/env python3
"""Extract counted RT_STRING entries from the source-built Sol_T .res file."""

import pathlib
import struct
import sys


def ordinal(data: bytes, offset: int, end: int) -> tuple[int | None, int]:
    if offset + 2 > end:
        raise ValueError("truncated resource ordinal")
    marker = struct.unpack_from("<H", data, offset)[0]
    if marker != 0xFFFF:
        return None, offset
    if offset + 4 > end:
        raise ValueError("truncated resource ordinal")
    return struct.unpack_from("<H", data, offset + 2)[0], offset + 4


def extract(data: bytes) -> dict[int, list[int]]:
    strings: dict[int, list[int]] = {}
    offset = 0
    while offset < len(data):
        if offset + 8 > len(data):
            raise ValueError("truncated resource header")
        size, header_size = struct.unpack_from("<II", data, offset)
        end_header = offset + header_size
        end_data = end_header + size
        if header_size < 32 or end_data > len(data):
            raise ValueError("invalid resource extent")
        resource_type, next_offset = ordinal(data, offset + 8, end_header)
        group, _ = ordinal(data, next_offset, end_header)
        if resource_type == 6:
            if not group or group > 4096:
                raise ValueError("invalid RT_STRING group")
            cursor = end_header
            for slot in range(16):
                if cursor + 2 > end_data:
                    raise ValueError("truncated RT_STRING length")
                length = struct.unpack_from("<H", data, cursor)[0]
                cursor += 2
                end_string = cursor + length * 2
                if end_string > end_data:
                    raise ValueError("truncated RT_STRING data")
                if length:
                    decoded = data[cursor:end_string].decode("utf-16-le")
                    codepoints = [ord(char) for char in decoded]
                    if len(codepoints) != length:
                        raise ValueError("non-BMP script string changes counted length")
                    string_id = (group - 1) * 16 + slot
                    if string_id in strings:
                        raise ValueError("duplicate RT_STRING identifier")
                    strings[string_id] = codepoints
                cursor = end_string
            if cursor != end_data:
                raise ValueError("unexpected RT_STRING trailing data")
        offset = (end_data + 3) & ~3
    if offset != len(data) or not strings:
        raise ValueError("missing or malformed RT_STRING data")
    return strings


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: embed_sol_t_strings.py Sol_T.res OUTPUT", file=sys.stderr)
        return 2
    try:
        strings = extract(pathlib.Path(sys.argv[1]).read_bytes())
        lines = ["// Generated from the source Sol_T.rc RT_STRING records; do not edit."]
        for string_id, codepoints in sorted(strings.items()):
            values = [len(codepoints), *codepoints, 0]
            contents = ", ".join(f"0x{value:04x}" for value in values)
            lines.append(f"static const wchar_t solTrainString{string_id}[] = {{{contents}}};")
        lines.append("static const NativeSolTrainStringEntry solTrainStringEntries[] = {")
        lines.extend(
            f"    {{{string_id}U, solTrainString{string_id}}},"
            for string_id in sorted(strings)
        )
        lines.append("};")
        pathlib.Path(sys.argv[2]).write_text("\n".join(lines) + "\n", encoding="ascii")
    except (OSError, UnicodeError, ValueError) as error:
        print(f"Sol_T string resource: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
