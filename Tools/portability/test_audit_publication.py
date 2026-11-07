"""Protect historical exposure detection using an isolated synthetic Git repo."""

import contextlib
import io
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
import zipfile
from unittest.mock import patch

import audit_publication as audit


class PublicationAuditTest(unittest.TestCase):
    def test_deleted_token_and_changed_binary_remain_visible(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)

            def git(*args):
                return subprocess.check_output(["git", "-C", directory, *args],
                                               stderr=subprocess.DEVNULL)

            def commit(message):
                git("add", ".")
                git("-c", "user.name=Publication audit fixture", "-c",
                    "user.email=fixture@example.invalid", "-c", "commit.gpgsign=false",
                    "commit", "-qm", message)

            git("init", "-q")
            token = "ghp_" + "A" * 36
            (root / "deleted.txt").write_text(token)
            (root / "library name.dll").write_bytes(b"MZsynthetic old binary")
            commit("Synthetic baseline")
            baseline = git("rev-parse", "HEAD").decode().strip()
            (root / "deleted.txt").unlink()
            (root / "library name.dll").write_bytes(b"MZsynthetic changed binary")
            commit("Synthetic removal")
            with patch.object(audit, "BASELINE", baseline):
                result = audit.audit(root)
                archive = root / "source.zip"
                with zipfile.ZipFile(archive, "w") as package:
                    package.writestr("Conquest/library name.dll", b"MZsynthetic old binary")
                comparison = audit.compare_source_archive(root, archive)
            compared = comparison["files"][0]
            self.assertEqual(compared["status"], "changed")
            self.assertTrue(compared["baseline_matches_source"])
            self.assertEqual(result["commit_count"], 2)
            findings = result["credential_pattern_findings"]
            self.assertEqual(len(findings), 1)
            self.assertEqual(findings[0]["scope"], "history")
            self.assertEqual(findings[0]["paths"], ["deleted.txt"])
            self.assertNotIn(token, json.dumps(result))
            binary = result["current_candidates"][0]
            self.assertNotEqual(binary["baseline_blob"], binary["working_git_blob"])
            historical = result["historical_candidates"][0]
            self.assertEqual(len(historical["blobs"]), 2)

    def test_output_inside_repository_is_rejected_before_scan(self):
        with patch("sys.argv", ["audit_publication.py", "--output", str(Path(audit.__file__).parent / "report.json")]), \
                patch.object(audit, "audit") as scan, contextlib.redirect_stderr(io.StringIO()):
            with self.assertRaises(SystemExit) as error:
                audit.main()
            self.assertEqual(error.exception.code, 2)
            scan.assert_not_called()

    def test_policy_names_and_nonsecret_text(self):
        self.assertIn("explicit_policy_exclusion", audit.categories("nested/BINKW32.DLL"))
        self.assertIn("explicit_policy_exclusion", audit.categories("Conquest_Frontier_Wars_Manual.pdf"))
        self.assertEqual(audit.credential_kinds(b"ordinary source text"), [])


if __name__ == "__main__":
    unittest.main()
