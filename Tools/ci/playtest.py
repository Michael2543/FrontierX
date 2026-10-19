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
    'From a genuine pre-end save, reach regular victory, inspect original result/statistics and Continue; enter the second Terran briefing and mission with correct initial units, objectives and normal orders.',
    'Reach regular defeat; inspect original result/return, retry and mission selection. Defeat, resign and cancelled briefings must not unlock victory progress.',
    'Quit the whole program and restart with the same external save/configuration roots; verify original campaign progress and available missions.',
    'Save/load before the first mission ends and after the second mission starts; continue with the correct objectives, orders, selection, both minimaps, animated portraits and full graphics/audio/media.',
    'Repeat start/load/close/retry/mission-switch cycles in one process; record the cycle count and check for hangs, stale mission content, duplicate speech/music or surviving effects.',
    'Normal close; privately report genuine checkpoint, last objective, cycle count, expected/observed result, visible/audible gaps and reproduction steps.']


P2_CHECKLIST = [
    'Choose the target mission through original campaign selection; inspect briefing, Start and initial units/objectives. No unlock or objective manipulation.',
    'Play every phase through regular victory and exercise the mission-specific mechanics, reinforcements, enemy waves, abilities and cutscenes.',
    'F7/F8 early and late, during production/research/abilities/combat; confirm objectives, orders, units and source scripts resume correctly.',
    'Inspect all required models/parts/animations, materials/textures/lighting, background, effects, HUD, both minimaps including system links, portraits, speech/music/spatial SFX and movies, including later spawned content and after loading.',
    'Check regular defeat/retry, result/statistics, Continue to the next original briefing and mission, or the original campaign ending after the last mission.',
    'Restart the process with the same external save/configuration roots; verify genuine progress and repeated load/retry/switch cleanup.',
    'Report the full mission result and genuine checkpoints privately. Missing presentation or pending feedback blocks acceptance.']
P2_TM2_CHECKLIST = [
    'TM2: verify platform production, missile direction, repair/sell and the original six-Corvette/three-Cruiser build gate; exercise Troopship research, pods and capture with save/load.',
    'TM2: check Scout Carrier/fighters, Zorap and enemy production queues, both Mantis Eyestocks and Blackwell defeat; regular victory may follow capture of a Thripid, Cocoon, BlastFurnace or Eyestock.',
    'TM2: preserve the previously reported complete mission run and save/load evidence. Use genuine late/early checkpoints for missing scenarios and normal TM3 transition; request another full run only for an identified regression.']

P2_TM4_CHECKLIST = [
    'TM4: locate the Mantis rebels, stop red-Mantis attacks and build the Perilon base/defenses; check rebel harvesters, their Jump Gate and Bekka/Luxor waves, including later carriers/fighters and Scarabs.',
    'TM4: save/load while protecting the rebels, during their return home and before investigating Alto; verify the original speech/objective sequence, system access and enemy reinforcements continue.',
    'TM4: retreat with both Blackwell and Hawkes to Capella for regular victory; exercise original admiral/rebel/Jump-Gate defeat conditions and normal result/progress/Continue to TM5.']

P2_TM5_CHECKLIST = [
    'TM5: inspect Battleship turret slew and projectile travel toward ship/building targets across several directions and after reload; the jointed muzzle must stay on the visible hull.',
    'TM5: reclaim Perilon, restore Light/Heavy Shipyards and inspect generated Scout/Hive Carrier and Frigate/Scarab waves; save/load during construction and active generator countdowns.',
    'TM5: follow the original BlackHole/distress sequence, Blackwell/Hawkes events, Ker Tak encounter and authored flagship abilities; check generated units, fighters, effects and media after loading.',
    'TM5: complete the Luxor/Bekka objectives and regular victory; verify original admiral/Ker Tak/HQ defeat conditions, result/progress/retry and the original next briefing. Use genuine checkpoints for targeted repeats.',
    'Menu: select TM5 through genuine campaign progress, cancel its briefing and select it again; check return from the game, normal save/load and process restart. Cancel/defeat must not invent a victory unlock.']


P2_TM7_CHECKLIST = [
    'TM7: inspect original Supply Lines briefing, Benson/Ker Tak, ten-system map and initial rebel alliances; cancel/re-enter briefing and use normal Start.',
    'TM7: build Propulsion and Advanced Weapons labs, extend the base/supply route through the systems toward Lemo, and save/load during research, construction and Mantis counterattacks.',
    'TM7: build the original HQ in Lemo and remove Mantis platforms for victory; check Benson/Ker Tak defeat, retry, results/progress and original Continue to TM8.']

