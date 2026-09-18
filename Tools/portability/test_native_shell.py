"""Process tests for the native SDL shell using synthetic empty data directories."""
import os
from pathlib import Path
import subprocess
import struct
import sys
import tempfile
import unittest

EXECUTABLE = str(Path(sys.argv.pop(1)).resolve())
EVENT_MODULE = str(Path(sys.argv.pop(1)).resolve()) if len(sys.argv) > 1 else ''


class NativeShellTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.data = self.root / 'Data'
        for directory in [
                'Objects', 'Interface', 'Textures', 'Mat', 'Profiles',
                'Maps/Campaign', 'Maps/Multiplayer']:
            (self.data / directory).mkdir(parents=True)

    def run_shell(self, *arguments, data_root=''):
        environment = os.environ.copy()
        environment['SDL_VIDEODRIVER'] = 'dummy'
        environment['FRONTIERX_DATA_ROOT'] = data_root
        environment['XDG_CONFIG_HOME'] = str(self.root / 'xdg')
        environment['HOME'] = str(self.root / 'home')
        return subprocess.run(
            [EXECUTABLE, *map(str, arguments)], cwd=self.root, env=environment,
            text=True, capture_output=True, timeout=10)

    def test_hidden_smoke_lifecycle(self):
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '5', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line; required directories passed', result.stdout)
        self.assertIn('Preferences source: xdg-config-home; status: missing; entries: 0; '
                      'user-defaults: absent; settings-source: defaults', result.stdout)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertIn('game startup and rendering were not attempted', result.stdout)
        self.assertNotIn(str(self.root), result.stdout)
        stored = self.root / 'xdg' / 'frontierx' / 'preferences.dat'
        self.assertTrue(stored.is_file())
        self.assertTrue(stored.read_bytes().startswith(b'FXSET001'))

    def test_environment_root_and_startup_failures(self):
        result = self.run_shell('--smoke-ms', '1', '--hidden', data_root=str(self.data))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: environment', result.stdout)
        self.assertEqual(
            self.run_shell('--data-root', 'missing', '--smoke-ms', '1', '--hidden').returncode,
            2)
        (self.data / 'Objects').rmdir()
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Objects: missing', result.stderr)

    def test_configuration_root_and_precedence(self):
        config = self.root / 'settings' / 'native.ini'
        config.parent.mkdir()
        config.write_text(
            '[FrontierX]\nDataRoot=../Data\n[Libraries]\n; no dynamic modules\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: configuration', result.stdout)
        self.assertIn('Native component library configuration applied', result.stdout)

        result = self.run_shell('--config', config, '--data-root', self.data,
                                '--smoke-ms', '1', '--hidden', data_root='missing')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line', result.stdout)

        config.write_text('[FrontierX]\n')
        self.assertEqual(
            self.run_shell('--config', config, '--smoke-ms', '1', '--hidden').returncode,
            2)

        config.write_text(
            '[FrontierX]\nDataRoot=../Data\n[Libraries]\nmissing-module.so\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native component configuration failed', result.stderr)
        self.assertNotIn(str(self.root), result.stderr)

    def test_usage_contract(self):
        result = self.run_shell('--help')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('does not start or render the game', result.stdout)
        for arguments in [
                ('--hidden',), ('--smoke-ms', '0'), ('--smoke-ms', '60001'),
                ('--smoke-ms', 'bad'), ('--data-root',), ('--preferences',), ('--unknown',)]:
            self.assertEqual(self.run_shell(*arguments).returncode, 1, arguments)

    def test_preferences_owner_and_failures(self):
        preferences = self.root / 'preferences.dat'
        preferences.write_bytes(b'FXSET001' + struct.pack('<I', 0))
        result = self.run_shell('--data-root', self.data, '--preferences', preferences,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Preferences source: command-line; status: loaded; entries: 0; '
                      'user-defaults: absent; settings-source: defaults', result.stdout)
        self.assertNotIn(str(self.root), result.stdout)
        self.assertGreater(len(preferences.read_bytes()), 12)

        result = self.run_shell('--data-root', self.data, '--preferences', 'missing.dat',
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 5)
        self.assertIn('Preferences error: explicit preference file is missing', result.stderr)
        self.assertNotIn(str(self.root), result.stderr)

        preferences.write_bytes(b'invalid')
        result = self.run_shell('--data-root', self.data, '--preferences', preferences,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 5)
        self.assertIn('Preferences error:', result.stderr)
        self.assertNotIn(str(self.root), result.stderr)

    @unittest.skipUnless(EVENT_MODULE, 'production event module path not supplied')
    def test_configured_production_event_module(self):
        config = self.root / 'module.ini'
        config.write_text(
            f'[FrontierX]\nDataRoot={self.data}\n'
            f'[Libraries]\n"{EVENT_MODULE}"\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native component library configuration applied', result.stdout)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertNotIn(EVENT_MODULE, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
