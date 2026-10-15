"""Embed original first-Terran context, minimap and edge-scroll cursor resources.

Blocker: ObjComm requests exact cursor IDs and UnbornMeshList gates its preview
on IDC_CURSOR_BUILD. SysMap/MScroll request their own resources. The display-only
cursor probe cannot cover malformed CUR decoding or bitmap XOR/AND semantics;
the deterministic self-check protects those original resource contracts.
"""
from pathlib import Path
import re
import struct
import sys

REQUIRED = (
    'DEFAULT', 'LEFT', 'RIGHT', 'UP', 'DOWN', 'UPRIGHT', 'UPLEFT',
    'DOWNRIGHT', 'DOWNLEFT', 'SELLOBJECT', 'MOVEOBJECT', 'BAN',
    'MICRO_MOVE', 'MICRO_GOTO', 'ATTACK', 'SPECIAL_ATTACK', 'HARVEST',
    'REPAIR', 'PATROL', 'JUMP', 'BUILD', 'ARROW', 'DEFEND', 'RESUPPLY',
    'RALLY', 'MICRO_RALLY', 'FABREPAIR',
)


def decode(data):
    if len(data) < 62 or struct.unpack_from('<HHH', data) != (0, 2, 1):
        raise ValueError('cursor requires one original CUR image')
    width, height, colors, reserved, hot_x, hot_y, size, offset = struct.unpack_from('<BBBBHHII', data, 6)
    if offset != 22 or size != len(data) - offset:
        raise ValueError('invalid cursor image extent')
    header, dib_width, dib_height, planes, bits, compression = struct.unpack_from('<IiiHHI', data, offset)
    if (width, height, colors, reserved, header, dib_width, dib_height,
            planes, compression) != (32, 32, 0, 0, 40, 32, 64, 1, 0) or bits not in (1, 8):
        raise ValueError('unsupported original cursor layout')
    if hot_x >= width or hot_y >= height:
        raise ValueError('invalid cursor hotspot')
    image_size, _, _, used, important = struct.unpack_from('<IiiII', data, offset + 20)
    count = 1 << bits
    stride = ((width * bits + 31) // 32) * 4
    if used not in (0, count) or important > count or image_size not in (0, stride * height):
        raise ValueError('invalid cursor DIB palette/image metadata')
    palette_offset = offset + header
    pixels_offset = palette_offset + count * 4
    mask_offset = pixels_offset + stride * height
    if mask_offset + height * 4 != len(data):
        raise ValueError('invalid cursor palette/pixel/mask extent')
    pixels, mask = [], []
    if bits == 1:
        if data[palette_offset:palette_offset + 8] != bytes((0, 0, 0, 0, 255, 255, 255, 0)):
            raise ValueError('unsupported monochrome cursor palette')
        for y in range(height):
            row = height - 1 - y
            for byte in range(4):
                xor = data[pixels_offset + row * stride + byte]
                and_bits = data[mask_offset + row * 4 + byte]
                # Windows: (destination AND mask) XOR image. SDL data/mask:
                # black=1/1, white=0/1, transparent=0/0, inverted=1/0.
                mask.append(and_bits ^ 255)
                pixels.append(xor ^ (and_bits ^ 255))
    else:
        for y in range(height):
            row = height - 1 - y
            for x in range(width):
                index = data[pixels_offset + row * stride + x]
                blue, green, red, _ = struct.unpack_from('<BBBB', data, palette_offset + index * 4)
                transparent = data[mask_offset + row * 4 + x // 8] & (128 >> (x % 8))
                pixels.extend((red, green, blue, 0 if transparent else 255))
    return hot_x, hot_y, bits == 1, pixels, mask


def resources(source):
    declarations = dict(re.findall(r'(IDC_CURSOR_\w+)\s+CURSOR\s+"([^"]+)"', source.read_text()))
    for suffix in REQUIRED:
        identifier = 'IDC_CURSOR_' + suffix
        path = source.parent
        for part in declarations[identifier].replace('\\\\', '/').split('/'):
            matches = [child for child in path.iterdir() if child.name.casefold() == part.casefold()]
            if len(matches) != 1:
                raise ValueError('cursor source path must resolve uniquely: ' + declarations[identifier])
            path = matches[0]
        yield identifier, path, decode(path.read_bytes())


def generate(source, destination):
    output = ['// Generated from original source-owned Globals.rc cursor resources.']
    table = []
    for identifier, path, (hot_x, hot_y, monochrome, pixels, mask) in resources(source):
        name = identifier.lower()
        for kind, values in (('pixels', pixels), ('mask', mask)):
            if not values:
                continue
            rows = [', '.join(str(v) for v in values[i:i + 32]) for i in range(0, len(values), 32)]
            output.append('constexpr unsigned char ' + name + '_' + kind + '[] = {\n' + ',\n'.join(rows) + '\n};')
        table.append(f'{{{identifier}, {hot_x}, {hot_y}, {str(monochrome).lower()}, '
                     f'{name}_pixels, ' + (name + '_mask' if mask else 'nullptr') + '}')
    output.append('constexpr SourceCursor source_cursors[] = {\n' + ',\n'.join(table) + '\n};')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text('\n'.join(output) + '\n')


def self_check(source):
    originals = list(resources(source))
    assert len(originals) == len(REQUIRED)
    for identifier, path, result in originals:
        x, y, monochrome, pixels, mask = result
        assert len(pixels) == (128 if monochrome else 4096)
        assert len(mask) == (128 if monochrome else 0)
        data = path.read_bytes()
        for offset, value in ((0, 1), (2, 1), (4, 2), (6, 0), (8, 1),
                              (10, 32), (18, 23), (22, 41), (36, 4), (38, 1)):
            broken = bytearray(data)
            broken[offset] = value
            try:
                decode(broken)
            except ValueError:
                continue
            raise AssertionError('accepted malformed cursor ' + identifier)
        for truncated in (data[:20], data[:-1], data + b'\0'):
            try:
                decode(truncated)
            except ValueError:
                continue
            raise AssertionError('accepted invalid cursor extent')
    # Distinct top/bottom pixels protect bottom-up rows, BGR palette order and AND alpha.
    color = bytearray(next(path.read_bytes() for identifier, path, result in originals
                           if identifier == 'IDC_CURSOR_BUILD'))
    pixel_offset = 62 + 1024
    mask_offset = pixel_offset + 1024
    color[62 + 7 * 4:62 + 7 * 4 + 4] = bytes((12, 34, 56, 0))
    color[62 + 8 * 4:62 + 8 * 4 + 4] = bytes((78, 90, 12, 0))
    color[pixel_offset + 31 * 32] = 7
    color[mask_offset + 31 * 4] &= 0x7F
    color[pixel_offset + 31] = 8
    color[mask_offset + 3] |= 1
    rgba = decode(color)[3]
    assert rgba[:4] == [56, 34, 12, 255] and rgba[-4:] == [12, 90, 78, 0]
    # Synthetic four-way truth table protects inversion, including both row ends.
    mono = bytearray(next(path.read_bytes() for identifier, path, result in originals if result[2]))
    xor_offset = 22 + 40 + 8
    mask_offset = xor_offset + 128
    mono[xor_offset + 31 * 4] = 0x50
    mono[mask_offset + 31 * 4] = 0x30
    result = decode(mono)
    assert result[3][0] >> 4 == 0x9 and result[4][0] >> 4 == 0xC
    print(f'Original cursor CPU check: {len(originals)} resources, strict rejection, monochrome XOR/AND')


if __name__ == '__main__':
    if sys.argv[1] == '--self-check':
        self_check(Path(sys.argv[2]))
    else:
        generate(Path(sys.argv[1]), Path(sys.argv[2]))
