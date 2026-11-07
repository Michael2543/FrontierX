#!/usr/bin/env python3
"""Inventory publication review candidates; never grant distribution approval.

Read local Git history and tracked working files. Output metadata only, never
matched credential contents. Untracked files, remote-only refs, GitHub artifacts,
and arbitrary credential formats are outside this deliberately bounded audit.
"""

import argparse
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
import re
import subprocess
import zipfile


BASELINE = "f66a257d538a4c476f561b9920a70efc43cd7912"
BINARY = {".exe", ".dll", ".lib", ".a", ".so", ".ocx", ".msi"}
GENERATED = {".pdb", ".obj", ".pch", ".ilk", ".idb", ".ncb", ".sbr", ".bsc", ".plg"}
MEDIA = {".wav", ".mp3", ".ogg", ".mpg", ".mpeg", ".bik", ".tga", ".bmp",
         ".png", ".jpg", ".ico", ".cur", ".wmf", ".3db", ".cmp", ".anm",
         ".shp", ".shield", ".pte", ".qmission", ".dmission", ".mission", ".ttf", ".otf"}
ARCHIVE = {".zip", ".7z", ".rar", ".cab", ".iso", ".pdf", ".doc", ".chm"}
TOKENS = {
    "github_token": re.compile(rb"\bgh[pousr]_[A-Za-z0-9]{36,255}\b"),
    "github_fine_grained_token": re.compile(rb"\bgithub_pat_[A-Za-z0-9_]{60,255}\b"),
    "aws_access_key_id": re.compile(rb"\b(?:AKIA|ASIA)[A-Z0-9]{16}\b"),
    "private_key_block": re.compile(rb"-----BEGIN (?:RSA |EC |OPENSSH |DSA )?PRIVATE KEY-----[\r\n]+[A-Za-z0-9+/=]{16}"),
}


def categories(path):
    name = Path(path).name.lower()
    suffix = Path(path).suffix.lower()
    result = []
    for label, extensions in (("binary", BINARY), ("generated", GENERATED),
                              ("media", MEDIA), ("archive_or_document", ARCHIVE)):
        if suffix in extensions:
            result.append(label)
    if name in {"binkw32.dll", "conquest_frontier_wars_manual.pdf"}:
        result.append("explicit_policy_exclusion")
    if name == ".env" or name.startswith(".env.") or suffix in {".pem", ".key", ".p12", ".pfx", ".dmp"}:
        result.append("sensitive_filename")
    return result


def credential_kinds(data):
    return [name for name, pattern in TOKENS.items() if pattern.search(data)]


def git(root, *args):
    return subprocess.check_output(["git", "-C", str(root), *args])


def tree(root, revision):
    records = {}
    for entry in git(root, "ls-tree", "-rz", revision).split(b"\0"):
        if not entry:
            continue
        header, path = entry.split(b"\t", 1)
        mode, kind, oid = header.decode("ascii").split()
        records[path.decode("utf-8", "surrogateescape")] = (mode, kind, oid)
    return records


def compare_source_archive(root, archive):
    """Compare bytes without extracting or importing any source-release files."""
    baseline = tree(root, BASELINE)
    head = tree(root, "HEAD")
    source = {}
    with archive.open("rb") as stream:
        archive_hash = hashlib.file_digest(stream, "sha256").hexdigest()
    with zipfile.ZipFile(archive) as package:
        for entry in package.infolist():
            if entry.is_dir():
                continue
            if not entry.filename.startswith("Conquest/"):
                raise ValueError("Unexpected source-release archive root")
            path = entry.filename.removeprefix("Conquest/")
            if path in source or ".." in Path(path).parts:
                raise ValueError("Duplicate or traversing source-release path")
            digest = hashlib.sha1(b"blob " + str(entry.file_size).encode() + b"\0")
            with package.open(entry) as stream:
                while chunk := stream.read(1024 * 1024):
                    digest.update(chunk)
            source[path] = digest.hexdigest()
    rows = []
    for path, (_, kind, oid) in sorted(head.items()):
        if kind != "blob":
            continue
        original = source.get(path)
        state = "identical" if original == oid else "changed" if original else "not_in_archive_at_same_path"
        rows.append({"path": path, "categories": categories(path), "status": state,
                     "source_blob": original, "head_blob": oid,
                     "baseline_matches_source": original is not None and
                     baseline.get(path, (None, None, None))[2] == original})
    return {"archive_sha256": archive_hash, "archive_bytes": archive.stat().st_size,
            "archive_files": len(source),
            "scope": "Exact case-sensitive paths and Git blob hashes at HEAD; absent paths may be renamed",
            "counts": dict(sorted(Counter(row["status"] for row in rows).items())),
            "files": rows}


