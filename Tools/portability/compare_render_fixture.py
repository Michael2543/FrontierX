#!/usr/bin/env python3
"""Compare the shared 64x64 renderer fixture across D3D9 and SDL GPU captures.

The one-pixel search allows the two rasterizers to assign triangle-edge pixels
to adjacent samples. A separate direct mean bound prevents a shifted or blurred
image from passing merely because nearby colors happen to match.
"""

import argparse
from pathlib import Path


WIDTH = 64
HEIGHT = 64
HEADER = b"P6\n64 64\n255\n"
CLEAR = (6, 9, 17)


def read_fixture(path: Path) -> list[tuple[int, int, int]]:
    data = path.read_bytes()
    expected = len(HEADER) + WIDTH * HEIGHT * 3
    if len(data) != expected or not data.startswith(HEADER):
        raise ValueError(f"{path}: expected exact 64x64 P6 RGB fixture ({expected} bytes)")
    return [tuple(data[offset : offset + 3]) for offset in range(len(HEADER), len(data), 3)]


def nearby_maximum(left: list[tuple[int, int, int]],
                   right: list[tuple[int, int, int]]) -> int:
    worst = 0
    for y in range(HEIGHT):
        for x in range(WIDTH):
            pixel = left[y * WIDTH + x]
            nearest = min(
                max(abs(a - b) for a, b in zip(pixel, right[yy * WIDTH + xx]))
                for yy in range(max(0, y - 1), min(HEIGHT, y + 2))
                for xx in range(max(0, x - 1), min(WIDTH, x + 2))
            )
            worst = max(worst, nearest)
    return worst


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("linux", type=Path, help="native SDL GPU P6 capture")
    parser.add_argument("windows", type=Path, help="Win32 D3D9 P6 capture")
    args = parser.parse_args()

    try:
        linux = read_fixture(args.linux)
        windows = read_fixture(args.windows)
    except (OSError, ValueError) as error:
        parser.error(str(error))

    changed = [sum(pixel != CLEAR for pixel in image) for image in (linux, windows)]
    unique = [len(set(image)) for image in (linux, windows)]
    direct_mean = sum(
        abs(a - b)
        for first, second in zip(linux, windows)
        for a, b in zip(first, second)
    ) / (WIDTH * HEIGHT * 3)
    window_to_linux = nearby_maximum(windows, linux)
    linux_to_window = nearby_maximum(linux, windows)

    print(f"foreground_pixels linux={changed[0]} windows={changed[1]}")
    print(f"distinct_rgb linux={unique[0]} windows={unique[1]}")
    print(f"direct_mean_channel_error={direct_mean:.4f}")
    print(f"one_pixel_max_channel_error windows_to_linux={window_to_linux} "
          f"linux_to_windows={linux_to_window}")

    valid = (
        linux[0] == CLEAR
        and windows[0] == CLEAR
        and all(2400 <= count <= 2600 for count in changed)
        and abs(changed[0] - changed[1]) <= 10
        and all(count >= 100 for count in unique)
        and direct_mean <= 3.0
        and window_to_linux <= 8
        and linux_to_window <= 8
    )
    print("fixture_comparison=" + ("PASS" if valid else "FAIL"))
    return 0 if valid else 1


if __name__ == "__main__":
    raise SystemExit(main())
