"""Process tests for the native SDL shell using synthetic empty data directories."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

EXECUTABLE = str(Path(sys.argv.pop(1)).resolve())


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
        return subprocess.run(
            [EXECUTABLE, *map(str, arguments)], cwd=self.root, env=environment,
            text=True, capture_output=True, timeout=10)

    def test_hidden_smoke_lifecycle(self):
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '5', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line; required directories passed', result.stdout)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertIn('game startup and rendering were not attempted', result.stdout)
        self.assertNotIn(str(self.root), result.stdout)

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
        config.write_text('[FrontierX]\nDataRoot=../Data\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: configuration', result.stdout)

        result = self.run_shell('--config', config, '--data-root', self.data,
                                '--smoke-ms', '1', '--hidden', data_root='missing')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line', result.stdout)

        config.write_text('[FrontierX]\n')
        self.assertEqual(
            self.run_shell('--config', config, '--smoke-ms', '1', '--hidden').returncode,
            2)

    def test_usage_contract(self):
        result = self.run_shell('--help')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('does not start or render the game', result.stdout)
        for arguments in [
                ('--hidden',), ('--smoke-ms', '0'), ('--smoke-ms', '60001'),
                ('--smoke-ms', 'bad'), ('--data-root',), ('--unknown',)]:
            self.assertEqual(self.run_shell(*arguments).returncode, 1, arguments)


if __name__ == '__main__':
    unittest.main()
