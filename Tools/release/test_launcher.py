"""Exercise configuration persistence and package relocation with synthetic files."""

import argparse
import importlib.util
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch


spec = importlib.util.spec_from_file_location("frontierx_launcher", Path(__file__).with_name("launcher.py"))
launcher = importlib.util.module_from_spec(spec)
spec.loader.exec_module(launcher)


class LauncherTest(unittest.TestCase):
    def test_configuration_repeat_failure_and_relocation(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            game = root / "Game With Spaces"
            game.mkdir()
            (game / "DATA").mkdir()
            fonts = game / "FONTS"
            fonts.mkdir()
            for name in ("StringPack.db", "GenData.db", "GameTypes.db"):
                (game / name).write_text("synthetic")
            for name in set(launcher.FONT_FILES.values()):
                (fonts / name.upper()).write_text("synthetic font path only")
            config, data = root / "config", root / "data"
            args = argparse.Namespace(game_dir=game, font_map=None, font_dir=[])
            launcher.configure(args, config)
            before = (config / "launcher.json").read_bytes()
            launcher.configure(args, config)
            self.assertEqual(before, (config / "launcher.json").read_bytes())
            self.assertEqual(len(list(config.glob("fonts-*.txt"))), 1)
            (fonts / "ARIALN.TTF").unlink()
            with self.assertRaises(ValueError):
                launcher.configure(args, config)
            self.assertEqual(before, (config / "launcher.json").read_bytes())
            (fonts / "ARIALN.TTF").write_text("synthetic font path only")
            bundle = root / "bundle"
            for name in ("bin/frontierx_native_shell", "resources/Conquest.ini", "resources/Hotkeys.utf"):
                path = bundle / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.touch()
            (bundle / "resources/mat").mkdir()
            (bundle / "shaders").mkdir()
            for suffix in launcher.SHADERS:
                (bundle / "shaders" / ("sdl3_gpu_probe" + suffix + ".spv")).touch()
            moved = root / "Relocated Package"
            bundle.rename(moved)
            argv = launcher.command(moved, config, data)
            self.assertEqual(argv[0], str(moved / "bin/frontierx_native_shell"))
            self.assertIn(str(game / "DATA"), argv)
            self.assertEqual(argv[-1], str(data / "saves"))
            self.assertFalse(any(arg.startswith("--skip-") for arg in argv))
            self.assertIn("--mission-campaign", argv)
            with patch.dict(os.environ, {"FRONTIERX_M3_TEST": "1", "LD_LIBRARY_PATH": "/external"}):
                env = launcher.environment(moved, config)
            self.assertNotIn("FRONTIERX_M3_TEST", env)
            self.assertEqual(env["LD_LIBRARY_PATH"], str(moved / "lib") + ":/external")

    def test_relative_map_and_duplicate_keys(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "face.ttf").touch()
            font_map = root / "map.txt"
            text = "".join(f"{key}=face.ttf\n" for key in launcher.FONT_FILES)
            font_map.write_text(text)
            self.assertEqual(set(launcher.read_fonts(font_map).values()), {root / "face.ttf"})
            font_map.write_text(text + " Arial,400 =face.ttf\n")
            with self.assertRaises(ValueError):
                launcher.read_fonts(font_map)


if __name__ == "__main__":
    unittest.main()
