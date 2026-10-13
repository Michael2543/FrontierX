"""Run original Linux Sol_Train repeats and exact-tick save cases on a trusted host.

Every failure remains fatal. Current partial capture cannot pass acceptance.
No trace, save, command line, private path or raw process output is published.
"""
import argparse
import os
from pathlib import Path
import subprocess
import sys
import json

from input_manifest import manifest, digest
from private_workspace import create


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--frame-input', choices=['measured', 'fixed-source-period'], default='measured')
    parser.add_argument('--case', choices=['repeat1', 'repeat2', '256-save', '256-reload',
                                         '512-save', '512-reload'])
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    install = os.environ.get('FRONTIERX_RETAIL_ROOT')
    font = os.environ.get('FRONTIERX_TEST_FONT_FILE')
    if not install or not font:
        raise RuntimeError('lawful external inputs missing')
    install = Path(install)
    data = next((p for p in install.iterdir() if p.name.lower() == 'data'), None)
    if data is None:
        raise RuntimeError('external data root missing')
    private = create(root, 'frontierx-m3-mission-')
    if private.resolve().is_relative_to(root):
        raise RuntimeError('private mission output must be outside source checkout')
    inputs = manifest(install, font)
    fixed = args.frame_input == 'fixed-source-period'
    if args.case and '-' in args.case and not fixed:
        raise RuntimeError('exact-tick checkpoint requires fixed frame input')
    inputs['protocol'] = {'version': 2, 'movement_owner': 'CQE_MOVE_SELECTED_UNITS/ObjComm',
                          'frame_input': args.frame_input,
                          'control_time_units': '1/1024 millisecond, truncated',
                          'commands_before_ticks': [32, 64], 'last_tick': 1024,
                          'window_input': 'active, centered pointer; host input excluded; close fatal',
                          'speech_completion': 'live original owner',
                          'coverage': 'partial'}
    inputs['execution'] = {
        'revision': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root, text=True).strip(),
        'working_tree_dirty': bool(subprocess.check_output(['git', 'status', '--porcelain'], cwd=root)),
        'workflow_run': os.environ.get('GITHUB_RUN_ID', 'local-supplement'),
        'job': os.environ.get('GITHUB_JOB', 'local-supplement'),
        'attempt': os.environ.get('GITHUB_RUN_ATTEMPT', 'local-supplement'),
        'compiler': os.environ.get('FRONTIERX_M3_COMPILER', 'local-supplement'),
        'scope': os.environ.get('FRONTIERX_M3_SCOPE', 'local-supplement'),
        'toolchain': os.environ.get('FRONTIERX_M3_TOOLCHAIN', 'local-supplement'),
        'timeout_seconds': 600 if fixed else 120}
    (private / 'input-manifest.json').write_text(json.dumps(inputs, sort_keys=True))
    command = [str(args.build / 'frontierx_native_shell'), '--data-root', str(data),
               '--material-root', str(root / 'Data/mat'), '--system-profile',
               str(root / 'Code/App/Src/Conquest.ini'), '--ui-font', font,
               '--skip-movies', '--skip-notification-cue', '--skip-space-background',
               '--skip-animated-portraits', '--skip-mission-sfx', '--base-textures-only',
               '--skip-legacy-lighting', '--skip-mission-music', '--mission-source',
               'Sol_Train.dmission', '--mission-engine', '--mission-start', '--mission-gpu']
    command += [str(args.build / 'generated/shaders' / ('sdl3_gpu_probe' + name + '.spv'))
                for name in ('.vert', '.frag', '_mt.vert', '_t4.vert')]
    for option, filename in [('--string-pack', 'stringpack.db'), ('--gen-data', 'gendata.db'),
                             ('--game-types', 'gametypes.db')]:
        command += [option, str(install / filename)]
    command += ['--hotkeys', str(root / 'Code/App/Src/Hotkeys.utf')]
    cases = ['repeat1', 'repeat2']
    if fixed:
        cases += ['256-save', '256-reload', '512-save', '512-reload']
    if args.case:
        cases = [args.case]
    codes = []
    for case in cases:
        run = private / case
        run.mkdir()
        env = os.environ.copy()
        env.update(FRONTIERX_M3_TRACE=str(run / 'ticks.jsonl'),
                   FRONTIERX_M3_JOURNAL=str(run / 'speech'),
                   FRONTIERX_M3_CONTROLLED_INPUT='1',
                   XDG_CONFIG_HOME=str(run / 'config'))
        if fixed:
            env['FRONTIERX_M3_FIXED_FRAME'] = '1'
        else:
            env.pop('FRONTIERX_M3_FIXED_FRAME', None)
        env.pop('FRONTIERX_M3_CHECKPOINT', None)
        env.pop('FRONTIERX_M3_CHECKPOINT_PREFIX', None)
        if '-' in case:
            env.update(FRONTIERX_M3_CHECKPOINT=case,
                       FRONTIERX_M3_CHECKPOINT_PREFIX=str(run / 'checkpoint'))
        save = run / 'save'
        save.mkdir()
        with (run / 'process-private.log').open('wb') as log:
            try:
                result = subprocess.run(command + ['--save-root', str(save)], cwd=root,
                                        env=env, stdout=log, stderr=subprocess.STDOUT,
                                        timeout=inputs['execution']['timeout_seconds'])
                code = result.returncode
            except subprocess.TimeoutExpired:
                code = 'timeout'
        codes.append(code)
        record = {'case': case, 'process_exit': code, 'coverage': 'partial'}
        saved = save / 'm3-checkpoint.mission'
        if saved.is_file():
            record['save_bytes'] = digest(saved)
        (run / 'result.json').write_text(json.dumps(record, sort_keys=True))
    comparisons = []
    compare = [sys.executable, str(root / 'Tools/portability/m3_compare.py')]
    if 'repeat1' in cases:
        for case in cases:
            if case != 'repeat1':
                comparisons.append(subprocess.run(compare + ['ticks',
                    str(private / 'repeat1/ticks.jsonl'), str(private / case / 'ticks.jsonl')]).returncode)
    for case in cases:
        if '-' not in case:
            continue
        run = private / case
        for stage in ('after-save', 'after-reload') if case.endswith('reload') else ('after-save',):
            comparisons.append(subprocess.run(compare + ['checkpoint',
                str(run / 'checkpoint-before-save.json'), str(run / ('checkpoint-' + stage + '.json')),
                '--observed-only']).returncode)
    # Byte hashes characterize actual saves privately. No blanket byte-equality
    # requirement or normalization of unknown metadata is inferred from them.
    summary = {'frame_input': args.frame_input, 'process_results': codes,
               'comparison_results': comparisons, 'coverage': 'partial',
               'byte_equality_contract': 'not established'}
    byte_comparisons = {}
    for point in (256, 512):
        files = [private / f'{point}-{action}' / 'save/m3-checkpoint.mission'
                 for action in ('save', 'reload')]
        if all(path.is_file() for path in files):
            equal = True
            with files[0].open('rb') as first, files[1].open('rb') as second:
                while True:
                    left, right = first.read(1024 * 1024), second.read(1024 * 1024)
                    if left != right:
                        equal = False
                        break
                    if not left:
                        break
            byte_comparisons[str(point)] = {'observed_equal': equal, 'required': False}
    summary['save_byte_comparisons'] = byte_comparisons
    (private / 'result.json').write_text(json.dumps(summary, sort_keys=True))
    execution = inputs['execution']
    if execution['workflow_run'] != 'local-supplement':
        import re
        tokens = [execution['workflow_run'], execution['attempt'], execution['revision'],
                  execution['scope'], args.frame_input, execution['compiler']]
        if (execution['working_tree_dirty'] or
                not all(re.fullmatch(r'[a-z0-9-]+', t) for t in tokens)):
            raise RuntimeError('CI mission provenance invalid')
        index = private.parent / 'mission-index'
        index.mkdir(mode=0o700, exist_ok=True)
        if not index.resolve().is_relative_to(private.parent.resolve()):
            raise RuntimeError('private mission index outside evidence root')
        if os.name != 'nt':
            index.chmod(0o700)
        record = index / ('-'.join(tokens) + '.json')
        record.write_text(json.dumps({'private_directory': str(private), 'manifest': inputs}, sort_keys=True))
        if os.name != 'nt':
            record.chmod(0o600)
    if any(codes) or any(comparisons):
        raise RuntimeError('mission comparison failed or incomplete')
    # Even a narrow --case without a cross-run comparison cannot pass M3.
    raise RuntimeError('partial observation cannot satisfy M3 acceptance')


if __name__ == '__main__':
    try:
        main()
    except (RuntimeError, OSError, ValueError, subprocess.SubprocessError):
        print('FAIL: original mission comparison; private diagnostics retained', file=sys.stderr)
        sys.exit(1)
