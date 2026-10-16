"""Mandatory execution gates reject skips, absent scopes and failures."""
import contextlib
import argparse
import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import os
import re
import shutil
import json
import subprocess
import sys

import check_ctest
import private_workspace
import cross_m3
import gpu_host_setup
import native_cross_m3
import check_readiness
import mission_m3
import playtest
import linux_m3


class GateTests(unittest.TestCase):
    # P0 regression: the original workflow always failed at M3 acceptance.
    # Protect the existing CI drivers' independent technical scope and strict
    # missing/failing/skipped gates without running a parallel game implementation.
    def test_technical_aggregate_requires_execution_and_reports_absent_hosts(self):
        env = dict(CPU_RESULT='success', WINDOWS_RESULT='success', HOST_SMOKES='true',
                   GPU_RESULT='success', MISSION_RESULT='success',
                   MISSION_SANITIZER_RESULT='success')
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(check_readiness.main(env), 0)
            for key in ('CPU_RESULT', 'WINDOWS_RESULT', 'GPU_RESULT', 'MISSION_RESULT',
                        'MISSION_SANITIZER_RESULT'):
                for result in ('failure', 'cancelled', 'skipped', ''):
                    self.assertEqual(check_readiness.main(dict(env, **{key: result})), 1)
        env.update(HOST_SMOKES='false', GPU_RESULT='skipped', MISSION_RESULT='skipped',
                   MISSION_SANITIZER_RESULT='skipped')
        with contextlib.redirect_stdout(io.StringIO()) as output:
            self.assertEqual(check_readiness.main(env), 0)
        self.assertIn('NOT RUN / UNTESTED', output.getvalue())

    def test_technical_smoke_rejects_early_successful_close(self):
        with tempfile.TemporaryDirectory() as tmp:
            log = Path(tmp) / 'process.log'
            markers = ['Native App selected training checkpoint preserved after source reload',
                       'Native App source mission closed after checkpoint']
            for text in ('', markers[0], markers[1], 'Native App regular close: mission audio streams=0'):
                log.write_text(text)
                with self.assertRaises(RuntimeError):
                    mission_m3.check_technical_smoke(log)
            log.write_text('\n'.join(markers))
            mission_m3.check_technical_smoke(log)

    def test_terran_smoke_rejects_early_close_and_missing_source_loop(self):
        with tempfile.TemporaryDirectory() as tmp:
            log = Path(tmp) / 'process.log'
            markers = ['Native App selected source script: script01.dll',
                       'Native App first Terran bounded source loop completed',
                       'Native App regular close: mission audio streams=0; hint ownership=idle']
            for excluded in range(len(markers)):
                log.write_text('\n'.join(marker for index, marker in enumerate(markers)
                                         if index != excluded))
                with self.assertRaises(RuntimeError):
                    mission_m3.check_terran_smoke(log)
            log.write_text('\n'.join(markers))
            mission_m3.check_terran_smoke(log)
            private = Path(tmp) / 'private'
            private.mkdir()
            args = argparse.Namespace(build=Path('/fixture/build'))
            def execute(command, **kwargs):
                self.assertEqual(kwargs['cwd'], private)
                kwargs['stdout'].write(('\n'.join(markers)).encode())
                return subprocess.CompletedProcess(command, 0)
            with patch('mission_m3.create', return_value=private), \
                 patch('playtest.mission_command', return_value=['/fixture/game', '--mission-campaign']), \
                 patch('mission_m3.subprocess.run', side_effect=execute), \
                 contextlib.redirect_stdout(io.StringIO()):
                mission_m3.run_terran_smoke(Path('/fixture/source'), args,
                                          Path('/fixture/retail'), '/fixture/font')

    def test_terran_full_launch_requires_no_presentation_exclusions(self):
        with tempfile.TemporaryDirectory() as tmp:
            install = Path(tmp)
            (install / 'DATA').mkdir()
            for name in ('stringpack.db', 'gendata.db', 'gametypes.db'):
                (install / name).touch()
            args = (Path('/fixture/source'), Path('/fixture/build'), install,
                    Path('/fixture/font'), Path('/fixture/save'))
            with self.assertRaises(ValueError):
                playtest.mission_command(*args, mission='terran1')
            full = playtest.mission_command(*args, mission='terran1', font_mapping=Path('/fixture/font-map'))
            self.assertIn('--ui-font-map', full)
            self.assertNotIn('--ui-font', full)
            self.assertIn('T_Mission1.dmission', full)
            self.assertIn('--mission-campaign', full)
            self.assertFalse(any(option.startswith('--skip-') or option == '--base-textures-only'
                                 for option in full))
            bounded = playtest.mission_command(*args, mission='terran1', bounded_presentation=True)
            self.assertIn('--skip-animated-portraits', bounded)
            self.assertIn('--skip-movies', bounded)

    def test_technical_driver_keeps_scopes_and_skips_comparisons(self):
        with tempfile.TemporaryDirectory() as tmp:
            for scope in ('default', 'optional', 'sanitizers', 'gpu', 'mission', 'mission-sanitizers'):
                calls = []
                with patch.object(sys, 'argv', ['linux_m3.py', '--compiler', 'clang',
                                               '--scope', scope, '--track', 'technical']), \
                     patch('linux_m3.create', return_value=Path(tmp)), \
                     patch('linux_m3.subprocess.check_output', return_value='fixture'), \
                     patch('linux_m3.run', side_effect=lambda command, *rest: calls.append(command)), \
                     patch.dict(os.environ, {'FRONTIERX_M3_GPU_SETUP': '',
                                             'FRONTIERX_LGPL_PREFIX': tmp,
                                             'FRONTIERX_TEST_FONT_FILE': __file__}), \
                     contextlib.redirect_stdout(io.StringIO()):
                    linux_m3.main()
                self.assertIn('--build', calls[1])
                self.assertTrue(any('check_ctest.py' in part for call in calls for part in call))
                self.assertFalse(any('--trace' in call for call in calls))
                ctest = calls[2]
                self.assertIn('host', ctest)
                self.assertEqual(ctest[ctest.index('host') - 1], '-L' if scope == 'gpu' else '-LE')
                if scope == 'default':
                    self.assertNotIn('-DFRONTIERX_BUILD_NATIVE_FONTS=ON', calls[0])
                    self.assertNotIn('-DFRONTIERX_BUILD_FFMPEG_VIDEO=ON', calls[0])
                else:
                    self.assertIn('-DFRONTIERX_BUILD_NATIVE_FONTS=ON', calls[0])
                    self.assertIn('-DFRONTIERX_BUILD_FFMPEG_AUDIO=ON', calls[0])
                    self.assertIn('-DFRONTIERX_BUILD_FFMPEG_VIDEO=ON', calls[0])
                if scope == 'sanitizers':
                    self.assertIn('-R', calls[2])
                    pattern = ctest[ctest.index('-R') + 1]
                    for owner in ('script02_schema', 'script03_schema', 'script04_schema', 'script05_schema', 'net_buffer'):
                        self.assertIsNotNone(re.fullmatch(pattern, 'portability.' + owner))
                if scope.startswith('mission'):
                    self.assertIn('--technical-smoke', calls[-1])
                if scope == 'optional':
                    self.assertIn('--handoff', calls[-1])
                    self.assertIn('terran1', calls[-1])

    def test_candidate_records_binary_and_rejects_dirty_ci_source(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            build = root / 'build'
            build.mkdir()
            (build / 'frontierx_native_shell').write_bytes(b'synthetic binary')
            (build / 'CMakeCache.txt').write_text('synthetic configuration')
            record = root / 'candidate.json'
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b' M fixture']):
                with self.assertRaises(ValueError):
                    playtest.write_handoff(root, build, record, {'GITHUB_RUN_ID': '123'})
            self.assertFalse(record.exists())
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b'']):
                playtest.write_handoff(root, build, record, {'GITHUB_RUN_ID': '123'})
            data = json.loads(record.read_text())
            self.assertEqual(data['revision'], 'a' * 40)
            self.assertFalse(data['working_tree_dirty'])
            self.assertEqual(data['binary'], playtest.digest(build / 'frontierx_native_shell'))
            self.assertIn('--play', data['launch_command'])
            self.assertIn('--candidate', data['launch_command'])
            self.assertIn(str(record.resolve()), data['launch_command'])
            self.assertIn('FRONTIERX_BUILD_FFMPEG_VIDEO', data['configuration'])
            self.assertIn('MPEG video/audio parsers', data['external_dependencies']['video'])

    def test_candidate_launch_rejects_scope_and_configuration_before_external_inputs(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            build = root / 'build'
            build.mkdir()
            (build / 'frontierx_native_shell').write_bytes(b'synthetic binary')
            (build / 'CMakeCache.txt').write_text('synthetic configuration')
            candidate = root / 'candidate.json'
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b'']):
                playtest.write_handoff(root, build, candidate, {}, mission='terran1')
            original = json.loads(candidate.read_text())
            self.assertEqual(original['checklist'], playtest.TERRAN_CHECKLIST)
            argv = ['playtest.py', '--build', str(build), '--mission', 'terran1',
                    '--candidate', str(candidate), '--play']
            for key, bad in [('mission', 'training'), ('bounded_presentation', True),
                             ('binary', {'bytes': 0, 'sha256': '0' * 64}),
                             ('cmake_cache', {'bytes': 0, 'sha256': '0' * 64})]:
                candidate.write_text(json.dumps(dict(original, **{key: bad})))
                with patch.object(sys, 'argv', argv), patch.dict(os.environ, {}, clear=True):
                    with self.assertRaises(ValueError):
                        playtest.main()
            candidate.unlink()
            with patch.object(sys, 'argv', argv), patch.dict(os.environ, {}, clear=True):
                with self.assertRaises(ValueError):
                    playtest.main()

    def test_p2_candidate_pins_target_without_bypassing_campaign_entry(self):
        # Regression: the earlier handoff always described TM1/P1c even when
        # the genuine player checkpoint and requested playtest belonged to TM2.
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            build = root / 'build'
            build.mkdir()
            (build / 'frontierx_native_shell').write_bytes(b'synthetic binary')
            (build / 'CMakeCache.txt').write_text('synthetic configuration')
            candidate = root / 'candidate.json'
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b'']):
                playtest.write_handoff(root, build, candidate, {}, mission='terran1', campaign_mission=2)
            record = json.loads(candidate.read_text())
            self.assertEqual(record['campaign_mission'], 2)
            self.assertIn('--campaign-mission 2', record['launch_command'])
            self.assertIn(playtest.P2_TM2_CHECKLIST[0], record['checklist'])
            self.assertNotIn('does not accept P1c', record['acceptance'])
            argv = ['playtest.py', '--build', str(build), '--mission', 'terran1',
                    '--candidate', str(candidate), '--play']
            for target in ([], ['--campaign-mission', '3']):
                with patch.object(sys, 'argv', argv + target), patch.dict(os.environ, {}, clear=True):
                    with self.assertRaisesRegex(ValueError, 'campaign target'):
                        playtest.main()
            with self.assertRaisesRegex(ValueError, 'Terran selector'):
                playtest.write_handoff(root, build, candidate, {}, campaign_mission=2)
            install = root / 'retail'
            (install / 'DATA').mkdir(parents=True)
            for name in ('stringpack.db', 'gendata.db', 'gametypes.db'):
                (install / name).touch()
            # A matching target reaches the ordinary game launch unchanged.
            (build / 'CMakeCache.txt').write_text('FRONTIERX_BUILD_FFMPEG_VIDEO:BOOL=ON\n')
            mapping = root / 'font-map'
            mapping.write_text('# synthetic external map\n')
            env = dict(FRONTIERX_RETAIL_ROOT=str(install),
                       FRONTIERX_PLAYTEST_FONT_MAP=str(mapping),
                       FRONTIERX_PLAYTEST_SAVE_ROOT=str(root / 'save'),
                       XDG_CONFIG_HOME=str(root / 'config'))
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b'']):
                playtest.write_handoff(root, build, candidate, env, mission='terran1', campaign_mission=2)
            with patch.object(sys, 'argv', argv + ['--campaign-mission', '2']), \
                 patch.dict(os.environ, env, clear=True), patch('playtest.subprocess.run') as launch:
                playtest.main()
                command = launch.call_args.args[0]

            self.assertEqual(command[command.index('--mission-source') + 1], 'T_Mission1.dmission')
            self.assertIn('--mission-campaign', command)
            self.assertNotIn('--campaign-mission', command)
            self.assertFalse(any(value.startswith('--skip-') or value == '--base-textures-only'
                                 for value in command))

    def test_tm4_checklist_preserves_campaign_entry_and_generic_acceptance(self):
        checklist = playtest.campaign_checklist('terran1', 4)
        self.assertEqual(checklist, playtest.P2_CHECKLIST + playtest.P2_TM4_CHECKLIST)
        self.assertNotIn(playtest.P2_TM2_CHECKLIST[0], checklist)
        self.assertEqual(playtest.MISSIONS['terran1'], 'T_Mission1.dmission')
        with self.assertRaisesRegex(ValueError, 'Terran selector'):
            playtest.campaign_checklist('training', 4)

    def test_playtest_launch_preserves_player_input_and_four_shader_order(self):
        with tempfile.TemporaryDirectory() as tmp:
            install = Path(tmp)
            (install / 'DATA').mkdir()
            for name in ('stringpack.db', 'gendata.db', 'gametypes.db'):
                (install / name).touch()
            command = playtest.mission_command(Path('/fixture/source'), Path('/fixture/build'),
                                               install, Path('/fixture/font'), Path('/fixture/save'))
            self.assertIn('--mission-campaign', command)
            self.assertNotIn('--smoke-ms', command)
            shaders = command[command.index('--mission-gpu') + 1:][:4]
            self.assertEqual([Path(p).name for p in shaders],
                             ['sdl3_gpu_probe.vert.spv', 'sdl3_gpu_probe.frag.spv',
                              'sdl3_gpu_probe_t4.vert.spv', 'sdl3_gpu_probe_mt.vert.spv'])
            # Pin the CLI's production vertex-layout contract, not only the
            # helper's previous self-consistent (but reversed) argument list.
            source = Path(playtest.__file__).resolve().parents[2]
            shell = (source / 'Code/Launcher/NativeShell.cpp').read_text()
            branch = shell.split('if (option == "--mission-gpu")', 1)[1].split('continue;', 1)[0]
            self.assertLess(branch.index('options.gameExtendedShaderPath = argv[++index]'),
                            branch.index('options.gameVertexShaderPath = argv[++index]'))
            shader_root = source / 'Tools/portability/shaders'
            self.assertIn('layout(location = 2) in vec2 in_texture_coordinate;',
                          (shader_root / 'sdl3_gpu_probe_t4.vert').read_text())
            game_shader = (shader_root / 'sdl3_gpu_probe_mt.vert').read_text()
            self.assertIn('layout(location = 2) in vec4 in_specular_bgra;', game_shader)
            self.assertIn('layout(location = 3) in vec2 in_texture_coordinate;', game_shader)
            self.assertEqual(mission_m3.MISSION_GPU_SHADER_SUFFIXES, playtest.MISSION_GPU_SHADER_SUFFIXES)
            with tempfile.TemporaryDirectory() as private_tmp:
                private = Path(private_tmp)
                save, config = private / 'save', private / 'config'
                launch_env = dict(FRONTIERX_RETAIL_ROOT=str(install),
                                  FRONTIERX_TEST_FONT_FILE='/fixture/font',
                                  FRONTIERX_PLAYTEST_SAVE_ROOT=str(save),
                                  XDG_CONFIG_HOME=str(config))
                argv = ['playtest.py', '--build', str(install / 'build'), '--play']
                with patch.object(sys, 'argv', argv), patch.dict(os.environ, launch_env, clear=True), \
                     patch('playtest.subprocess.run') as launch:
                    playtest.main()
                self.assertEqual(launch.call_args.kwargs['cwd'], save / 'runtime')
                self.assertTrue(launch.call_args.kwargs['check'])
                self.assertTrue((save / 'runtime').is_dir())
                self.assertEqual((save / 'runtime').stat().st_mode & 0o077, 0)
                (save / 'runtime').rmdir()
                (save / 'runtime').symlink_to(Path(playtest.__file__).resolve().parents[2],
                                             target_is_directory=True)
                with patch.object(sys, 'argv', argv), patch.dict(os.environ, launch_env, clear=True), \
                     patch('playtest.subprocess.run') as launch:
                    with self.assertRaises(ValueError):
                        playtest.main()
                launch.assert_not_called()
            env = playtest.play_environment(dict(FRONTIERX_M3_FIXED_FRAME='1',
                                                FRONTIERX_M3_TRACE='private', DISPLAY='fixture'))
            self.assertEqual(env, {'DISPLAY': 'fixture'})

    def test_full_terran_rejects_video_free_build_before_launch(self):
        with tempfile.TemporaryDirectory() as tmp:
            private = Path(tmp)
            build = private / 'build'; build.mkdir()
            (build / 'CMakeCache.txt').write_text('FRONTIERX_BUILD_FFMPEG_VIDEO:BOOL=OFF\n')
            mapping = private / 'fonts.map'; mapping.write_text('Arial,400=/external/arial.ttf\n')
            env = dict(FRONTIERX_RETAIL_ROOT=str(private / 'retail'),
                       FRONTIERX_PLAYTEST_FONT_MAP=str(mapping))
            argv = ['playtest.py', '--build', str(build), '--mission', 'terran1', '--play']
            with patch.object(sys, 'argv', argv), patch.dict(os.environ, env, clear=True), \
                 patch('playtest.subprocess.run') as launch:
                with self.assertRaisesRegex(ValueError, 'optional native movie build'):
                    playtest.main()
                launch.assert_not_called()

    def test_single_font_interim_enables_all_media_and_preserves_candidate_scope(self):
        with tempfile.TemporaryDirectory() as tmp:
            private = Path(tmp)
            install = private / 'retail'; install.mkdir(); (install / 'DATA').mkdir()
            for name in ('stringpack.db', 'gendata.db', 'gametypes.db'): (install / name).touch()
            build = private / 'build'; build.mkdir()
            (build / 'frontierx_native_shell').write_bytes(b'synthetic binary')
            (build / 'CMakeCache.txt').write_text('FRONTIERX_BUILD_FFMPEG_VIDEO:BOOL=ON\n')
            font = private / 'font.ttf'; font.write_bytes(b'synthetic external font identity')
            command = playtest.mission_command(Path('/fixture/source'), build, install, font,
                private / 'save', mission='terran1', single_font_interim=True)
            self.assertIn('--ui-font', command)
            self.assertNotIn('--ui-font-map', command)
            self.assertFalse(any(arg.startswith('--skip-') or arg == '--base-textures-only' for arg in command))
            self.assertNotIn('--smoke-ms', command)
            with self.assertRaises(ValueError):
                playtest.mission_command(Path('/fixture/source'), build, install, font,
                    private / 'save', single_font_interim=True)
            with self.assertRaises(ValueError):
                playtest.mission_command(Path('/fixture/source'), build, install, font,
                    private / 'save', mission='terran1', bounded_presentation=True, single_font_interim=True)
            with patch.object(sys, 'argv', ['playtest.py', '--build', str(build), '--mission', 'terran1',
                        '--single-font-interim', '--bounded-presentation', '--play']), contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit) as rejected:
                    playtest.main()
                self.assertEqual(rejected.exception.code, 2)
            candidate = private / 'candidate.json'
            env = dict(FRONTIERX_RETAIL_ROOT=str(install), FRONTIERX_TEST_FONT_FILE=str(font),
                       FRONTIERX_PLAYTEST_SAVE_ROOT=str(private / 'save'), XDG_CONFIG_HOME=str(private / 'config'))
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b'']):
                playtest.write_handoff(private, build, candidate, env, mission='terran1', single_font_interim=True)
            record = json.loads(candidate.read_text())
            self.assertTrue(record['single_font_interim'])
            self.assertFalse(record['bounded_presentation'])
            self.assertIn('--single-font-interim', record['launch_command'])
            self.assertEqual(record['single_font_fingerprint'], playtest.digest(font))
            self.assertTrue(any('Arial Narrow' in item for item in record['known_limits']))
            self.assertFalse(any(item in playtest.EXCLUSIONS for item in record['known_limits']))
            argv = ['playtest.py', '--build', str(build), '--mission', 'terran1', '--candidate', str(candidate), '--play']
            with patch.object(sys, 'argv', argv), patch.dict(os.environ, env, clear=True), patch('playtest.subprocess.run') as launch:
                with self.assertRaisesRegex(ValueError, 'typography scope'):
                    playtest.main()
                launch.assert_not_called()
            argv += ['--single-font-interim']
            with patch.object(sys, 'argv', argv), patch.dict(os.environ, env, clear=True), patch('playtest.subprocess.run') as launch:
                playtest.main()
                self.assertFalse(any(arg.startswith('--skip-') or arg == '--base-textures-only' for arg in launch.call_args.args[0]))
                self.assertEqual(launch.call_args.kwargs['cwd'], private / 'save/runtime')
            with patch.object(sys, 'argv', argv + ['--no-frame-limit']), patch.dict(os.environ, env, clear=True), patch('playtest.subprocess.run') as launch:
                with self.assertRaisesRegex(ValueError, 'frame-limiting option'):
                    playtest.main()
                launch.assert_not_called()
            uncapped = private / 'candidate-uncapped.json'
            with patch('playtest.subprocess.check_output', side_effect=['a' * 40, b'']):
                playtest.write_handoff(private, build, uncapped, env, mission='terran1',
                    single_font_interim=True, no_frame_limit=True)
            uncapped_argv = [str(uncapped) if value == str(candidate) else value for value in argv]
            with patch.object(sys, 'argv', uncapped_argv + ['--no-frame-limit']), patch.dict(os.environ, env, clear=True), patch('playtest.subprocess.run') as launch:
                playtest.main()
                self.assertIn('--no-frame-limit', launch.call_args.args[0])
                self.assertFalse(any(arg.startswith('--skip-') or arg == '--base-textures-only' for arg in launch.call_args.args[0]))
            font.write_bytes(b'changed external font identity')
            with patch.object(sys, 'argv', argv), patch.dict(os.environ, env, clear=True), patch('playtest.subprocess.run') as launch:
                with self.assertRaisesRegex(ValueError, 'interim font changed'):
                    playtest.main()
                launch.assert_not_called()

    def test_native_cross_rejects_stale_dirty_and_outside_windows_evidence(self):
        config = dict(target='fixture@example', key='/fixture/key', known_hosts='/fixture/known',
                      windows_private_root='C:/Fixture/Private')
        execution = dict(revision='a' * 40, working_tree_dirty=False, workflow_run='123',
                         attempt='1', compiler='msvc', scope='mission', configuration='Final')
        record = dict(private_directory='C:/Fixture/Private/run', manifest=dict(
            execution=execution, protocol=dict(frame_input='fixed-source-period')))
        def result():
            return subprocess.CompletedProcess([], 0, json.dumps(record).encode(), b'')
        with patch('native_cross_m3.subprocess.run', side_effect=lambda *a, **k: result()):
            native_cross_m3.windows_case(config, '123', '1', 'a' * 40, 'fixed-source-period')
            execution['working_tree_dirty'] = True
            with self.assertRaises(ValueError):
                native_cross_m3.windows_case(config, '123', '1', 'a' * 40, 'fixed-source-period')
            execution['working_tree_dirty'] = False
            execution['revision'] = 'b' * 40
            with self.assertRaises(ValueError):
                native_cross_m3.windows_case(config, '123', '1', 'a' * 40, 'fixed-source-period')
            execution['revision'] = 'a' * 40
            for directory in ('C:/Fixture/Private-other/run', 'C:/Fixture/Private/../Outside'):
                record['private_directory'] = directory
                with self.assertRaises(ValueError):
                    native_cross_m3.windows_case(config, '123', '1', 'a' * 40, 'fixed-source-period')

    def test_gpu_host_setup_never_selects_unrelated_processes(self):
        child = subprocess.Popen([sys.executable, '-c', 'import sys; sys.stdin.read()'],
                                 stdin=subprocess.PIPE)
        try:
            if Path('/proc/self/stat').exists():
                self.assertTrue(gpu_host_setup.descendant(child.pid, os.getpid()))
            else:
                self.assertFalse(gpu_host_setup.descendant(child.pid, os.getpid()))
            self.assertFalse(gpu_host_setup.descendant(os.getpid(), child.pid))
            self.assertFalse(gpu_host_setup.descendant(0, child.pid))
        finally:
            child.communicate(timeout=5)

    def test_cross_build_rejects_stale_dirty_and_outside_host_evidence(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'mission-index').mkdir()
            evidence = root / 'run'
            evidence.mkdir()
            inputs = {name: {'bytes': 1, 'sha256': '0' * 64} for name in
                      ('stringpack.db', 'gendata.db', 'gametypes.db', 'maps',
                       'objects', 'mspeech', 'speech', 'textures', 'font')}
            execution = dict(revision='a' * 40, working_tree_dirty=False,
                             workflow_run='123', attempt='1', compiler='gcc', scope='mission')
            record = dict(private_directory=str(evidence), manifest=dict(schema=1,
                          inputs=inputs, execution=execution,
                          protocol={'frame_input': 'measured'}))
            path = root / 'mission-index' / ('123-1-' + 'a' * 40 + '-mission-measured-gcc.json')
            with patch.dict(os.environ, {'GITHUB_RUN_ID': '123', 'GITHUB_RUN_ATTEMPT': '1'}):
                path.write_text(json.dumps(record))
                self.assertEqual(cross_m3.read_case(root, 'a' * 40, 'mission', 'measured', 'gcc')[0], evidence)
                for key, value in [('revision', 'b' * 40), ('working_tree_dirty', True),
                                   ('workflow_run', '124'), ('compiler', 'clang')]:
                    old = execution[key]
                    execution[key] = value
                    path.write_text(json.dumps(record))
                    with self.assertRaises(ValueError):
                        cross_m3.read_case(root, 'a' * 40, 'mission', 'measured', 'gcc')
                    execution[key] = old
                record['private_directory'] = str(root.parent)
                path.write_text(json.dumps(record))
                with self.assertRaises(ValueError):
                    cross_m3.read_case(root, 'a' * 40, 'mission', 'measured', 'gcc')

    def test_private_evidence_survives_runner_temp_cleanup(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            checkout, transient = root / 'checkout', root / 'runner-temp'
            checkout.mkdir()
            transient.mkdir()
            with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': str(root / 'private'),
                                         'RUNNER_TEMP': str(transient)}):
                evidence = private_workspace.create(checkout, 'test-')
                record = evidence / 'generated.txt'
                record.write_text('generated evidence')
                shutil.rmtree(transient)
                self.assertEqual(record.read_text(), 'generated evidence')
                if os.name != 'nt':
                    self.assertEqual(evidence.parent.stat().st_mode & 0o077, 0)

    def test_private_evidence_rejects_disposable_and_protected_roots(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            checkout, transient, retail = (root / name for name in ('checkout', 'runner-temp', 'retail'))
            for directory in (checkout, transient, retail):
                directory.mkdir()
            for directory in (checkout, transient, retail):
                with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': str(directory / 'evidence'),
                                             'RUNNER_TEMP': str(transient),
                                             'FRONTIERX_RETAIL_ROOT': str(retail)}):
                    with self.assertRaises(RuntimeError):
                        private_workspace.create(checkout, 'test-')
            alias = root / 'alias'
            alias.symlink_to(checkout, target_is_directory=True)
            with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': str(alias / 'evidence')}):
                with self.assertRaises(RuntimeError):
                    private_workspace.create(checkout, 'test-')

    def test_private_evidence_requires_explicit_absolute_configuration(self):
        for value in ('', 'relative-evidence'):
            with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': value}):
                with self.assertRaises(RuntimeError):
                    private_workspace.create(Path.cwd(), 'test-')

    def test_report_requires_actual_successful_testcases(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'generated.xml'
            for content, expected in [
                ('<testsuite/>', 1),
                ('<testsuite><testcase><skipped/></testcase></testsuite>', 1),
                ('<testsuite><testcase><failure/></testcase></testsuite>', 1),
                ('<testsuite><testcase><error/></testcase></testsuite>', 1),
                ('not XML', 1),
                ('<testsuite><testcase/></testsuite>', 0),
            ]:
                path.write_text(content)
                with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                    self.assertEqual(check_ctest.main(path), expected)


if __name__ == '__main__':
    unittest.main()
