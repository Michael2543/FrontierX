"""Private candidate provenance and ordinary user launch; never upload runtime inputs."""
import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys

from input_manifest import digest, one_child


EXCLUSIONS = ['movies', 'notification cue', 'space background', 'animated portraits',
              'spatial mission SFX', 'non-base texture mips', 'legacy lighting', 'mission music']
MISSIONS = {'training': 'Sol_Train.dmission', 'terran1': 'T_Mission1.dmission'}
# NativeShell consumes EXTENDED then GAME: RPVERTEX2 UV at location 2,
# MTVERTEX specular at location 2 and UV at location 3.
MISSION_GPU_SHADER_SUFFIXES = ('.vert', '.frag', '_t4.vert', '_mt.vert')
CHECKLIST = ['Original campaign selector, mission selection, briefing and Start; inspect HUD and speech.',
             'Normal selection/movement, construction, combat and objective progression.',
             'F7/F8 early and in active combat/construction; continue the restored state.',
             'Victory/Continue/return and fresh entry; defeat/retry where applicable.',
             'Normal close; report last objective, expected/observed result and reproduction steps privately.']
TERRAN_CHECKLIST = [
    'Original campaign/mission selection, briefing and Start; inspect original text, portraits and briefing media.',
    'Normal orders, production, Seeker radar, Khamir ability, enemy behavior and objective progression.',
    'Both minimaps: system connections, original interactions, selection/target markers and restoration after load.',
    'F7/F8 early and during active orders, production and combat; continue correctly after each load.',
    'Inspect all models/parts/animations, textures/materials/lighting, background and weapon/particle/shield/drive effects throughout the mission and after load.',
    'Verify portrait switch/clear/animation, HUD/text, speech/music/spatial sound; reach original victory and regular defeat. Following mission is P1c.',
    'Normal close; privately report last objective, expected/observed result, visible/audible gaps and reproduction steps.']


def mission_command(root, build, install, font, save, mission="training", bounded_presentation=False, font_mapping=None, single_font_interim=False, no_frame_limit=False):
    if single_font_interim and (mission != 'terran1' or bounded_presentation):
        raise ValueError('single-font interim scope requires Terran1 without bounded media exclusions')
    data = one_child(install, 'data')
    command = [str(build / 'frontierx_native_shell'), '--data-root', str(data),
               '--material-root', str(root / 'Data/mat'), '--system-profile',
               str(root / 'Code/App/Src/Conquest.ini'),
               '--mission-source', MISSIONS[mission], '--mission-engine', '--mission-start', '--mission-campaign',
               ]
    if mission == 'terran1' and not bounded_presentation and not single_font_interim:
        if font_mapping is None:
            raise ValueError('full Terran presentation requires an external strict font map')
        command += ['--ui-font-map', str(font_mapping)]
    else:
        if font is None:
            raise ValueError('bounded/training font face missing')
        command += ['--ui-font', str(font)]
    if mission == 'training' or bounded_presentation:
        command += ['--skip-movies', '--skip-notification-cue', '--skip-space-background',
                    '--skip-animated-portraits', '--skip-mission-sfx', '--base-textures-only',
                    '--skip-legacy-lighting', '--skip-mission-music']
    command += ['--mission-gpu']
    command += [str(build / 'generated/shaders' / ('sdl3_gpu_probe' + name + '.spv'))
                for name in MISSION_GPU_SHADER_SUFFIXES]
    for option, filename in [('--string-pack', 'stringpack.db'), ('--gen-data', 'gendata.db'),
                             ('--game-types', 'gametypes.db')]:
        command += [option, str(one_child(install, filename))]
    if no_frame_limit:
        command.append('--no-frame-limit')
    return command + ['--hotkeys', str(root / 'Code/App/Src/Hotkeys.utf'), '--save-root', str(save)]


def play_environment(env):
    # Ordinary player input/timing, no inherited comparison controls or journals.
    return {key: value for key, value in env.items() if not key.startswith('FRONTIERX_M3_')}



def font_map_fingerprints(path):
    path = Path(path).resolve()
    files = {}
    for line in path.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith('#') or '=' not in line:
            continue
        filename = Path(line.split('=', 1)[1].strip())
        if not filename.is_absolute():
            filename = path.parent / filename
        filename = filename.resolve()
        files[str(filename)] = digest(filename) if filename.is_file() else {'missing': True}
    return {'map': digest(path), 'faces': files}