def audit(root):
    baseline = tree(root, BASELINE)
    head = tree(root, "HEAD")
    commits = git(root, "rev-list", "--all").decode().splitlines()
    history = defaultdict(set)
    blob_paths = defaultdict(set)
    special = []
    for commit in commits:
        for path, (mode, kind, oid) in tree(root, commit).items():
            history[path].add(oid)
            if kind == "blob":
                blob_paths[oid].add(path)
            if kind != "blob" or mode == "120000":
                special.append({"commit": commit, "path": path, "mode": mode, "kind": kind})

    tracked = [p.decode("utf-8", "surrogateescape") for p in
               git(root, "ls-files", "-z").split(b"\0") if p]
    current = []
    findings = []
    for path in sorted(set(tracked)):
        target = root / path
        # Do not follow a tracked symlink into files outside the repository.
        if target.is_symlink() or not target.is_file():
            current.append({"path": path, "categories": ["symlink_or_missing"]})
            continue
        data = target.read_bytes()
        labels = categories(path)
        if labels:
            current.append({"path": path, "categories": labels, "bytes": len(data),
                            "sha256": hashlib.sha256(data).hexdigest(),
                            "baseline_blob": baseline.get(path, (None, None, None))[2],
                            "head_blob": head.get(path, (None, None, None))[2],
                            "working_git_blob": hashlib.sha1(b"blob " + str(len(data)).encode() + b"\0" + data).hexdigest()})
        for kind in credential_kinds(data):
            findings.append({"scope": "working_tree", "path": path, "kind": kind})

    # Stream one blob at a time; no size-based omissions and no secret output.
    with subprocess.Popen(["git", "-C", str(root), "cat-file", "--batch"],
                          stdin=subprocess.PIPE, stdout=subprocess.PIPE) as proc:
        for oid, paths in sorted(blob_paths.items()):
            proc.stdin.write(oid.encode() + b"\n")
            proc.stdin.flush()
            header = proc.stdout.readline().split()
            if len(header) != 3 or header[1] != b"blob":
                raise RuntimeError("Unexpected Git blob response")
            size = int(header[2])
            data = proc.stdout.read(size)
            if len(data) != size or proc.stdout.read(1) != b"\n":
                raise RuntimeError("Incomplete Git blob response")
            for kind in credential_kinds(data):
                findings.append({"scope": "history", "blob": oid,
                                 "paths": sorted(paths), "kind": kind})
        proc.stdin.close()
        if proc.wait() != 0:
            raise RuntimeError("Git blob reader failed")

    historical = [{"path": path, "categories": categories(path),
                   "blobs": sorted(oids), "present_in_head": path in head}
                  for path, oids in sorted(history.items()) if categories(path)]
    return {
        "status": "review_required",
        "scope": "Local refs and tracked working files; not publication approval or comprehensive secret detection",
        "head": git(root, "rev-parse", "HEAD").decode().strip(),
        "baseline": BASELINE,
        "local_refs": git(root, "for-each-ref", "--format=%(refname) %(objectname)").decode().splitlines(),
        "commit_count": len(commits), "tracked_files": len(set(tracked)),
        "historical_blob_count": len(blob_paths),
        "current_category_counts": dict(sorted(Counter(c for row in current for c in row["categories"]).items())),
        "current_candidates": current, "historical_candidates": historical,
        "special_git_entries": special, "credential_pattern_findings": findings,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", required=True, type=Path,
                        help="Private JSON report outside the repository")
    parser.add_argument("--source-archive", type=Path,
                        help="Lawfully supplied original Conquest source ZIP; compare without extracting")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    destination = args.output.resolve()
    if destination == root or root in destination.parents:
        parser.error("Keep the audit report outside the repository")
    report = audit(root)
    if args.source_archive:
        report["source_archive_comparison"] = compare_source_archive(root, args.source_archive)
    # Refuse to replace existing reports or follow existing output symlinks.
    with destination.open("x", encoding="utf-8") as stream:
        json.dump(report, stream, indent=2, ensure_ascii=True)
        stream.write("\n")
    print(json.dumps({k: report[k] for k in ("status", "head", "commit_count", "tracked_files",
                                            "historical_blob_count", "current_category_counts")}))
    print(f"Credential-pattern findings: {len(report['credential_pattern_findings'])}; report: {destination}")
    return 2  # An inventory never constitutes permission to publish.


if __name__ == "__main__":
    raise SystemExit(main())
