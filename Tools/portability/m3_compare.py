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
        validate_tick(record, tick)
    if any(record.get('schema') != records[0]['schema'] for record in records):
        raise InvalidTrace('mixed observation schemas')
    if records[0]['schema'] in (4, 5, 6) and any(
            record['boundary'] != records[1]['boundary'] for record in records[1:]):
        raise InvalidTrace('mixed logic/physical observation boundaries')
    return records


def validate_tick(record, tick):
    fields = {'schema', 'coverage', 'tick', 'units', 'objectives', 'agents', 'resources'}
    if isinstance(record, dict) and record.get('schema') in (2, 3, 4, 5, 6):
        fields.add('globals')
    if isinstance(record, dict) and record.get('schema') in (3, 4, 5, 6):
        fields.add('script')
        fields.add('operations')
    if isinstance(record, dict) and record.get('schema') in (4, 5, 6):
        fields.add('boundary')
    if isinstance(record, dict) and record.get('schema') in (5, 6):
        fields.add('world')
    if isinstance(record, dict) and record.get('schema') == 6:
        fields.add('mission_globals')
    if (not isinstance(record, dict) or
            set(record) != fields or
            type(record['schema']) is not int or record['schema'] not in (1, 2, 3, 4, 5, 6) or
            record['coverage'] != 'partial' or
            type(record['tick']) is not int or record['tick'] != tick):
        raise InvalidTrace('invalid tick/schema/coverage')
    if record['schema'] in (4, 5, 6) and record['boundary'] not in (
            ('initial',) if tick == 0 else ('after-logic', 'after-physical')):
        raise InvalidTrace('invalid original observation boundary')
    if record['schema'] in (5, 6):
        validate_world(record['world'])
    if record['schema'] == 6:
        validate_script(dict(schema=1, data=record['mission_globals'], last_extended=0, programs=[]))
    if record['schema'] in (3, 4, 5, 6):
        validate_script(record['script'])
        validate_operations(record['operations'])
    if record['schema'] in (2, 3, 4, 5, 6):
        state = record['globals']
        if (not isinstance(state, dict) or set(state) != {'game_speed', 'frame_limit',
                'constant_update_rate', 'game_active', 'paused', 'world_paused'} or
                type(state['game_speed']) is not int or not -10 <= state['game_speed'] <= 10 or
                any(type(v) is not int or v not in (0, 1) for k, v in state.items()
                    if k != 'game_speed')):
            raise InvalidTrace('invalid original global control state')
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


def validate_world(world):
    def bits(values):
        return (isinstance(values, list) and len(values) == 3 and
                all(isinstance(v, str) and len(v) == 8 and
                    all(c in '0123456789abcdef' for c in v) and
                    int(v, 16) & 0x7f800000 != 0x7f800000 for v in values))
    if not isinstance(world, list) or not world:
        raise InvalidTrace('missing original world object list')
    for obj in world:
        scalars = {'id', 'class', 'owner', 'system', 'update_bin'}
        if (not isinstance(obj, dict) or set(obj) != scalars | {
                'transform_f32', 'velocity_f32', 'visibility', 'flags'} or
                any(type(obj[k]) is not int or not 0 <= obj[k] <= 0xffffffff for k in scalars) or
                not isinstance(obj['transform_f32'], list) or len(obj['transform_f32']) != 4 or
                not all(bits(v) for v in obj['transform_f32']) or not bits(obj['velocity_f32']) or
                not isinstance(obj['visibility'], list) or len(obj['visibility']) != 2 or
                any(type(v) is not int or not 0 <= v <= 255 for v in obj['visibility']) or
                not 0 <= obj['update_bin'] <= 31 or
                not isinstance(obj['flags'], list) or len(obj['flags']) != 8 or
                any(type(v) is not int or v not in (0, 1) for v in obj['flags'])):
            raise InvalidTrace('invalid original world object state')


def validate_script(script):
    def u32(value):
        return type(value) is int and 0 <= value <= 0xffffffff

    def fields(records, depth=0):
        if not isinstance(records, list) or depth > 32:
            raise InvalidTrace('invalid script field tree')
        for field in records:
            if (not isinstance(field, dict) or not isinstance(field.get('name'), str) or
                    not isinstance(field.get('type'), str)):
                raise InvalidTrace('invalid script field descriptor')
            if isinstance(field.get('value'), list):
                if set(field) != {'name', 'type', 'value'}:
                    raise InvalidTrace('invalid nested script field')
                fields(field['value'], depth + 1)
            elif (set(field) != {'name', 'type', 'value', 'width', 'bits'} or
                    type(field['width']) is not int or not 1 <= field['width'] <= 8 or
                    type(field['bits']) is not int or not 0 <= field['bits'] <= field['width'] * 8 or
                    not isinstance(field['value'], str) or len(field['value']) != 16 or
                    any(c not in '0123456789abcdef' for c in field['value']) or
                    '*' in field['type']):
                raise InvalidTrace('invalid script semantic scalar')

    if (not isinstance(script, dict) or set(script) != {'schema', 'data', 'last_extended', 'programs'} or
            type(script['schema']) is not int or script['schema'] != 1 or
            not u32(script['last_extended']) or not isinstance(script['programs'], list)):
        raise InvalidTrace('invalid script state')
    fields(script['data'])
    for program in script['programs']:
        if (not isinstance(program, dict) or set(program) != {'name', 'flags', 'state'} or
                not isinstance(program['name'], str) or not program['name'] or
                not u32(program['flags'])):
            raise InvalidTrace('invalid original script program')
        fields(program['state'])


