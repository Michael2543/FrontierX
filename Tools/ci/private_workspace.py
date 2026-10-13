"""Retain private CI evidence outside checkout and disposable runner directories."""
import argparse
import os
from pathlib import Path
import tempfile
import sys


def create(repository, prefix):
    configured = os.environ.get('FRONTIERX_M3_PRIVATE_ROOT')
    if not configured or not Path(configured).is_absolute():
        raise RuntimeError('absolute retained private evidence root required')
    parent = Path(configured).resolve()
    excluded = [Path(repository).resolve()]
    for name in ('RUNNER_TEMP', 'FRONTIERX_RETAIL_ROOT'):
        if os.environ.get(name):
            excluded.append(Path(os.environ[name]).resolve())
    if any(parent.is_relative_to(path) for path in excluded):
        raise RuntimeError('private evidence must be outside source, runner temp and retail roots')
    parent.mkdir(mode=0o700, parents=True, exist_ok=True)
    if os.name != 'nt':
        parent.chmod(0o700)
    return Path(tempfile.mkdtemp(prefix=prefix, dir=parent))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--repository', type=Path, required=True)
    parser.add_argument('--prefix', required=True)
    args = parser.parse_args()
    # Windows callers capture this output; never echo it to public job logs.
    try:
        print(create(args.repository, args.prefix))
    except (RuntimeError, OSError):
        print('FAIL: retained private evidence root unavailable or unsafe', file=sys.stderr)
        sys.exit(2)
