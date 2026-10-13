"""Compare private same-revision CI observations with the native Windows host.

Only reviewed default-branch jobs may access the retained connection config.
SSH output and copied semantic traces stay outside checkout/logs/artifacts.
"""
import json
import os
from pathlib import Path, PureWindowsPath
import re
import subprocess
import sys
import base64

from private_workspace import create
from cross_m3 import read_case

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'portability'))
import m3_compare


def windows_case(config, run, attempt, revision, mode):
    parent = PureWindowsPath(config['windows_private_root'])
    if not parent.is_absolute() or '..' in parent.parts:
        raise ValueError('invalid Windows private evidence root')
    name = f'{run}-{attempt}-{revision}-mission-{mode}-msvc.json'
    literal = str(parent / 'mission-index' / name).replace("'", "''")
    script = "$ErrorActionPreference='Stop';Get-Content -LiteralPath '" + literal + "' -Raw"
    command = ['ssh', '-i', config['key'], '-o', 'IdentitiesOnly=yes', '-o', 'BatchMode=yes',
               '-o', 'StrictHostKeyChecking=yes', '-o', 'UserKnownHostsFile=' + config['known_hosts'],
               '-o', 'ConnectTimeout=10', config['target'], 'powershell', '-NoProfile',
               '-EncodedCommand', base64.b64encode(script.encode('utf-16le')).decode('ascii')]
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=30)
    if result.returncode:
        raise ValueError('required native CI index unavailable')
    record = json.loads(result.stdout.decode('utf-8-sig'))
    directory = PureWindowsPath(record['private_directory'])
    if not directory.is_relative_to(parent) or '..' in directory.parts:
        raise ValueError('native evidence outside private root')
    manifest = record['manifest']
    execution = manifest['execution']
    if (execution['revision'] != revision or execution['working_tree_dirty'] or
            execution['workflow_run'] != run or execution['attempt'] != attempt or
            execution['compiler'] != 'msvc' or execution['scope'] != 'mission' or
            execution['configuration'] != 'Final' or manifest['protocol']['frame_input'] != mode):
        raise ValueError('native CI provenance mismatch')
    return directory, manifest


def copy_trace(config, directory, case, destination, log):
    source = config['target'] + ':' + (directory / case / 'ticks.jsonl').as_posix()
    command = ['scp', '-q', '-i', config['key'], '-o', 'IdentitiesOnly=yes', '-o', 'BatchMode=yes',
               '-o', 'StrictHostKeyChecking=yes', '-o', 'UserKnownHostsFile=' + config['known_hosts'],
               '-o', 'ConnectTimeout=10', source, str(destination)]
    with log.open('ab') as output:
        subprocess.run(command, stdout=output, stderr=subprocess.STDOUT, check=True, timeout=600)
    destination.chmod(0o600)


def main():
    repository = Path(__file__).resolve().parents[2]
    root = Path(os.environ['FRONTIERX_M3_PRIVATE_ROOT'])
    private = create(repository, 'frontierx-m3-native-cross-')
    config = json.loads((root / 'private-config/windows-ssh.json').read_text())
    if (set(config) != {'target', 'key', 'known_hosts', 'windows_private_root'} or
            not re.fullmatch(r'[A-Za-z0-9_.-]+@[A-Za-z0-9_.-]+', config['target']) or
            any(not Path(config[k]).is_absolute() or not Path(config[k]).is_file()
                for k in ('key', 'known_hosts'))):
        raise ValueError('controlled native connection unavailable')
    revision = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=repository, text=True).strip()
    run, attempt = os.environ['GITHUB_RUN_ID'], os.environ['GITHUB_RUN_ATTEMPT']
    if not all(re.fullmatch(r'[a-z0-9-]+', value) for value in (revision, run, attempt)):
        raise ValueError('invalid CI identity')
    for mode in ('measured', 'fixed-source-period'):
        try:
            windows, manifest = windows_case(config, run, attempt, revision, mode)
            inputs = []
            for case in ('repeat1', 'repeat2'):
                target = private / (mode + '-' + case + '.jsonl')
                copy_trace(config, windows, case, target, private / 'transfer-private.log')
                inputs.append(m3_compare.read_ticks(target))
            difference = m3_compare.first_difference(*inputs)
            if difference:
                print(f'FAIL: native Windows {mode} repeatability at {difference}')
            for compiler in ('gcc', 'clang'):
                linux, lmanifest = read_case(root, revision, 'mission', mode, compiler)
                lfirst = m3_compare.read_ticks(linux / 'repeat1/ticks.jsonl')
                lsecond = m3_compare.read_ticks(linux / 'repeat2/ticks.jsonl')
                ldifference = m3_compare.first_difference(lfirst, lsecond)
                if ldifference:
                    print(f'FAIL: {compiler} {mode} repeatability at {ldifference}')
                if manifest['inputs'] != lmanifest['inputs'] or manifest['protocol'] != lmanifest['protocol']:
                    print(f'FAIL: {mode} {compiler}/Windows controlled inputs or protocol differ')
                    continue
                cases = ['repeat1']
                if mode == 'fixed-source-period':
                    cases += ['256-save', '256-reload', '512-save', '512-reload']
                for case in cases:
                    target = private / (mode + '-' + case + '.jsonl')
                    if not target.exists():
                        copy_trace(config, windows, case, target, private / 'transfer-private.log')
                    first = m3_compare.read_ticks(linux / case / 'ticks.jsonl')
                    second = m3_compare.read_ticks(target)
                    mismatch = m3_compare.first_difference(first, second)
                    if mismatch:
                        print(f'FAIL: {mode} {compiler}/Windows {case} first observed field {mismatch}')
                    if difference or ldifference:
                        print('BLOCKED: same-build failure prevents port attribution')
            print('BLOCKED: native/Linux observations still have partial physics/RNG coverage')
        except (OSError, ValueError, TypeError, KeyError, subprocess.SubprocessError):
            print(f'FAIL: {mode} required native CI evidence or controlled input unavailable')
    return 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, TypeError, KeyError, subprocess.SubprocessError):
        print('FAIL: protected native comparison prerequisite unavailable', file=sys.stderr)
        sys.exit(1)
