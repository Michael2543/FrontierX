"""Compare retained CI mission observations on the same protected Linux host.

No private traces are uploaded. Incomplete coverage or failed repeatability
remains a failure; cross-compiler observations then characterize, not isolate,
a compiler defect.
"""
import json
import os
from pathlib import Path
import subprocess
import sys
import re

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'portability'))
import m3_compare


def read_case(root, revision, scope, mode, compiler):
    run = os.environ['GITHUB_RUN_ID']
    attempt = os.environ['GITHUB_RUN_ATTEMPT']
    path = root / 'mission-index' / f'{run}-{attempt}-{revision}-{scope}-{mode}-{compiler}.json'
    record = json.loads(path.read_text())
    directory = Path(record['private_directory']).resolve()
    if not directory.is_relative_to(root.resolve()):
        raise ValueError('unsafe private evidence location')
    manifest = record['manifest']
    inputs = manifest['inputs']
    if (manifest['schema'] != 1 or set(inputs) != {'stringpack.db', 'gendata.db',
            'gametypes.db', 'maps', 'objects', 'mspeech', 'speech', 'textures', 'font'} or
            any(set(item) != {'bytes', 'sha256'} or type(item['bytes']) is not int or
                item['bytes'] <= 0 or not re.fullmatch(r'[0-9a-f]{64}', item['sha256'])
                for item in inputs.values())):
        raise ValueError('input manifest incomplete')
    execution = manifest['execution']
    if (execution['revision'] != revision or execution['working_tree_dirty'] or
            execution['workflow_run'] != run or execution['attempt'] != attempt or
            execution['compiler'] != compiler or execution['scope'] != scope or
            manifest['protocol']['frame_input'] != mode):
        raise ValueError('CI provenance mismatch')
    return directory, manifest


def main():
    repository = Path(__file__).resolve().parents[2]
    revision = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=repository, text=True).strip()
    root = Path(os.environ['FRONTIERX_M3_PRIVATE_ROOT'])
    if not root.is_absolute():
        raise ValueError('private evidence root missing')
    for scope in ('mission', 'mission-sanitizers'):
        for mode in ('measured', 'fixed-source-period'):
            try:
                gcc, gmanifest = read_case(root, revision, scope, mode, 'gcc')
                clang, cmanifest = read_case(root, revision, scope, mode, 'clang')
                if (gmanifest['inputs'] != cmanifest['inputs'] or
                        gmanifest['protocol'] != cmanifest['protocol']):
                    raise ValueError('input provenance differs')
                builds = []
                repeatable = True
                for compiler, directory in (('gcc', gcc), ('clang', clang)):
                    first = m3_compare.read_ticks(directory / 'repeat1/ticks.jsonl')
                    second = m3_compare.read_ticks(directory / 'repeat2/ticks.jsonl')
                    difference = m3_compare.first_difference(first, second)
                    if difference:
                        repeatable = False
                        print(f'FAIL: {scope} {mode} {compiler} repeatability at {difference}')
                    builds.append(first)
                cases = ['repeat1']
                if mode == 'fixed-source-period':
                    cases += ['256-save', '256-reload', '512-save', '512-reload']
                for case in cases:
                    first = m3_compare.read_ticks(gcc / case / 'ticks.jsonl')
                    second = m3_compare.read_ticks(clang / case / 'ticks.jsonl')
                    difference = m3_compare.first_difference(first, second)
                    if difference:
                        print(f'FAIL: {scope} {mode} {case} GCC/Clang observed field {difference}')
                if not repeatable:
                    print('BLOCKED: failed same-build repeatability prevents compiler attribution')
                print('BLOCKED: private mission observation still has partial coverage')
            except (OSError, ValueError, TypeError, KeyError):
                print(f'FAIL: {scope} {mode} required private CI input absent or invalid')
    return 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, subprocess.SubprocessError):
        print('FAIL: controlled cross-build infrastructure or provenance unavailable', file=sys.stderr)
        sys.exit(1)
