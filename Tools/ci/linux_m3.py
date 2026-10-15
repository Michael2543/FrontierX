"""Trusted Linux CI driver. Build/test logs stay private on the runner."""
import argparse
import os
from pathlib import Path
import subprocess
import sys
from private_workspace import create
import gpu_host_setup


def run(command, root, log, env):
    with log.open('ab') as stream:
        result = subprocess.run(command, cwd=root, env=env, stdout=stream,
                                stderr=subprocess.STDOUT)
    if result.returncode:
        raise RuntimeError('required command failed; private runner log retained')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--compiler', choices=['gcc', 'clang'], required=True)
    parser.add_argument('--scope', choices=['default', 'optional', 'sanitizers', 'gpu', 'mission', 'mission-sanitizers'], required=True)
    parser.add_argument('--frame-input', choices=['measured', 'fixed-source-period'], default='measured')
    parser.add_argument('--track', choices=['technical', 'm3'], default='m3')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    private = create(root, 'frontierx-m3-')
    build, log = private / 'build', private / 'execution-private.log'
    env = os.environ.copy()
    env['ASAN_OPTIONS'] = 'detect_leaks=1:halt_on_error=1'
    env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    revision = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root, text=True).strip()
    compiler = 'g++' if args.compiler == 'gcc' else 'clang++'
    version = subprocess.check_output([compiler, '--version'], text=True).splitlines()[0]
    env.update(FRONTIERX_M3_COMPILER=args.compiler, FRONTIERX_M3_SCOPE=args.scope,
               FRONTIERX_M3_TOOLCHAIN=version)
    print(f'Revision {revision}; {version}; input scope synthetic (mission uses external inputs)', flush=True)
    command = ['cmake', '-S', str(root), '-B', str(build), '-G', 'Ninja',
               '-DCMAKE_BUILD_TYPE=Debug', '-DBUILD_TESTING=ON',
               '-DCMAKE_CXX_COMPILER=' + ('g++' if args.compiler == 'gcc' else 'clang++'),
               '-DFRONTIERX_BUILD_SDL3=ON', '-DFRONTIERX_BUILD_SDL3_GPU=ON']
    if args.scope in ('optional', 'sanitizers', 'gpu', 'mission', 'mission-sanitizers'):
        prefix = env.get('FRONTIERX_LGPL_PREFIX')
        if not prefix:
            raise RuntimeError('external reviewed shared LGPL decoder prefix missing')
        font = env.get('FRONTIERX_TEST_FONT_FILE')
        if not font or not Path(font).is_file():
            raise RuntimeError('external font for optional lifecycle tests missing')
        command += ['-DFRONTIERX_TEST_FONT_FILE=' + font,
                    '-DFRONTIERX_BUILD_NATIVE_FONTS=ON', '-DFRONTIERX_BUILD_FFMPEG_AUDIO=ON',
                    '-DCMAKE_PREFIX_PATH=' + prefix]
        if args.track == 'technical':
            command += ['-DFRONTIERX_BUILD_FFMPEG_VIDEO=ON']
    if args.scope in ('sanitizers', 'mission-sanitizers'):
        command += ['-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer',
                    '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined',
                    '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined']
    run(command, root, log, env)
    run(['cmake', '--build', str(build), '--parallel', '4'], root, log, env)
    junit = private / 'ctest.xml'
    command = ['ctest', '--test-dir', str(build), '--output-on-failure',
               '--no-tests=error', '--output-junit', str(junit)]
    if args.track == 'technical' and args.scope == 'sanitizers':
        # Integrated save/layout/input/render owners; no host GPU in CPU sanitizers.
        command += ['-R', '^portability\\.(layout|app_schema|sol_t_schema|script01_schema|data_root|native_shell|native_window_manager|native_heap|event_system|hotkey_event|vertex_buffer_manager|render_submission|render_batcher_runtime|render_pipeline|state_material|game_material_manager.*|view_parser)$']
    # Technical CPU runners need no display; the existing cursor owner joins
    # GPU tests on the desktop host. M3 retains its historical scope selection.
    host_label = 'host' if args.track == 'technical' else 'gpu'
    command += ['-L', host_label] if args.scope == 'gpu' else ['-LE', host_label]
    if args.scope == 'gpu' and env.get('FRONTIERX_M3_GPU_SETUP'):
        print('Explicit GPU host setup: ' + env['FRONTIERX_M3_GPU_SETUP'], flush=True)
        gpu_host_setup.run(command, root, log, env, private)
    else:
        run(command, root, log, env)
    run([sys.executable, str(root / 'Tools/ci/check_ctest.py'), str(junit)], root, log, env)
    if args.track == 'm3':
        # Only synthetic CRT/RandomGen draws may be uploaded. No retail state/logs.
        output = root / 'out' / 'm3-synthetic' / (args.compiler + '.json')
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open('wb') as stream:
            subprocess.run([str(build / 'frontierx_rng_contract_probe'), '--trace'],
                           cwd=private, env=env, stdout=stream, check=True)
        with (output.parent / (args.compiler + '-randomgen.json')).open('wb') as stream:
            subprocess.run([str(build / 'frontierx_rng_contract_probe'), '--randomgen-trace'],
                           cwd=private, env=env, stdout=stream, check=True)
    if args.scope in ('mission', 'mission-sanitizers'):
        mission = [sys.executable, str(root / 'Tools/ci/mission_m3.py'),
                   '--build', str(build), '--frame-input', args.frame_input]
        if args.track == 'technical':
            mission += ['--technical-smoke']
        run(mission, root, log, env)
        if args.track == 'technical':
            run(mission + ['--mission', 'terran1'], root, log, env)
    if args.track == 'technical' and args.scope == 'optional':
        run([sys.executable, str(root / 'Tools/ci/playtest.py'), '--build', str(build),
             '--handoff', str(private / 'candidate.json')], root, log, env)
        run([sys.executable, str(root / 'Tools/ci/playtest.py'), '--build', str(build),
             '--mission', 'terran1', '--handoff', str(private / 'candidate-terran1.json')],
            root, log, env)
        print('Candidates retained privately; candidate.json training and candidate-terran1.json (P1b acceptance pending; no upload)')
    print(f'Linux {args.compiler} {args.scope}: required scope executed without skips')


if __name__ == '__main__':
    try:
        main()
    except (RuntimeError, OSError, ValueError, subprocess.SubprocessError):
        print('FAIL: Linux CI scope; inspect private runner diagnostics', file=sys.stderr)
        sys.exit(1)
