"""Process-level startup checks using empty synthetic directories, never retail media."""
import os
from pathlib import Path
import subprocess
import struct
import sys
import tempfile
import unittest

EXECUTABLE = str(Path(sys.argv.pop(1)).resolve())


class HeadlessTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.data = self.root / 'Data'
        directories = [
            'Objects', 'Interface', 'Textures', 'Mat', 'Profiles',
            'Maps/Campaign', 'Maps/Multiplayer',
        ]
        for directory in directories:
            (self.data / directory).mkdir(parents=True)

    def run_check(self, *args, environment='', xdg_config_home='', home=None):
        env = os.environ.copy()
        env['FRONTIERX_DATA_ROOT'] = environment
        env['XDG_CONFIG_HOME'] = xdg_config_home
        env['HOME'] = str(self.root / 'home') if home is None else home
        return subprocess.run([EXECUTABLE, *map(str, args)], cwd=self.root, env=env,
                              text=True, capture_output=True, timeout=10)

    @staticmethod
    def write_preferences(path, entries):
        path.parent.mkdir(parents=True, exist_ok=True)
        payload = bytearray(b'FXSET001' + struct.pack('<I', len(entries)))
        for key, value_type, value in sorted(entries):
            key_bytes = key.encode('ascii')
            payload.extend(struct.pack('<III', len(key_bytes), value_type, len(value)))
            payload.extend(key_bytes)
            payload.extend(value)
        path.write_bytes(payload)

    @staticmethod
    def native_user_settings(prefix=''):
        u32 = lambda value: struct.pack('<I', value)
        s32 = lambda value: struct.pack('<i', value)
        return [
            (prefix + 'UserDefaults.Schema', 1, u32(1)),
            (prefix + 'Window.MainX', 1, s32(0)),
            (prefix + 'Window.MainY', 1, s32(0)),
            (prefix + 'Window.Width', 1, s32(0)),
            (prefix + 'Window.Height', 1, s32(0)),
            (prefix + 'Window.ShowState', 1, u32(5)),
            (prefix + 'Viewer.Width', 1, s32(100)),
            (prefix + 'Viewer.Height', 1, s32(100)),
            (prefix + 'Network.MinLatency', 1, u32(0)),
            (prefix + 'Network.PacketLossPercent', 1, u32(0)),
            (prefix + 'Input.MouseSpeed', 1, s32(0)),
            (prefix + 'Camera.ScrollRate', 1, struct.pack('<f', 1.0)),
            (prefix + 'Sound.Music.Muted', 1, u32(0)),
            (prefix + 'Sound.Music.Volume', 1, s32(6)),
            (prefix + 'Sound.Effects.Muted', 1, u32(0)),
            (prefix + 'Sound.Effects.Volume', 1, s32(9)),
            (prefix + 'Sound.Comms.Muted', 1, u32(0)),
            (prefix + 'Sound.Comms.Volume', 1, s32(7)),
            (prefix + 'Sound.Chat.Muted', 1, u32(0)),
            (prefix + 'Sound.Chat.Volume', 1, s32(9)),
            (prefix + 'Video.GammaCorrection', 1, s32(0)),
            (prefix + 'Simulation.GameSpeed', 1, s32(0)),
            (prefix + 'Simulation.FogMode', 1, s32(-1)),
            (prefix + 'UserDefaults.Flags', 1, u32(0x02822000)),
            (prefix + 'Simulation.MaxProjectiles', 1, u32(20)),
        ]

    @staticmethod
    def player_prefix(name):
        return 'Players.' + name.encode('utf-8').hex() + '.'

    def test_default_and_optional_media(self):
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: default', result.stdout)
        self.assertIn('Preferences source: home-default; status: missing; entries: 0; '
                      'user-defaults: absent',
                      result.stdout)
        self.assertIn('Movies: missing [optional]', result.stdout)
        self.assertIn('media contents are not validated', result.stdout)
        self.assertEqual(result.stdout, self.run_check().stdout)

    def test_preferences_path_and_validation(self):
        xdg = self.root / 'xdg'
        default_path = xdg / 'frontierx' / 'preferences.dat'
        self.write_preferences(default_path, [('CQPlayerName', 2, b'Blackwell')])
        result = self.run_check(xdg_config_home=str(xdg))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Preferences source: xdg-config-home; status: loaded; entries: 1',
                      result.stdout)
        self.assertNotIn(str(self.root), result.stdout)

        explicit = self.root / 'explicit.dat'
        self.write_preferences(explicit, [('CQWindowWidth', 1, struct.pack('<I', 1024))])
        result = self.run_check('--preferences', explicit, xdg_config_home='relative-xdg')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Preferences source: command-line; status: loaded; entries: 1',
                      result.stdout)

        self.assertEqual(self.run_check('--preferences', 'absent.dat').returncode, 6)
        explicit.write_bytes(b'not preferences')
        self.assertEqual(self.run_check('--preferences', explicit).returncode, 6)

        self.write_preferences(explicit, self.native_user_settings())
        result = self.run_check('--preferences', explicit)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('entries: 25; user-defaults: loaded', result.stdout)
        incomplete = self.native_user_settings()[:-1]
        self.write_preferences(explicit, incomplete)
        self.assertEqual(self.run_check('--preferences', explicit).returncode, 6)

        player_name = 'Blackwell'
        player_entries = self.native_user_settings()
        player_entries += self.native_user_settings(self.player_prefix(player_name))
        player_entries.append(('CQPlayerName', 2, player_name.encode('ascii')))
        self.write_preferences(explicit, player_entries)
        result = self.run_check('--preferences', explicit)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('user-defaults: loaded; settings-source: player', result.stdout)

        missing_player = self.native_user_settings()
        missing_player.append(('CQPlayerName', 2, player_name.encode('ascii')))
        self.write_preferences(explicit, missing_player)
        result = self.run_check('--preferences', explicit)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('user-defaults: absent; settings-source: defaults', result.stdout)

        invalid_player = player_entries[:-1]
        invalid_player = [
            entry for entry in invalid_player
            if entry[0] != self.player_prefix(player_name) + 'Window.Width'
        ]
        invalid_player.append(('CQPlayerName', 2, player_name.encode('ascii')))
        self.write_preferences(explicit, invalid_player)
        result = self.run_check('--preferences', explicit)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('user-defaults: loaded; settings-source: global', result.stdout)

        result = self.run_check(xdg_config_home='relative-xdg')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Preferences source: home-default', result.stdout)
        self.assertEqual(self.run_check(xdg_config_home='relative-xdg', home='').returncode, 6)

    def test_precedence(self):
        config = self.root / 'native.ini'
        config.write_text('[FrontierX]\nDataRoot=missing\n')
        result = self.run_check('--config', config, environment=str(self.data))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('source: environment', result.stdout)
        result = self.run_check('--config', config, '--data-root', self.data, environment='missing')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('source: command-line', result.stdout)

    def test_config_relative_to_file(self):
        config = self.root / 'settings' / 'native.ini'
        config.parent.mkdir()
        config.write_text('[FrontierX]\nDataRoot=../Data\n')
        result = self.run_check('--config', config)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('source: configuration', result.stdout)

    def test_configuration_failures(self):
        self.assertEqual(self.run_check('--config', 'absent').returncode, 2)
        config = self.root / 'bad.ini'
        invalid_configs = [
            '[FrontierX]\n',
            '[FrontierX]\nDataRoot=\n',
            '[FrontierX]\nDataRoot=Data\r\n[Next]\n',
        ]
        for text in invalid_configs:
            config.write_bytes(text.encode('ascii'))
            self.assertEqual(self.run_check('--config', config).returncode, 2)

    def test_required_case_and_root(self):
        (self.data / 'Objects').rename(self.data / 'objects')
        result = self.run_check()
        self.assertEqual(result.returncode, 3)
        self.assertIn('Objects: case-mismatch [required]', result.stdout)
        self.assertEqual(self.run_check('--data-root', 'missing').returncode, 3)

    def test_utf_header(self):
        path = self.root / 'metadata.db'
        # A synthetic header plus one directory slot and a padded name table.
        words = [0x20465455, 0x101, 56, 44, 0, 44, 100, 4, 2, 104, 0, 0, 0, 0]
        def write(values):
            root_entry = struct.pack('<11I', 0, 0, 0x10, 0, 0, 0, 0, 0, 0, 0, 0)
            path.write_bytes(struct.pack('<14I', *values) + root_entry + b'/\0\0\0')
        write(words)
        result = self.run_check('--metadata', path)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('payload contents are not validated', result.stdout)
        for index, value in [(0, 0), (1, 0x100), (20 // 4, 40), (12 // 4, 45),
                             (8 // 4, 0xfffffff0), (32 // 4, 5), (24 // 4, 56),
                             (36 // 4, 80), (16 // 4, 1)]:
            changed = words.copy()
            changed[index] = value
            write(changed)
            self.assertEqual(self.run_check('--metadata', path).returncode, 5, (index, value))
        path.write_bytes(b'UTF ')
        self.assertEqual(self.run_check('--metadata', path).returncode, 5)
        self.assertEqual(self.run_check('--metadata', 'absent.db').returncode, 5)

    def test_utf_tree(self):
        path = self.root / 'tree.db'
        words = [0x20465455, 0x101, 56, 88, 0, 44, 144, 8, 7, 152, 0, 0, 0, 0]
        root = [0, 0, 0x10, 0, 44, 0, 0, 0, 0, 0, 0]
        leaf = [0, 2, 0, 0, 0, 3, 3, 3, 0, 0, 0]
        def write(r, entry, names=b'/\0file\0\0'):
            path.write_bytes(struct.pack('<14I', *words) + struct.pack('<11I', *r) +
                             struct.pack('<11I', *entry) + names + b'abc')
        write(root, leaf)
        result = self.run_check('--metadata', path)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('1 reachable files', result.stdout)
        read = self.run_check('--metadata', path, '--entry', 'file')
        self.assertEqual(read.returncode, 0, read.stderr)
        self.assertIn('Entry read: 3 bytes; FNV-1a 440920331', read.stdout)
        self.assertEqual(self.run_check('--metadata', path, '--entry', 'FILE').returncode, 5)
        for invalid in ['/dir/file', 'dir//file', 'dir\\file', 'dir/file/']:
            self.assertEqual(self.run_check('--metadata', path, '--entry', invalid).returncode, 5)
        unequal = leaf.copy()
        unequal[7] = 4
        write(root, unequal)
        self.assertEqual(self.run_check('--metadata', path, '--entry', 'file').returncode, 5)
        write(root, leaf)
        for index, value in [(0, 44), (0, 45), (1, 7), (2, 0xffffffff), (4, 4), (5, 4), (6, 4)]:
            changed = leaf.copy()
            changed[index] = value
            write(root, changed)
            self.assertEqual(self.run_check('--metadata', path).returncode, 5, (index, value))
        write(root, leaf, b'/\0xxxxx\0')
        self.assertEqual(self.run_check('--metadata', path).returncode, 5)
        words[4] = 100  # Absolute offset: the same entry cannot be both live and free.
        write(root, leaf)
        self.assertEqual(self.run_check('--metadata', path).returncode, 5)
        empty_root = root.copy()
        empty_root[4] = 0
        unused = leaf.copy()
        unused[2] = 0xffffffff
        write(empty_root, unused)
        result = self.run_check('--metadata', path)
        self.assertEqual(result.returncode, 0, result.stderr)
        unused[0] = 44  # Relative link back to itself.
        write(empty_root, unused)
        self.assertEqual(self.run_check('--metadata', path).returncode, 5)

    def test_utf_nested_entry(self):
        path = self.root / 'nested.db'
        words = [0x20465455, 0x101, 56, 132, 0, 44, 188, 12, 12, 200, 0, 0, 0, 0]
        root = [0, 0, 0x10, 0, 44, 0, 0, 0, 0, 0, 0]
        directory = [0, 2, 0x10, 0, 88, 0, 0, 0, 0, 0, 0]
        leaf = [0, 6, 0, 0, 0, 3, 3, 3, 0, 0, 0]
        names = b'/\0dir\0file\0\0'
        path.write_bytes(struct.pack('<14I', *words) + struct.pack('<11I', *root) +
                         struct.pack('<11I', *directory) + struct.pack('<11I', *leaf) +
                         names + b'xyz')
        result = self.run_check('--metadata', path, '--entry', 'dir/file')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Entry read: 3 bytes; FNV-1a 3298945248', result.stdout)
        self.assertEqual(self.run_check('--metadata', path, '--entry', 'Dir/file').returncode, 5)

    def test_arguments(self):
        self.assertEqual(self.run_check('--help').returncode, 0)
        for args in [('--entry', 'file'), ('--unknown',), ('--config',), ('--preferences',),
                     ('--data-root', ''), ('--data-root', 'a', '--data-root', 'b')]:
            self.assertEqual(self.run_check(*args).returncode, 1)


if __name__ == '__main__':
    unittest.main()
