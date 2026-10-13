"""Hash bounded lawful external inputs without exposing installation paths."""
import hashlib
import json
from pathlib import Path
import sys


def one_child(root, name):
    matches = [p for p in root.iterdir() if p.name.casefold() == name.casefold()]
    if len(matches) != 1:
        raise ValueError('missing or ambiguous logical input')
    return matches[0]


def digest(path):
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return {'bytes': path.stat().st_size, 'sha256': h.hexdigest()}


def manifest(installation, font):
    installation = Path(installation)
    data = one_child(installation, 'data')
    result = {'schema': 1, 'inputs': {}}
    for name in ('stringpack.db', 'gendata.db', 'gametypes.db'):
        result['inputs'][name] = digest(one_child(installation, name))
    for name in ('maps', 'objects', 'mspeech', 'speech', 'textures'):
        result['inputs'][name] = digest(one_child(data, name))
    result['inputs']['font'] = digest(Path(font))
    return result


if __name__ == '__main__':
    try:
        # The caller redirects this minimal manifest to private runner storage.
        print(json.dumps(manifest(sys.argv[1], sys.argv[2]), sort_keys=True))
    except (OSError, ValueError, IndexError):
        print('FAIL: external input manifest unavailable', file=sys.stderr)
        sys.exit(1)
