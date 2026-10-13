"""Process tests for the native SDL shell using synthetic empty data directories."""
import os
from pathlib import Path
import subprocess
import struct
import shutil
import sys
import tempfile
import unittest
import wave

EXECUTABLE = str(Path(sys.argv.pop(1)).resolve())
EVENT_MODULE = str(Path(sys.argv.pop(1)).resolve()) if len(sys.argv) > 1 else ''
HOTKEY_TABLE = str(Path(sys.argv.pop(1)).resolve()) if len(sys.argv) > 1 else ''
GPU_ENABLED = sys.argv.pop(1) == '1' if len(sys.argv) > 1 else False
FONT_FILE = sys.argv.pop(1) if len(sys.argv) > 1 else ''


class NativeShellTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.data = self.root / 'Data'
        for directory in [
                'Objects', 'Interface', 'Textures', 'Mat', 'Profiles',
                'Maps/Campaign', 'Maps/Multiplayer', 'Music', 'SFX']:
            (self.data / directory).mkdir(parents=True)
        with wave.open(str(self.data / 'SFX/interface1.wav'), 'wb') as sound:
            sound.setnchannels(1)
            sound.setsampwidth(2)
            sound.setframerate(22050)
            sound.writeframes(b'\0\0' * 220)
        (self.data / 'Profiles/native-smoke.ini').write_text(
            '[NativeSmoke]\nValue = ready\n')

    def run_shell(self, *arguments, data_root='', video_driver='dummy'):
        environment = os.environ.copy()
        environment['SDL_VIDEODRIVER'] = video_driver
        environment['SDL_AUDIODRIVER'] = 'dummy'
        environment['FRONTIERX_DATA_ROOT'] = data_root
        environment['XDG_CONFIG_HOME'] = str(self.root / 'xdg')
        environment['HOME'] = str(self.root / 'home')
        return subprocess.run(
            [EXECUTABLE, *map(str, arguments)], cwd=self.root, env=environment,
            text=True, capture_output=True, timeout=10)

    def test_packed_campaign_mounts_and_required_materials(self):
        shutil.rmtree(self.data / 'Maps')
        names = b'/\0Campaign\0Multiplayer\0'
        name_offset = 56 + 3 * 44
        header = struct.pack('<14I', 0x20465455, 0x101, 56, 3 * 44, 0, 44,
                             name_offset, len(names), len(names),
                             name_offset + len(names), 0, 0, 0, 0)
        def entry(next_entry, name, child):
            return struct.pack('<11I', next_entry, name, 0x10, 0, child, 0, 0, 0, 0, 0, 0)
        (self.data / 'maps').write_bytes(
            header + entry(0, 0, 44) + entry(88, 2, 0) + entry(0, 11, 0) + names)
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native data archives mounted: 2', result.stdout)
        (self.data / 'Mat').rmdir()
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Mat: missing', result.stderr)
        self.assertNotIn('Maps/Campaign:', result.stderr)
        self.assertNotIn('Native shell opened', result.stdout)

        material_root = self.root / 'material-source'
        material_root.mkdir()
        result = self.run_shell('--data-root', self.data, '--material-root', material_root,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Native material source lacks directory: shaders', result.stderr)
        for directory in ('shaders', 'materials', 'textures'):
            (material_root / directory).mkdir()
        result = self.run_shell('--data-root', self.data, '--material-root', material_root,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native material source: explicit directory', result.stdout)
        self.assertIn('Native data archives mounted: 2', result.stdout)
        self.assertNotIn(str(material_root), result.stdout + result.stderr)
        (material_root / 'textures').rmdir()
        (material_root / 'textures').write_text('not a directory')
        result = self.run_shell('--data-root', self.data, '--material-root', material_root,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Native material source lacks directory: textures', result.stderr)

    def test_hidden_smoke_lifecycle(self):
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '5', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line; required data sources passed', result.stdout)
        self.assertIn('Preferences source: xdg-config-home; status: missing; entries: 0; '
                      'user-defaults: absent; settings-source: defaults', result.stdout)
        defaults_ready = result.stdout.find(
            'Native user defaults initialized before GameSystem startup')
        self.assertGreaterEqual(defaults_ready, 0)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertIn('retail game startup and rendering were not attempted', result.stdout)
        if GPU_ENABLED:
            game_ready = result.stdout.find(
                'Native GameSystem startup completed with synthetic profile')
            self.assertGreater(game_ready, defaults_ready)
        mission = self.data / 'Maps/Campaign/Sol_Train.dmission'
        (mission / 'ParseData').mkdir(parents=True)
        (mission / 'ParseData/Data.h').write_text('struct MT_GlobalData { int id; };')
        result = self.run_shell('--data-root', self.data,
                                '--mission-source', 'Sol_Train.dmission',
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native App parser schema initialization failed', result.stderr)
            profile = self.root / 'mission.ini'
            profile.write_text('[System]\nIEventSystem\nIStreamer\n'
                               'IWindowManager = WindowManager\n'
                               'IViewConstructor\nIParticleManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--mission-engine', '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App mission Engine components or IRenderer unavailable',
                          result.stderr)
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App mission global data unavailable', result.stderr)
            (mission / 'ParseData/Data.h').write_text('static_assert(1);')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App mission ParseData invalid', result.stderr)
            (mission / 'ParseData/Data.h').write_text(
                'struct MT_GlobalData { int id; };')
            (mission / 'MT_GlobalData').mkdir()
            (mission / 'MT_GlobalData/Globals').write_bytes(b'\x01\x00\x00\x00')
            save_root = self.root / 'SavedGame'
            save_root.mkdir()
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--save-root', save_root,
                                    '--skip-notification-cue',
                                    '--sfx-smoke',
                                    *(['--font-smoke', FONT_FILE] if FONT_FILE else []),
                                    '--profile-smoke', 'native-smoke.ini',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            if FONT_FILE:
                self.assertIn('Native source teletype counted string, pause, expiry and flush checked', result.stdout)
                self.assertIn('subtitle wrapping and release checked', result.stdout)
                self.assertIn('status/hint/scrolling lifecycle checked', result.stdout)
                self.assertIn('hot-rectangle input/post/lifecycle checked', result.stdout)
                self.assertIn('original VFX image reader checked', result.stdout)
                self.assertIn('unit alert lifecycle checked', result.stdout)
            if GPU_ENABLED:
                self.assertIn('Native effect-animation randomization and RNG consumption checked', result.stdout)
            self.assertIn('Native App parser schema loaded from embedded data.i',
                          result.stdout)
            self.assertIn('Native App mission source opened through SPMAPDIR', result.stdout)
            self.assertIn('Native App mission IStreamer initialized with Music and SFX sources',
                          result.stdout)
            self.assertIn('Native App source string resources initialized', result.stdout)
            self.assertIn('Native App writable save directory initialized outside retail data',
                          result.stdout)
            self.assertIn('Native App text resources initialized', result.stdout)
            self.assertIn('Native App status: Disabled', result.stderr)
            self.assertIn('Native App hint: Disabled', result.stderr)
            self.assertIn('Native App message: Disabled', result.stderr)
            self.assertIn('Native App message: Native message', result.stderr)
            self.assertIn('scrolling text initialized; notification cue explicitly disabled',
                          result.stdout)
            self.assertIn('Native App mission progress owner initialized', result.stdout)
            self.assertIn('Native mission loading: 5%', result.stderr)
            self.assertIn('Native App MusicManager initialized', result.stdout)
            self.assertIn('Native ISFX BUTTONPRESS1 preload/open/play/stop checked', result.stdout)
            self.assertIn('Native mission sound retains EOF until actual completion and owner cleanup checked',
                          result.stdout)
            if GPU_ENABLED:
                self.assertIn('Native operation storage alignment/copy/load checked', result.stdout)
                self.assertIn('Native objective storage compaction/state checked', result.stdout)
            self.assertIn('Native App PROFILEDIR CreateProfileParser checked', result.stdout)
            self.assertIn('mission not started', result.stdout)
            (self.data / 'SFX/interface1.wav').unlink()
            (self.data / 'SFX').rmdir()
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--save-root', save_root,
                                    '--skip-notification-cue',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('mission audio source or IStreamer unavailable', result.stderr)
            (self.data / 'SFX').mkdir()
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--save-root', self.data,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App save-directory binding failed', result.stderr)
            profile.write_text('[System]\nIEventSystem\n'
                               'IWindowManager = WindowManager\n'
                               'IViewConstructor\nIParticleManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('mission audio source or IStreamer unavailable', result.stderr)
            profile.write_text('[System]\nIEventSystem\nIStreamer\n'
                               'IWindowManager = WindowManager\n'
                               'IViewConstructor\nIParticleManager\n[Engine]\n')
            invalid_gen_data = self.root / 'invalid-gendata.db'
            invalid_gen_data.write_bytes(b'not a UTF archive')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--gen-data', invalid_gen_data,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App general data archive invalid', result.stderr)
            invalid_game_types = self.root / 'invalid-gametypes.db'
            invalid_game_types.write_bytes(b'not a UTF archive')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--game-types', invalid_game_types,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App game-types archive invalid', result.stderr)
        self.assertNotIn(str(self.root), result.stdout)
        stored = self.root / 'xdg' / 'frontierx' / 'preferences.dat'
        self.assertTrue(stored.is_file())
        self.assertTrue(stored.read_bytes().startswith(b'FXSET001'))

    def test_environment_root_and_startup_failures(self):
        result = self.run_shell('--smoke-ms', '1', '--hidden', data_root=str(self.data))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: environment', result.stdout)
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden',
                                video_driver='frontierx_missing_driver')
        self.assertEqual(result.returncode, 3 if GPU_ENABLED else 4)
        self.assertIn('Native game-system startup failed' if GPU_ENABLED else
                      'Native window-manager startup failed', result.stderr)
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
        self.assertIn('render smoke draws a repository fixture', result.stdout)
        self.assertIn('game-scene renders one supplied UTF mesh', result.stdout)
        for arguments in [
                ('--hidden',), ('--smoke-ms', '0'), ('--smoke-ms', '60001'),
                ('--smoke-ms', 'bad'), ('--data-root',), ('--preferences',),
                ('--hotkeys',), ('--system-profile',), ('--render-smoke',),
                ('--game-types',), ('--game-types', 'types.db'),
                ('--animation-object',), ('--animation-object', 'Forger'),
                ('--mesh-object',), ('--mesh-object', '../Forger'),
                ('--mission-source',), ('--mission-source', '../Sol_Train.dmission'),
                ('--mission-source', 'Sol_Train.mission'),
                ('--mission-start',),
                ('--mission-briefing',),
                ('--mission-briefing', '--mission-briefing'),
                ('--mission-briefing', '--mission-start', '--hidden'),
                ('--skip-sector-map',),
                ('--skip-space-background',),
                ('--skip-unit-labels',),
                ('--mission-alert-smoke',),
                ('--mission-hint-check',),
                ('--base-textures-only',),
                ('--skip-legacy-lighting',),
                ('--skip-training-speech',),
                ('--skip-mission-music',),
                ('--mission-start', '--mission-source', 'Sol_Train.dmission'),
                ('--render-smoke', 'vertex.spv'),
                ('--render-smoke', 'vertex.spv', '--hidden'), ('--unknown',)]:
            self.assertEqual(self.run_shell(*arguments).returncode, 1, arguments)
        for arguments in [
                ('--game-scene',),
                ('--game-scene', 'materials.utf', 'mesh.utf', 'v.spv',
                 'f.spv', 't4.spv', '--hidden'),
                ('--render-smoke', 'v.spv', 'f.spv', '--game-scene',
                 'materials.utf', 'mesh.utf', 'v.spv', 'f.spv', 't4.spv', 'mt.spv')]:
            self.assertEqual(self.run_shell(*arguments).returncode, 1, arguments)

    def test_production_animation_channel_from_object_directory(self):
        # The real Anim::LoadChannels caller must create IChannel2 and reject
        # corrupt frame data; a registered factory alone would miss this gate.
        object_dir = self.data / 'Objects' / 'Forger'
        channel_dir = object_dir / 'Animation' / 'Chnl' / 'Track'
        script_dir = object_dir / 'Animation' / 'Script'
        channel_dir.mkdir(parents=True)
        script_dir.mkdir()
        (channel_dir / 'Header').write_bytes(struct.pack('<IfI', 2, 1.0, 1))
        (channel_dir / 'Frames').write_bytes(struct.pack('<ff', 0.0, 10.0))
        (script_dir / 'Move').write_bytes(
            b'\0' * 256 + b'Track\0'.ljust(128, b'\0'))
        profile = self.root / 'animation.ini'
        profile.write_text('[System]\nIEventSystem\nIWindowManager = WindowManager\n'
                           '[Engine]\nIAnimation\n')
        arguments = ('--data-root', self.data, '--system-profile', profile,
                     '--animation-object', 'Forger', 'Move',
                     '--smoke-ms', '1', '--hidden')
        result = self.run_shell(*arguments)
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 6,
                         result.stdout + result.stderr)
        if not GPU_ENABLED:
            return
        self.assertIn('Native animation script loaded; scripts: 1; duration: 1',
                      result.stdout)
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--mesh-object', 'Forger', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        self.assertIn('Native mesh archetype load failed', result.stderr)
        project_root = Path(__file__).resolve().parents[2]
        result = self.run_shell(
            '--data-root', self.data, '--material-root', project_root / 'Data/mat',
            '--system-profile', project_root / 'Code/App/Src/Conquest.ini',
            '--skip-movies', '--mesh-object', 'Forger', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        self.assertIn('Native GameSystem startup completed with supplied profile',
                      result.stdout)
        self.assertIn('IMeshManager returned [OK]', result.stdout + result.stderr)
        self.assertIn('Native mesh archetype load failed', result.stderr)
        (channel_dir / 'Frames').write_bytes(struct.pack('<f', 0.0))
        result = self.run_shell(*arguments)
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        self.assertIn('Native animation script-set load failed', result.stderr)

    def test_render_smoke_rejects_missing_shaders_before_gpu_startup(self):
        result = self.run_shell(
            '--data-root', self.data,
            '--render-smoke', 'missing.vert.spv', 'missing.frag.spv',
            '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 7)
        if GPU_ENABLED:
            self.assertIn('Native render initialization failed: could not open shader',
                          result.stderr)
        else:
            self.assertIn('Native render smoke is unavailable in this build',
                          result.stderr)
        self.assertNotIn(str(self.root), result.stdout + result.stderr)

    @unittest.skipUnless(HOTKEY_TABLE, 'production hotkey table path not supplied')
    def test_production_hotkey_pipeline(self):
        result = self.run_shell('--data-root', self.data, '--hotkeys', HOTKEY_TABLE,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native hotkey table loaded and event pipeline connected', result.stdout)
        self.assertNotIn(HOTKEY_TABLE, result.stdout + result.stderr)

        missing = self.root / 'missing-hotkeys.utf'
        result = self.run_shell('--data-root', self.data, '--hotkeys', missing,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6)
        self.assertIn('Native hotkey table error:', result.stderr)
        self.assertNotIn(str(missing), result.stdout + result.stderr)

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

    def test_supplied_system_profile_and_loader_failures(self):
        profile = self.root / 'system.ini'
        profile.write_text(
            '[System]\nIEventSystem\nIWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\nVertexBufferManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\nVertexBufferManager\nIStringTable\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\nVertexBufferManager\nIStringTable\nIMaterialManager\nITManager\n'
            '[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('IMaterialManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('ITManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)

        profile.write_text('[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn('Native shell opened', result.stdout)

        profile.write_text(f'[System]\n{self.root}/MissingSystemComponent\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn('Native shell opened', result.stdout)
        self.assertNotIn(str(self.root), result.stdout + result.stderr)

        profile.write_text('[System]\nIEventSystem\nIStreamer\n'
                           'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('IStreamer returned [OK]', result.stdout + result.stderr)

        for component in ('IVideoSystem',):
            profile.write_text(
                '[System]\nIEventSystem\n' + component + '\nIWindowManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 3)
            self.assertIn(component + ' returned [FAILED]', result.stdout + result.stderr)
            self.assertNotIn('IWindowManager returned [OK]', result.stdout + result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)

        if GPU_ENABLED:
            profile.write_text('[System]\nIEventSystem\nIParticleManager\n'
                               'IWindowManager = WindowManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn('IParticleManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('point-emitter program created', result.stdout)

        # The existing App movie flag excludes only full-screen movie service creation.
        profile.write_text('[System]\nIEventSystem\nIStreamer\nIVideoSystem\n'
                           'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--skip-movies', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('IVideoSystem [DISABLED by --skip-movies]', result.stdout + result.stderr)
        self.assertIn('IStreamer returned [OK]', result.stdout + result.stderr)
        self.assertNotIn('IVideoSystem returned [OK]', result.stdout + result.stderr)
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--skip-movies', '--skip-movies', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 1)
        self.assertIn('duplicate option: --skip-movies', result.stderr)
        profile.write_text('[System]\nIEventSystem\nIVideoSystem\nIParticleManager\nIMeshManager\n'
                           'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--skip-movies', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3)
        if GPU_ENABLED:
            self.assertIn('IParticleManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('IMeshManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('Native shell opened', result.stdout)
        else:
            self.assertIn('IParticleManager returned [FAILED]', result.stdout + result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)

        # Engine aggregates from the production animation and hardpoint owners
        # must initialize and tear down through GameSystem, even without a scene.
        profile.write_text('[System]\nIEventSystem\nIWindowManager = WindowManager\n'
                           '[Engine]\nIAnimation\nIHardpoint\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell closed cleanly', result.stdout)

            render_profile = (
                '[System]\nIEventSystem\nIRenderPrimitive\n'
                'IRenderPipeline = RendSft\nTextureLibrary\n'
                'IWindowManager = WindowManager\n[Engine]\nIRenderer\n'
                '[RenderManager]\n')
            profile.write_text(render_profile + 'Optics\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)

            profile.write_text(render_profile + 'MissingRenderComponent\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 3, result.stdout + result.stderr)
            self.assertIn('required render component is not available',
                          result.stdout + result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)

        profile.write_text(
            '[System]\nIEventSystem\nIWindowManager = WindowManager\n'
            f'[Engine]\n{self.root}/MissingEngineComponent\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        if GPU_ENABLED:
            self.assertEqual(result.returncode, 3)
            self.assertIn('Native game-system startup failed', result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)
            self.assertNotIn(str(self.root), result.stdout + result.stderr)

        result = self.run_shell('--data-root', self.data, '--system-profile',
                                self.root / 'missing.ini', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native system profile read failed', result.stderr)
        self.assertNotIn(str(self.root), result.stdout + result.stderr)

        profile.write_bytes(b'x' * (1024 * 1024 + 1))
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native system profile size is invalid', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

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
