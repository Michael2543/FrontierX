"""Mandatory execution gates reject skips, absent scopes and failures."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import os
import shutil

import check_ctest
import private_workspace


class GateTests(unittest.TestCase):
    def test_private_evidence_survives_runner_temp_cleanup(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            checkout, transient = root / 'checkout', root / 'runner-temp'
            checkout.mkdir()
            transient.mkdir()
            with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': str(root / 'private'),
                                         'RUNNER_TEMP': str(transient)}):
                evidence = private_workspace.create(checkout, 'test-')
                record = evidence / 'generated.txt'
                record.write_text('generated evidence')
                shutil.rmtree(transient)
                self.assertEqual(record.read_text(), 'generated evidence')
                if os.name != 'nt':
                    self.assertEqual(evidence.parent.stat().st_mode & 0o077, 0)

    def test_private_evidence_rejects_disposable_and_protected_roots(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            checkout, transient, retail = (root / name for name in ('checkout', 'runner-temp', 'retail'))
            for directory in (checkout, transient, retail):
                directory.mkdir()
            for directory in (checkout, transient, retail):
                with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': str(directory / 'evidence'),
                                             'RUNNER_TEMP': str(transient),
                                             'FRONTIERX_RETAIL_ROOT': str(retail)}):
                    with self.assertRaises(RuntimeError):
                        private_workspace.create(checkout, 'test-')
            alias = root / 'alias'
            alias.symlink_to(checkout, target_is_directory=True)
            with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': str(alias / 'evidence')}):
                with self.assertRaises(RuntimeError):
                    private_workspace.create(checkout, 'test-')

    def test_private_evidence_requires_explicit_absolute_configuration(self):
        for value in ('', 'relative-evidence'):
            with patch.dict(os.environ, {'FRONTIERX_M3_PRIVATE_ROOT': value}):
                with self.assertRaises(RuntimeError):
                    private_workspace.create(Path.cwd(), 'test-')

    def test_report_requires_actual_successful_testcases(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'generated.xml'
            for content, expected in [
                ('<testsuite/>', 1),
                ('<testsuite><testcase><skipped/></testcase></testsuite>', 1),
                ('<testsuite><testcase><failure/></testcase></testsuite>', 1),
                ('<testsuite><testcase><error/></testcase></testsuite>', 1),
                ('not XML', 1),
                ('<testsuite><testcase/></testsuite>', 0),
            ]:
                path.write_text(content)
                with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                    self.assertEqual(check_ctest.main(path), expected)


if __name__ == '__main__':
    unittest.main()