def write_handoff(root, build, destination, env, mission="training", bounded_presentation=False, single_font_interim=False, no_frame_limit=False):
    if single_font_interim and (mission != 'terran1' or bounded_presentation):
        raise ValueError('single-font interim scope requires Terran1 without bounded media exclusions')
    cache = {}
    for line in (build / 'CMakeCache.txt').read_text().splitlines():
        if line and not line.startswith(('#', '//')) and '=' in line:
            key, value = line.split('=', 1)
            cache[key.split(':', 1)[0]] = value
    configuration = {key: cache.get(key, 'unrecorded') for key in (
        'CMAKE_BUILD_TYPE', 'CMAKE_CXX_COMPILER', 'CMAKE_CXX_FLAGS',
        'FRONTIERX_BUILD_SDL3', 'FRONTIERX_BUILD_SDL3_GPU',
        'FRONTIERX_BUILD_NATIVE_FONTS', 'FRONTIERX_BUILD_FFMPEG_AUDIO',
        'FRONTIERX_BUILD_FFMPEG_VIDEO')}
    reduced = mission == 'training' or bounded_presentation
    record = {'mission': mission, 'bounded_presentation': reduced, 'single_font_interim': single_font_interim, 'no_frame_limit': no_frame_limit, 'revision': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root,
                                                  text=True).strip(),
              'working_tree_dirty': bool(subprocess.check_output(['git', 'status', '--porcelain'], cwd=root)),
              'workflow_run': env.get('GITHUB_RUN_ID', 'local'),
              'attempt': env.get('GITHUB_RUN_ATTEMPT', 'local'),
              'compiler': env.get('FRONTIERX_M3_COMPILER', 'local'),
              'toolchain': env.get('FRONTIERX_M3_TOOLCHAIN', 'see CMakeCache.txt'),
              'configuration': configuration,
              'build_location': str(build), 'binary': digest(build / 'frontierx_native_shell'),
              'cmake_cache': digest(build / 'CMakeCache.txt'),
              'external_dependencies': {'decoder_prefix': env.get('FRONTIERX_LGPL_PREFIX', cache.get('CMAKE_PREFIX_PATH')),
                                        'video': 'Full Terran requires reviewed shared-LGPL FFmpeg MPEG1/MP2 decoders, MPEG video/audio parsers and shared swscale',
                                        'font': env.get('FRONTIERX_TEST_FONT_FILE', cache.get('FRONTIERX_TEST_FONT_FILE')),
                                        'font_mapping': env.get('FRONTIERX_PLAYTEST_FONT_MAP',
                                            'Required externally for full Terran; family/weight validation at launch'),
                                        'retail_installation': 'FRONTIERX_RETAIL_ROOT supplied privately at launch',
                                        'tools': 'CMake/Ninja, GCC or Clang, pkg-config SDL3, glslc, Python3, llvm-rc; FreeType >=2.14'},
              'launch_cwd': str(root),
              'game_runtime_cwd': str(Path(env['FRONTIERX_PLAYTEST_SAVE_ROOT']).resolve() / 'runtime')
                  if env.get('FRONTIERX_PLAYTEST_SAVE_ROOT') else '${FRONTIERX_PLAYTEST_SAVE_ROOT}/runtime',
              'launch_command': shlex.join([sys.executable, str(root / 'Tools/ci/playtest.py'),
                                            '--build', str(build), '--mission', mission, '--candidate',
                                            str(destination.resolve()), '--play'] +
                                           (['--bounded-presentation'] if bounded_presentation else []) +
                                           (['--single-font-interim'] if single_font_interim else []) +
                                           (['--no-frame-limit'] if no_frame_limit else [])),
              'launch_environment': ['FRONTIERX_RETAIL_ROOT', 'FRONTIERX_TEST_FONT_FILE',
                                     'FRONTIERX_PLAYTEST_FONT_MAP (required for full Terran)',
                                     'FRONTIERX_PLAYTEST_SAVE_ROOT', 'XDG_CONFIG_HOME (external writable directory)'],
              'known_limits': (EXCLUSIONS if reduced else []) + [
                                           'P1b gameplay and complete presentation/media acceptance pending; unsupported owners fail explicitly',
                                           'Wayland resize and whole-process GPU/library leak checks remain open'],
              'checklist': TERRAN_CHECKLIST if mission == 'terran1' else CHECKLIST,
              'acceptance': 'Provenance only; report technical/host execution separately. P1b full user gameplay and presentation acceptance pending.'}
    mapping = env.get('FRONTIERX_PLAYTEST_FONT_MAP')
    record['font_mapping_fingerprints'] = font_map_fingerprints(mapping) if mapping and not single_font_interim else None
    font = env.get('FRONTIERX_TEST_FONT_FILE', cache.get('FRONTIERX_TEST_FONT_FILE'))
    record['single_font_fingerprint'] = digest(Path(font)) if single_font_interim and font and Path(font).is_file() else None
    if single_font_interim:
        record['known_limits'].insert(0, 'Single font interim: original Arial/Arial Narrow family and weight mapping missing; typography and full P1b acceptance remain open. Launch requests all graphics/media without exclusion flags.')
    record['font_mapping_readiness'] = ('Supplied; source family/weight validation and glyph presentation remain unproven'
        if mapping else 'External strict map missing; full Terran launch blocked until supplied')
    if single_font_interim:
        record['font_mapping_readiness'] = 'Explicit single-font interim; original family/weight mapping remains a P1b blocker'
    if no_frame_limit:
        record['known_limits'].append('Original No Frame limiting enabled: original elapsed-time normalization/game-speed policy changes apply; simulation constants unchanged, 60 FPS not guaranteed.')
    if env.get('GITHUB_RUN_ID') and record['working_tree_dirty']:
        raise ValueError('CI candidate source tree is dirty')
    record['shaders'] = {path.name: digest(path)
                         for path in sorted((build / 'generated/shaders').glob('sdl3_gpu_probe*.spv'))}
    destination.write_text(json.dumps(record, indent=2) + '\n')
    destination.chmod(0o600)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--mission', choices=MISSIONS, default='training')
    parser.add_argument('--candidate', type=Path,
                        help='Private provenance record to validate before player launch')
    parser.add_argument('--no-frame-limit', action='store_true',
                        help='Use the original No Frame limiting option; retains its original timing/game-speed semantics')
    presentation = parser.add_mutually_exclusive_group()
    presentation.add_argument('--bounded-presentation', action='store_true',
                        help='Explicit interim exclusions; never a complete P1b presentation candidate')
    presentation.add_argument('--single-font-interim', action='store_true',
                        help='Terran1 with all graphics/media enabled and one external font; original typography acceptance remains open')
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument('--handoff', type=Path)
    action.add_argument('--play', action='store_true')
    args = parser.parse_args()
    if args.single_font_interim and args.mission != 'terran1':
        parser.error('--single-font-interim requires --mission terran1')
    root = Path(__file__).resolve().parents[2]
    build = args.build.resolve()
    if args.handoff:
        write_handoff(root, build, args.handoff, os.environ, args.mission, args.bounded_presentation, args.single_font_interim, args.no_frame_limit)
        return
    candidate = args.candidate or build.parent / ('candidate-terran1.json' if args.mission == 'terran1'
                                else 'candidate.json')
    if args.candidate and not candidate.is_file():
        raise ValueError('explicit candidate provenance record missing')
    if candidate.is_file():
        record = json.loads(candidate.read_text())
        if record.get('mission', 'training') != args.mission:
            raise ValueError('candidate mission differs from launch')
        if ('bounded_presentation' in record and record['bounded_presentation'] !=
                (args.mission == 'training' or args.bounded_presentation)):
            raise ValueError('candidate presentation scope differs from launch')
        if record.get('single_font_interim', False) != args.single_font_interim:
            raise ValueError('candidate typography scope differs from launch')
        if record.get('no_frame_limit', False) != args.no_frame_limit:
            raise ValueError('candidate frame-limiting option differs from launch')
        if record['binary'] != digest(build / 'frontierx_native_shell'):
            raise ValueError('candidate binary changed after handoff')
        if record.get('cmake_cache') != digest(build / 'CMakeCache.txt'):
            raise ValueError('candidate configuration cache changed after handoff')
        for name, expected in record.get('shaders', {}).items():
            if digest(build / 'generated/shaders' / name) != expected:
                raise ValueError('candidate shader changed after handoff')
    env = play_environment(os.environ)
    install = Path(env['FRONTIERX_RETAIL_ROOT']).resolve()
    font = Path(env['FRONTIERX_TEST_FONT_FILE']).resolve() if env.get('FRONTIERX_TEST_FONT_FILE') else None
    font_mapping = Path(env['FRONTIERX_PLAYTEST_FONT_MAP']).resolve() if env.get('FRONTIERX_PLAYTEST_FONT_MAP') else None
    if args.mission == 'terran1' and not args.bounded_presentation:
        if args.single_font_interim:
            if font is None or not font.is_file() or font.is_relative_to(root):
                raise ValueError('single-font interim requires a readable external font outside checkout')
            if candidate.is_file() and record.get('single_font_fingerprint') != digest(font):
                raise ValueError('candidate interim font changed after handoff')
        elif font_mapping is None or not font_mapping.is_file() or font_mapping.is_relative_to(root):
            raise ValueError('full Terran requires a readable external strict font map outside checkout')
        if 'FRONTIERX_BUILD_FFMPEG_VIDEO:BOOL=ON' not in (build / 'CMakeCache.txt').read_text().splitlines():
            raise ValueError('full Terran requires the optional native movie build; use an explicit bounded candidate for exclusions')
    if candidate.is_file() and record.get('font_mapping_fingerprints') is not None:
        if font_mapping is None or font_map_fingerprints(font_mapping) != record['font_mapping_fingerprints']:
            raise ValueError('candidate mapped font inputs changed after handoff')
    save = Path(env['FRONTIERX_PLAYTEST_SAVE_ROOT']).resolve()
    config = Path(env['XDG_CONFIG_HOME']).resolve()
    runtime = (save / 'runtime').resolve()
    for path in (save, config, runtime):
        if path.is_relative_to(root) or path.is_relative_to(install):
            raise ValueError('player runtime outputs must be outside checkout and retail installation')
        path.mkdir(mode=0o700, parents=True, exist_ok=True)
    # Interactive output remains on the user's host, never a CI artifact.
    subprocess.run(mission_command(root, build, install, font, save, args.mission,
                                   args.bounded_presentation, font_mapping, args.single_font_interim, args.no_frame_limit), cwd=runtime, env=env, check=True)


if __name__ == '__main__':
    try:
        main()
    except (KeyError, OSError, ValueError, subprocess.SubprocessError):
        print('FAIL: private playtest handoff/launch; verify external inputs and build', file=sys.stderr)
        sys.exit(1)