P2_TM8_CHECKLIST = [
    'TM8: inspect original Lost Sheep briefing, fourteen-system map, Ker Tak portraits/speech and initial objectives; save/load before and after entering Aeon/Kalidon.',
    'TM8: locate Benson, check the original rescue/death timer and Mantis wave sequence, including Kracus/Sendlor/Varn/Talos and later repeat waves; save/load during active attacks.',
    'TM8: inspect later Benson Battleship, Blackwell chrome ship and Solarian Monolith/Taos/Trireme reinforcements with full weapons/materials/effects/audio; clear the original systems/Cocoons and preserve required admirals.',
    'TM8: verify normal victory/defeat/retry/progress and Continue to the original TM9 briefing. TM9 is linked as the next-entry dependency; its full gameplay is not accepted.']


P2_TM10_CHECKLIST = [
    'Replay the unlocked Celereon film (cq_2.mpg) from the original selector; check video, sound, EOF/return and Esc.',
    'TM10: original summit briefing and entry; locate Vivac in Procyon, continue to Natus and keep the required admirals alive.',
    'TM10: inspect Aurora cloak/advanced units and F7/F8 during combat; genuine victory must unlock TM11.']
P2_TM11_CHECKLIST = [
    'TM11: original opening scene, disabled platforms, capture/repair, Blackwell events and later attacks; F7/F8 during these states.',
    'After genuine TM11 victory, inspect cq_3.mpg through audio/video EOF and return, then Continue to TM12; replay remains available.']
P2_TM12_CHECKLIST = [
    'TM12: original briefing, regeneration/capture, Xin/Rendar and Quasili attacks, Smirnoff fleet and required objectives/admirals.',
    'F7/F8 during regeneration, active attack waves and fleet orders; check genuine victory/defeat and result/return.',
    'TM13 is not linked by this candidate; stop after the TM12 result rather than treating the next mission as integrated.']


def campaign_checklist(mission, campaign_mission):
    if campaign_mission is not None:
        if mission != 'terran1' or campaign_mission not in range(1, 17):
            raise ValueError('campaign target requires Terran selector and mission 1..16')
        return P2_CHECKLIST + {2: P2_TM2_CHECKLIST, 4: P2_TM4_CHECKLIST, 5: P2_TM5_CHECKLIST, 7: P2_TM7_CHECKLIST, 8: P2_TM8_CHECKLIST, 10: P2_TM10_CHECKLIST, 11: P2_TM11_CHECKLIST, 12: P2_TM12_CHECKLIST}.get(campaign_mission, [])
    return TERRAN_CHECKLIST if mission == 'terran1' else CHECKLIST


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


def write_handoff(root, build, destination, env, mission="training", bounded_presentation=False, single_font_interim=False, no_frame_limit=False, campaign_mission=None):
    checklist = campaign_checklist(mission, campaign_mission)
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
    record = {'mission': mission, 'campaign_mission': campaign_mission, 'bounded_presentation': reduced, 'single_font_interim': single_font_interim, 'no_frame_limit': no_frame_limit, 'revision': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root,
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
                                           (['--no-frame-limit'] if no_frame_limit else []) +
                                           (['--campaign-mission', str(campaign_mission)] if campaign_mission is not None else [])),
              'launch_environment': ['FRONTIERX_RETAIL_ROOT', 'FRONTIERX_TEST_FONT_FILE',
                                     'FRONTIERX_PLAYTEST_FONT_MAP (required for full Terran)',
                                     'FRONTIERX_PLAYTEST_SAVE_ROOT', 'XDG_CONFIG_HOME (external writable directory)'],
              'known_limits': (EXCLUSIONS if reduced else []) + [
                                           'P1c is user-accepted in its documented scope. Each P2 mission requires separate full gameplay/media acceptance; unsupported owners fail explicitly',
                                           'Wayland resize and whole-process GPU/library leak checks remain open'],
              'checklist': checklist,
              'acceptance': 'Provenance only; report technical/host execution separately. P1b/P1c user acceptance remains valid in its documented scope; this record accepts no additional P2 mission and resolves no pending technical check.'}
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
    parser.add_argument('--campaign-mission', type=int, choices=range(1, 17),
                        help='P2 playtest target only; choose it through the original selector, no unlock or direct entry')
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
    if args.campaign_mission is not None and args.mission != 'terran1':
        parser.error('--campaign-mission requires --mission terran1')
    if args.single_font_interim and args.mission != 'terran1':
        parser.error('--single-font-interim requires --mission terran1')
    root = Path(__file__).resolve().parents[2]
    build = args.build.resolve()
    if args.handoff:
        write_handoff(root, build, args.handoff, os.environ, args.mission, args.bounded_presentation, args.single_font_interim, args.no_frame_limit, args.campaign_mission)
        return
    candidate = args.candidate or build.parent / ('candidate-terran1.json' if args.mission == 'terran1'
                                else 'candidate.json')
    if args.candidate and not candidate.is_file():
        raise ValueError('explicit candidate provenance record missing')
    if candidate.is_file():
        record = json.loads(candidate.read_text())
        if record.get('campaign_mission') != args.campaign_mission:
            raise ValueError('candidate campaign target differs from launch')
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
