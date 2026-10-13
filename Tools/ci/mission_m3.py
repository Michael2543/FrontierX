"""Run the original Linux Sol_Train observation twice on a trusted GPU host.

Every failure remains fatal. Current partial capture cannot pass acceptance.
No trace, save, command line, private path or raw process output is published.
"""
import argparse
import os
from pathlib import Path
import subprocess
import sys
import json

from input_manifest import manifest
from private_workspace import create


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, required=True)
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
    (private / 'input-manifest.json').write_text(json.dumps(manifest(install, font), sort_keys=True))
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
    codes = []
    for repeat in (1, 2):
        run = private / str(repeat)
        run.mkdir()
        env = os.environ.copy()
        env.update(FRONTIERX_M3_TRACE=str(run / 'ticks.jsonl'),
                   FRONTIERX_M3_JOURNAL=str(run / 'speech'),
                   XDG_CONFIG_HOME=str(run / 'config'))
        save = run / 'save'
        save.mkdir()
        with (run / 'process-private.log').open('wb') as log:
            result = subprocess.run(command + ['--save-root', str(save)], cwd=root,
                                    env=env, stdout=log, stderr=subprocess.STDOUT, timeout=120)
        codes.append(result.returncode)
    comparison = subprocess.run([sys.executable, str(root / 'Tools/portability/m3_compare.py'),
                                 'ticks', str(private / '1/ticks.jsonl'),
                                 str(private / '2/ticks.jsonl')])
    if any(codes) or comparison.returncode:
        raise RuntimeError('mission comparison failed or incomplete')


if __name__ == '__main__':
    try:
        main()
    except (RuntimeError, OSError, ValueError, subprocess.SubprocessError):
        print('FAIL: original mission comparison; private diagnostics retained', file=sys.stderr)
        sys.exit(1)
