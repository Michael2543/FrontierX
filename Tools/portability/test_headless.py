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
        for directory in ['Objects', 'Interface', 'Textures', 'Mat', 'Profiles', 'Maps/Campaign', 'Maps/Multiplayer']:
            (self.data / directory).mkdir(parents=True)

    def run_check(self, *args, environment=''):
        env = os.environ.copy()
        env['FRONTIERX_DATA_ROOT'] = environment
        return subprocess.run([EXECUTABLE, *map(str, args)], cwd=self.root, env=env,
                              text=True, capture_output=True, timeout=10)

    def test_default_and_optional_media(self):
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: default', result.stdout)
        self.assertIn('Movies: missing [optional]', result.stdout)
        self.assertIn('media contents are not validated', result.stdout)
        self.assertEqual(result.stdout, self.run_check().stdout)

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
        for text in ['[FrontierX]\n', '[FrontierX]\nDataRoot=\n', '[FrontierX]\nDataRoot=Data\r\n[Next]\n']:
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
            path.write_bytes(struct.pack('<14I', *values) + struct.pack('<11I', 0, 0, 0x10, 0, 0, 0, 0, 0, 0, 0, 0) + b'/\0\0\0')
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

    def test_arguments(self):
        self.assertEqual(self.run_check('--help').returncode, 0)
        for args in [('--entry', 'file'), ('--unknown',), ('--config',), ('--data-root', ''), ('--data-root', 'a', '--data-root', 'b')]:
            self.assertEqual(self.run_check(*args).returncode, 1)


if __name__ == '__main__':
    unittest.main()
