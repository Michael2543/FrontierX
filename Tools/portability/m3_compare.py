"""Exact M3 diagnostic comparisons. Partial traces never satisfy acceptance."""
import argparse
import json
from pathlib import Path
import sys


class InvalidTrace(ValueError):
    pass


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise InvalidTrace('duplicate JSON key')
        result[key] = value
    return result


def load_json(text):
    return json.loads(text, object_pairs_hook=unique_object,
                      parse_constant=lambda _: (_ for _ in ()).throw(
                          InvalidTrace('nonfinite JSON number')))


def first_difference(left, right, path='$'):
    if type(left) is not type(right):
        return path
    if isinstance(left, dict):
        if left.keys() != right.keys():
            return path + '.fields'
        for key in sorted(left):
            result = first_difference(left[key], right[key], path + '.' + key)
            if result:
                return result
    elif isinstance(left, list):
        if len(left) != len(right):
            return path + '.length'
        for index, (a, b) in enumerate(zip(left, right)):
            result = first_difference(a, b, f'{path}[{index}]')
            if result:
                return result
    elif left != right:
        return path
    return None


def read_ticks(path):
    # Paths and input values are intentionally absent from diagnostics.
    with Path(path).open(encoding='utf-8') as source:
        records = [load_json(line) for line in source if line.strip()]
    if len(records) != 1025:
        raise InvalidTrace('expected all ticks 0..1024')
    for tick, record in enumerate(records):
        if (not isinstance(record, dict) or
                set(record) != {'schema', 'coverage', 'tick', 'units',
                                'objectives', 'agents', 'resources'} or
                type(record['schema']) is not int or record['schema'] != 1 or
                record['coverage'] != 'partial' or
                type(record['tick']) is not int or record['tick'] != tick):
            raise InvalidTrace('invalid tick/schema/coverage')
        # Preserve source linked-list order: changing it may change simulation.
        ids = []
        for unit in record['units']:
            if set(unit) != {'id', 'class', 'owner', 'system', 'position_f32',
                             'hull', 'supplies', 'selected', 'ready',
                             'pending_valid', 'pending_op', 'last_op', 'group'}:
                raise InvalidTrace('invalid unit fields')
            ids.append(unit['id'])
            if any(type(v) is not int or not 0 <= v <= 0xffffffff for k, v in unit.items()
                   if k != 'position_f32'):
                raise InvalidTrace('invalid unit scalar')
            bits = unit['position_f32']
            if (not isinstance(bits, list) or len(bits) != 3 or
                    any(not isinstance(v, str) or len(v) != 8 or
                        any(c not in '0123456789abcdef' for c in v) for v in bits)):
                raise InvalidTrace('invalid f32 bits')
            if any(int(v, 16) & 0x7f800000 == 0x7f800000 for v in bits):
                raise InvalidTrace('nonfinite position')
        if not ids or len(ids) != len(set(ids)):
            raise InvalidTrace('empty/duplicate mission unit identity')
        objective_ids = []
        for objective in record['objectives']:
            if set(objective) != {'id', 'completed', 'failed', 'secondary'}:
                raise InvalidTrace('invalid objective fields')
            objective_ids.append(objective['id'])
            if (type(objective['id']) is not int or
                    any(type(objective[k]) is not int or objective[k] not in (0, 1)
                        for k in ('completed', 'failed', 'secondary'))):
                raise InvalidTrace('invalid objective scalar')
        if len(objective_ids) != len(set(objective_ids)):
            raise InvalidTrace('duplicate objective identity')
        if (type(record['agents']) is not int or record['agents'] < 0 or
                not isinstance(record['resources'], list) or
                len(record['resources']) != 3 or
                any(type(v) is not int or v < 0 for v in record['resources'])):
            raise InvalidTrace('invalid resource/agent fields')
    return records


def read_rng(path):
    value = load_json(Path(path).read_text(encoding='utf-8'))
    if (set(value) != {'schema', 'seed', 'rand_max', 'draws'} or
            type(value['schema']) is not int or value['schema'] != 1 or
            type(value['seed']) is not int or value['seed'] != 1 or
            type(value['rand_max']) is not int or value['rand_max'] <= 0 or
            not isinstance(value['draws'], list) or len(value['draws']) != 16 or
            any(type(v) is not int or not 0 <= v <= value['rand_max']
                for v in value['draws'])):
        raise InvalidTrace('invalid CRT RNG contract')
    return value


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('kind', choices=['rng', 'ticks'])
    parser.add_argument('left')
    parser.add_argument('right')
    parser.add_argument('--observed-only', action='store_true',
                        help='compare partial observation; never an M3 acceptance')
    args = parser.parse_args(argv)
    try:
        reader = read_rng if args.kind == 'rng' else read_ticks
        left, right = reader(args.left), reader(args.right)
        difference = first_difference(left, right)
    except (OSError, ValueError, TypeError, KeyError):
        print('FAIL: invalid or unavailable comparison input', file=sys.stderr)
        return 2
    if difference:
        print('FAIL: first differing field ' + difference, file=sys.stderr)
        return 1
    if args.kind == 'ticks' and not args.observed_only:
        print('BLOCKED: partial coverage omits RNG, script state and queued order payloads',
              file=sys.stderr)
        return 2
    print('Equal observed fields; ' + ('CRT characterization only' if args.kind == 'rng'
                                     else 'partial coverage, no M3 acceptance'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
