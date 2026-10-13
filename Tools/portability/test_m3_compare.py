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


if __name__ == '__main__':
    unittest.main()
