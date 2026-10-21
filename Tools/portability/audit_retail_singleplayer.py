#!/usr/bin/env python3
"""Read-only evidence for the named retail catalogue blockers.

Uses the engine's fixed UTF 1.1 header/44-byte directory contract. Reports
named references and model presence only, never inferred numeric reachability,
asset substitution, gameplay support or a successful catalogue gate. Write the
report outside the checkout because it identifies external archive contents.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct


BLOCKERS = {
    'PLATGUN!!T_DefenseBattery': 'TFIGHTDEF_PLAT.3DB',
    'PLATBUILD!!M_AntennaBeacon': 'Meye.cmp',
}


def utf_files(blob):
    if len(blob) < 56:
        raise ValueError('truncated UTF header')
    header = struct.unpack_from('<14I', blob)
    magic, version, directory, length, free, stride = header[:6]
    names, names_size, names_used, data = header[6:10]
    if magic != 0x20465455 or version != 0x101 or stride != 44 or not length or length % stride:
        raise ValueError('unsupported UTF header')
    if (directory < 56 or names < 56 or names_used > names_size or
            directory + length > len(blob) or names + names_size > len(blob) or
            (directory < names + names_size and names < directory + length) or
            data < max(directory + length,names + names_size) or
            data > len(blob) or length + names_used > 64 * 1024 * 1024):
        raise ValueError('invalid UTF metadata ranges')
    table = blob[names:names + names_used]
    pending, visited, result = [(0, '')], set(), {}
    while pending:
        offset, parent = pending.pop()
        if offset % stride or offset >= length or offset in visited:
            raise ValueError('invalid/cyclic UTF link')
        visited.add(offset)
        fields = struct.unpack_from('<11I', blob, directory + offset)
        sibling, name, attributes = fields[:3]
        if attributes == 0xffffffff or name >= len(table):
            raise ValueError('invalid UTF entry')
        end = table.find(b'\0', name)
        if end < 0:
            raise ValueError('unterminated UTF name')
        leaf = table[name:end].decode('ascii')
        if offset and (not leaf or '/' in leaf or '\\' in leaf):
            raise ValueError('invalid UTF component')
        path = parent + leaf if offset else ''
        if sibling:
            pending.append((sibling, parent))
        if attributes & 0x10:
            if fields[4]:
                pending.append((fields[4], path + '/' if path else ''))
        else:
            content, allocated, used, unpacked = fields[4:8]
            if used > allocated or data + content + allocated > len(blob) or used != unpacked:
                raise ValueError('invalid/encoded UTF payload')
            if path in result:
                raise ValueError('duplicate UTF path')
            result[path] = blob[data + content:data + content + used]
    return result


def inspect(root, source):
    archives = {}
    fingerprints = {}
    for name in ('gametypes.db', 'gendata.db', 'DATA/maps', 'DATA/objects'):
        blob = (root / name).read_bytes()
        fingerprints[name] = hashlib.sha256(blob).hexdigest()
        archives[name] = utf_files(blob)
    references = {name: [] for name in BLOCKERS}
    nested_maps = 0
    def scan(label, files):
        nonlocal nested_maps
        for path, payload in files.items():
            lower = payload.lower()
            for name in references:
                in_path = name.casefold() in [part.casefold() for part in path.split('/')]
                # Map instances can identify their archetype by directory name.
                # Only the catalogue's own declaration is not a reference.
                if (in_path and label != 'gametypes.db') or (
                        not in_path and name.lower().encode('ascii') in lower):
                    references[name].append(label + ':' + path)
            if label == 'DATA/maps' and payload.startswith(b'UTF '):
                nested_maps += 1
                scan('map/' + path, utf_files(payload))
    for label in ('gametypes.db', 'gendata.db', 'DATA/maps'):
        scan(label, archives[label])
    for path in (source / 'Code').rglob('*'):
        if path.suffix.lower() not in ('.cpp', '.h', '.rc'):
            continue
        lower = path.read_bytes().lower()
        for name in references:
            if name.lower().encode('ascii') in lower:
                references[name].append(str(path.relative_to(source)))
    objects = {path.casefold() for path in archives['DATA/objects']}
    return {
        'evidence': 'read-only external metadata and source text audit',
        'archive_sha256': fingerprints,
        'nested_maps_inspected': nested_maps,
        'blockers': {name: {
            'required_model': model,
            'model_present': model.casefold() in objects,
            'named_references': references[name],
        } for name, model in BLOCKERS.items()},
        'limits': ['No numeric/script reachability proof.',
                   'No failed record excluded; no replacement asset selected.',
                   'No factory, instance, ability, combat or save/load acceptance.'],
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--retail-root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    source = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if output.is_relative_to(source):
        parser.error('external-data reports must stay outside the checkout')
    report = inspect(args.retail_root, source)
    output.write_text(json.dumps(report, indent=2) + '\n')
    for name, finding in report['blockers'].items():
        print(name, 'model_present=', finding['model_present'],
              'named_references=', len(finding['named_references']))
    print('This is evidence, not a catalogue-completeness pass.')


if __name__ == '__main__':
    main()
