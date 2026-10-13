"""Mandatory execution gates reject skips, absent scopes and failures."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest

import check_ctest


class GateTests(unittest.TestCase):
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
