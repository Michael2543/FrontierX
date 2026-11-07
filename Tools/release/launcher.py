#!/usr/bin/env python3
"""Configure and launch an extracted FrontierX preview without modifying game data."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile


FONT_FILES = {
    "Arial,400": "arial.ttf", "Arial,500": "arial.ttf",
    "Arial,700": "arialbd.ttf", "Arial,900": "arialbd.ttf",
    "Arial Narrow,400": "arialn.ttf", "Arial Narrow,700": "arialnb.ttf",
    "BauhausMdITCTT,400": "bauhmib.ttf", "BauhausMdITCTT,600": "bauhmib.ttf",
    "BauhausMdITCTT,800": "bauhmib.ttf",
}
SHADERS = (".vert", ".frag", "_t4.vert", "_mt.vert")


def child(parent, name):
    matches = [p for p in parent.iterdir() if p.name.casefold() == name.casefold()]
    if len(matches) != 1:
        raise ValueError(f"Expected exactly one {name} in {parent}")
    return matches[0]


def locations():
    config = Path(os.environ.get("XDG_CONFIG_HOME", Path.home() / ".config")) / "frontierx-preview"
    data = Path(os.environ.get("XDG_DATA_HOME", Path.home() / ".local/share")) / "frontierx-preview"
    return config, data


def write_atomic(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(mode="w", dir=path.parent, delete=False, encoding="utf-8") as stream:
        temporary = Path(stream.name)
        try:
            stream.write(text)
            stream.flush()
            os.fsync(stream.fileno())
        except BaseException:
            temporary.unlink()
            raise
    try:
        temporary.replace(path)
    finally:
        temporary.unlink(missing_ok=True)


def validate_game(game):
    if not child(game, "data").is_dir():
        raise ValueError("The game's DATA directory is missing")
    for name in ("stringpack.db", "gendata.db", "gametypes.db"):
        if not child(game, name).is_file():
            raise ValueError(f"Missing game file: {name}")


def read_fonts(font_map):
    faces = {}
    for line in font_map.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        key, separator, value = line.partition("=")
        key = key.strip()
        if not separator or key in faces:
            raise ValueError("Invalid or duplicate font mapping")
        filename = Path(value.strip()).expanduser()
        if not filename.is_absolute():
            filename = font_map.parent / filename
        faces[key] = filename.resolve()
    if not FONT_FILES.keys() <= faces.keys():
        raise ValueError("Font map must provide Arial, Arial Narrow and Bauhaus at all documented weights")
    if any(not face.is_file() for face in faces.values()):
        raise ValueError("A mapped font file is missing")
    return faces


def configure(args, config):
    game = args.game_dir.expanduser().resolve()
    validate_game(game)
    if args.font_map:
        faces = read_fonts(args.font_map.expanduser().resolve())
    else:
        directories = [child(game, "fonts"), *[p.expanduser().resolve() for p in args.font_dir]]
        found = {}
        for directory in directories:
            for path in sorted(directory.rglob("*")):
                if path.is_file() and path.name.lower() in FONT_FILES.values():
                    found.setdefault(path.name.lower(), path.resolve())
        missing = sorted(set(FONT_FILES.values()) - found.keys())
        if missing:
            raise ValueError("Supply your own fonts with --font-dir or --font-map; missing: " + ", ".join(missing))
        faces = {key: found[name] for key, name in FONT_FILES.items()}
    contents = "".join(f"{key}={faces[key]}\n" for key in FONT_FILES)
    font_map = config / ("fonts-" + hashlib.sha256(contents.encode()).hexdigest()[:16] + ".txt")
    # A content-addressed map leaves the previous setup usable if configuration fails.
    write_atomic(font_map, contents)
    write_atomic(config / "launcher.json", json.dumps({"game_dir": str(game), "font_map": str(font_map)}, indent=2) + "\n")
    print("Configured. Run ./frontierx check, then ./frontierx play.")


def command(bundle, config, data):
    settings_path = config / "launcher.json"
    if not settings_path.is_file():
        raise ValueError("Run ./frontierx configure --game-dir PATH --font-dir PATH first; see INSTALL.md")
    settings = json.loads(settings_path.read_text())
    game = Path(settings["game_dir"])
    font_map = Path(settings["font_map"])
    validate_game(game)
    read_fonts(font_map)
    resources = bundle / "resources"
    binary = bundle / "bin/frontierx_native_shell"
    required = [binary, resources / "Conquest.ini", resources / "Hotkeys.utf"]
    shaders = [bundle / "shaders" / ("sdl3_gpu_probe" + suffix + ".spv") for suffix in SHADERS]
    if any(not p.is_file() for p in required + shaders) or not (resources / "mat").is_dir():
        raise ValueError("Incomplete preview package; extract a fresh copy")
    result = [str(binary), "--data-root", str(child(game, "data")),
              "--material-root", str(resources / "mat"), "--system-profile", str(resources / "Conquest.ini"),
              "--mission-source", "T_Mission1.dmission", "--mission-engine", "--mission-start", "--mission-campaign",
              "--ui-font-map", str(font_map), "--mission-gpu", *map(str, shaders)]
    for option, name in (("--string-pack", "stringpack.db"), ("--gen-data", "gendata.db"), ("--game-types", "gametypes.db")):
        result += [option, str(child(game, name))]
    return result + ["--hotkeys", str(resources / "Hotkeys.utf"), "--save-root", str(data / "saves")]


def environment(bundle, config):
    env = {key: value for key, value in os.environ.items() if not key.startswith("FRONTIERX_M3_")}
    env["XDG_CONFIG_HOME"] = str(config / "game")
    env["LD_LIBRARY_PATH"] = str(bundle / "lib") + (":" + env["LD_LIBRARY_PATH"] if env.get("LD_LIBRARY_PATH") else "")
    return env


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    actions = parser.add_subparsers(dest="action", required=True)
    setup = actions.add_parser("configure", help="Set external game and font paths")
    setup.add_argument("--game-dir", type=Path, required=True)
    fonts = setup.add_mutually_exclusive_group()
    fonts.add_argument("--font-map", type=Path)
    fonts.add_argument("--font-dir", type=Path, action="append", default=[])
    actions.add_parser("check", help="Check paths and dynamic-library loading without opening a window")
    play = actions.add_parser("play", help="Open the original single-player menus")
    play.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    bundle = Path(__file__).resolve().parent
    config, data = locations()
    if args.action == "configure":
        configure(args, config)
        return 0
    argv = command(bundle, config, data)
    env = environment(bundle, config)
    if args.action == "check":
        subprocess.run([argv[0], "--help"], env=env, check=True, stdout=subprocess.DEVNULL)
        print("Game files, font paths and dynamic-library loading checked. Visual/font compatibility still requires launch.")
        return 0
    if args.dry_run:
        print(shlex.join(argv))
        return 0
    data.mkdir(parents=True, exist_ok=True)
    (data / "saves").mkdir(exist_ok=True)
    (config / "game").mkdir(parents=True, exist_ok=True)
    with (data / "player.log").open("w") as log:
        print(f"Starting FrontierX. Log: {data / 'player.log'}", flush=True)
        result = subprocess.run(argv, env=env, cwd=data, stdout=log, stderr=subprocess.STDOUT)
    if result.returncode:
        print("The game exited with an error; inspect player.log.", file=sys.stderr)
    return result.returncode


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, KeyError, subprocess.CalledProcessError) as error:
        print(f"FrontierX: {error}", file=sys.stderr)
        raise SystemExit(1)
