"""Comparator regressions use generated metadata, never fabricated game evidence."""
import contextlib
import copy
import io
import json
from pathlib import Path
import tempfile
import unittest

import m3_compare as compare


class ComparisonTests(unittest.TestCase):
    def test_world_keeps_non_mission_objects_orientation_and_velocity_bits(self):
        obj = dict(id=0, **{'class': 1}, owner=0, system=1, update_bin=0,
                   transform_f32=[['00000000'] * 3 for _ in range(4)],
                   velocity_f32=['00000000'] * 3, visibility=[0, 0], flags=[0] * 8)
        compare.validate_world([obj])
        changed = copy.deepcopy(obj)
        changed['transform_f32'][0][1] = '80000000'
        self.assertEqual(compare.first_difference(obj, changed), '$.transform_f32[0][1]')
        changed['velocity_f32'][0] = '7f800000'
        with self.assertRaises(compare.InvalidTrace):
            compare.validate_world([changed])

    def test_engine_angular_velocity_is_nullable_and_compared_exactly(self):
        obj = dict(id=0, **{'class': 1}, owner=0, system=1, update_bin=0,
                   transform_f32=[['00000000'] * 3 for _ in range(4)],
                   velocity_f32=['00000000'] * 3, visibility=[0, 0], flags=[0] * 8,
                   engine_angular_velocity_f32=None)
        compare.validate_world([obj], engine_state=True)
        changed = copy.deepcopy(obj)
        changed['engine_angular_velocity_f32'] = ['00000000'] * 3
        compare.validate_world([changed], engine_state=True)
        self.assertEqual(compare.first_difference(obj, changed), '$.engine_angular_velocity_f32')
        obj['engine_angular_velocity_f32'] = ['00000000'] * 3
        changed['engine_angular_velocity_f32'][0] = '80000000'
        self.assertEqual(compare.first_difference(obj, changed), '$.engine_angular_velocity_f32[0]')
        changed['engine_angular_velocity_f32'] = ['00000000'] * 2
        with self.assertRaises(compare.InvalidTrace):
            compare.validate_world([changed], engine_state=True)

    def test_operation_payload_and_queue_order_are_comparison_inputs(self):
        op = dict(id=1, command=4, packet_type=0xfffffff4, parent=0, group=0, user_bits=0, control_bits=0,
                  flags=[0, 0, 0, 0], objects=[11, 12],
                  payload=dict(grid_quarters=[80, 120], system=1))
        state = dict(last_id=1, update_counter=64, buffered_bytes=0,
                     queues=[[op], []], termination=[], completion=[], recalculate=[])
        compare.validate_operations(state)
        other = copy.deepcopy(state)
        other['queues'][0][0]['payload']['grid_quarters'][0] += 1
        self.assertEqual(compare.first_difference(state, other),
                         '$.queues[0][0].payload.grid_quarters[0]')
        other = copy.deepcopy(state)
        other['queues'][0][0]['objects'].reverse()
        self.assertEqual(compare.first_difference(state, other), '$.queues[0][0].objects[0]')
        other['queues'][0][0]['payload']['raw_padding'] = 'ignored'
        with self.assertRaises(compare.InvalidTrace):
            compare.validate_operations(other)

    def test_script_program_state_and_identity_are_semantic_comparison_inputs(self):
        field = dict(name='state', type='State', value='0000000000000001', width=4, bits=0)
        script = dict(schema=1, data=[], last_extended=0,
                      programs=[dict(name='OriginalProgram', flags=1, state=[field])])
        compare.validate_script(script)
        other = copy.deepcopy(script)
        other['programs'][0]['state'][0]['value'] = '0000000000000002'
        self.assertEqual(compare.first_difference(script, other), '$.programs[0].state[0].value')
        other['programs'][0]['state'][0]['type'] = 'void*'
        with self.assertRaises(compare.InvalidTrace):
            compare.validate_script(other)
        other = copy.deepcopy(script)
        other['programs'][0]['state'][0]['padding'] = 'ignored'
        with self.assertRaises(compare.InvalidTrace):
            compare.validate_script(other)

    def test_nested_first_difference_keeps_float_bits_and_order(self):
        a = {'units': [{'position_f32': ['00000000', '3f800000', '00000000']}]}
        b = copy.deepcopy(a)
        b['units'][0]['position_f32'][0] = '80000000'
        self.assertEqual(compare.first_difference(a, b), '$.units[0].position_f32[0]')
        self.assertEqual(compare.first_difference([2, 1], [1, 2]), '$[0]')
        self.assertEqual(compare.first_difference(1, True), '$')

    def test_rejects_duplicate_keys_and_nonfinite_values(self):
        for text in ('{"tick":0,"tick":1}', '{"position":NaN}'):
            with self.assertRaises(compare.InvalidTrace):
                compare.load_json(text)

    def test_requires_every_tick_and_never_accepts_partial_coverage(self):
        unit = dict(id=1, owner=1, system=1, **{'class': 1},
                    position_f32=['00000000'] * 3, hull=10, supplies=10,
                    selected=0, ready=1, pending_valid=0, pending_op=0,
                    last_op=0, group=0)
        records = [dict(schema=1, coverage='partial', tick=tick, units=[unit],
                        objectives=[], agents=0, resources=[1, 1, 1])
                   for tick in range(1025)]
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'generated.jsonl'
            path.write_text(''.join(json.dumps(v) + '\n' for v in records))
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(compare.main(['ticks', str(path), str(path)]), 2)
                self.assertEqual(compare.main(['ticks', str(path), str(path), '--observed-only']), 0)
            records.pop(32)
            path.write_text(''.join(json.dumps(v) + '\n' for v in records))
            with self.assertRaises(compare.InvalidTrace):
                compare.read_ticks(path)

    def test_missing_rng_draws_are_not_equal_evidence(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'rng.json'
            path.write_text('{"schema":1,"seed":1,"rand_max":32767,"draws":[]}')
            with self.assertRaises(compare.InvalidTrace):
                compare.read_rng(path)
            contract = dict(schema=1, owner='RandomGen', modulus=32767, state_bytes=4,
                            sequences=[dict(seed=seed, draws=[0] * 32) for seed in (0, 1, 17, 254)])
            path.write_text(json.dumps(contract))
            self.assertEqual(compare.read_randomgen(path), contract)
            contract['sequences'][0]['draws'].pop()
            path.write_text(json.dumps(contract))
            with self.assertRaises(compare.InvalidTrace):
                compare.read_randomgen(path)
            contract['sequences'][0]['draws'].append(0)
            contract['state_bytes'] = 8
            path.write_text(json.dumps(contract))
            with self.assertRaises(compare.InvalidTrace):
                compare.read_randomgen(path)

    def test_checkpoint_observation_never_counts_as_complete_save_semantics(self):
        unit = dict(id=1, owner=1, system=1, **{'class': 1},
                    position_f32=['00000000'] * 3, hull=10, supplies=10,
                    selected=0, ready=1, pending_valid=0, pending_op=0,
                    last_op=0, group=0)
        record = dict(schema=1, coverage='partial', tick=256, units=[unit],
                      objectives=[], agents=0, resources=[1, 1, 1])
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'generated.json'
            path.write_text(json.dumps(record))
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(compare.main(['checkpoint', str(path), str(path)]), 2)
                self.assertEqual(compare.main(['checkpoint', str(path), str(path), '--observed-only']), 0)
            record['tick'] = 257
            path.write_text(json.dumps(record))
            with self.assertRaises(compare.InvalidTrace):
                compare.read_checkpoint(path)
            record.update(schema=2, tick=256, globals=dict(game_speed=0, frame_limit=1,
                          constant_update_rate=0, game_active=1, paused=0, world_paused=0))
            path.write_text(json.dumps(record))
            self.assertEqual(compare.read_checkpoint(path)['globals']['game_speed'], 0)
            record['globals']['game_speed'] = 11
            path.write_text(json.dumps(record))
            with self.assertRaises(compare.InvalidTrace):
                compare.read_checkpoint(path)
            record['globals']['game_speed'] = 0
            record.update(schema=4, boundary='after-physical',
                          script=dict(schema=1, data=[], last_extended=0, programs=[]),
                          operations=dict(last_id=0, update_counter=256, buffered_bytes=0,
                                          queues=[[], []], termination=[], completion=[], recalculate=[]))
            path.write_text(json.dumps(record))
            self.assertEqual(compare.read_checkpoint(path)['boundary'], 'after-physical')
            other = copy.deepcopy(record)
            other['boundary'] = 'after-logic'
            self.assertEqual(compare.first_difference(record, other), '$.boundary')
            path.write_text(json.dumps(other))
            with self.assertRaises(compare.InvalidTrace):
                compare.read_checkpoint(path)


if __name__ == '__main__':
    unittest.main()