def validate_operations(state):
    def u32(value):
        return type(value) is int and 0 <= value <= 0xffffffff
    if (not isinstance(state, dict) or set(state) != {'last_id', 'update_counter',
            'buffered_bytes', 'queues', 'termination', 'completion', 'recalculate'} or
            not all(u32(state[k]) for k in ('last_id', 'update_counter', 'buffered_bytes')) or
            not isinstance(state['queues'], list) or len(state['queues']) != 2):
        raise InvalidTrace('invalid original operation owner state')
    for queue in state['queues']:
        if not isinstance(queue, list):
            raise InvalidTrace('invalid original operation queue')
        for op in queue:
            scalars = {'id', 'command', 'packet_type', 'parent', 'group', 'user_bits', 'control_bits'}
            if (not isinstance(op, dict) or set(op) != scalars | {'flags', 'objects', 'payload'} or
                    not all(u32(op[k]) for k in scalars) or
                    not isinstance(op['flags'], list) or len(op['flags']) != 4 or
                    any(type(x) is not int or x not in (0, 1) for x in op['flags']) or
                    not isinstance(op['objects'], list) or not all(u32(x) for x in op['objects'])):
                raise InvalidTrace('invalid original operation header/set')
            payload = op['payload']
            if payload is not None and (not isinstance(payload, dict) or
                    set(payload) != {'grid_quarters', 'system'} or
                    not (op['command'] == 4 or
                         (op['command'] == 0 and op['packet_type'] == 0xfffffff4)) or
                    type(payload['system']) is not int or not 0 <= payload['system'] <= 255 or
                    not isinstance(payload['grid_quarters'], list) or len(payload['grid_quarters']) != 2 or
                    any(type(x) is not int or not 0 <= x <= 255 for x in payload['grid_quarters'])):
                raise InvalidTrace('unsupported original operation payload')
    for name in ('termination', 'completion'):
        if (not isinstance(state[name], list) or any(not isinstance(pair, list) or
                len(pair) != 2 or not all(u32(x) for x in pair) for pair in state[name])):
            raise InvalidTrace('invalid deferred operation list')
    if not isinstance(state['recalculate'], list) or not all(u32(x) for x in state['recalculate']):
        raise InvalidTrace('invalid operation dependency queue')


def read_checkpoint(path):
    record = load_json(Path(path).read_text(encoding='utf-8'))
    if not isinstance(record, dict) or record.get('tick') not in (256, 512):
        raise InvalidTrace('invalid checkpoint tick')
    validate_tick(record, record['tick'])
    if record['schema'] in (4, 5, 6) and record['boundary'] != 'after-physical':
        raise InvalidTrace('checkpoint requires completed physical boundary')
    return record


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


def read_randomgen(path):
    value = load_json(Path(path).read_text(encoding='utf-8'))
    if (not isinstance(value, dict) or set(value) != {'schema', 'owner', 'modulus', 'state_bytes', 'sequences'} or
            type(value['schema']) is not int or value['schema'] != 1 or value['owner'] != 'RandomGen' or
            type(value['modulus']) is not int or value['modulus'] != 32767 or
            type(value['state_bytes']) is not int or value['state_bytes'] != 4 or
            not isinstance(value['sequences'], list) or len(value['sequences']) != 4):
        raise InvalidTrace('invalid original RandomGen contract')
    for expected, sequence in zip((0, 1, 17, 254), value['sequences']):
        if (not isinstance(sequence, dict) or set(sequence) != {'seed', 'draws'} or
                type(sequence['seed']) is not int or sequence['seed'] != expected or
                not isinstance(sequence['draws'], list) or len(sequence['draws']) != 32 or
                any(type(draw) is not int or not 0 <= draw < 32767 for draw in sequence['draws'])):
            raise InvalidTrace('invalid original RandomGen sequence')
    return value


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('kind', choices=['rng', 'randomgen', 'ticks', 'checkpoint'])
    parser.add_argument('left')
    parser.add_argument('right')
    parser.add_argument('--observed-only', action='store_true',
                        help='compare partial observation; never an M3 acceptance')
    args = parser.parse_args(argv)
    try:
        reader = {'rng': read_rng, 'randomgen': read_randomgen,
                  'ticks': read_ticks, 'checkpoint': read_checkpoint}[args.kind]
        left, right = reader(args.left), reader(args.right)
        difference = first_difference(left, right)
    except (OSError, ValueError, TypeError, KeyError):
        print('FAIL: invalid or unavailable comparison input', file=sys.stderr)
        return 2
    if difference:
        print('FAIL: first differing field ' + difference, file=sys.stderr)
        return 1
    if args.kind not in ('rng', 'randomgen') and not args.observed_only:
        print('BLOCKED: partial coverage omits full physics, transport payloads and RNG state',
              file=sys.stderr)
        return 2
    description = {'rng': 'CRT characterization only', 'randomgen': 'original RandomGen synthetic contract only'}
    print('Equal observed fields; ' + description.get(args.kind, 'partial coverage, no M3 acceptance'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
