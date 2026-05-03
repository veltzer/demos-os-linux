#!/usr/bin/env python

"""
Sync (or check) terms.ambiguous/ and terms.unambiguous/ from the shared
veltzer/terms repository into this project. The source of truth is
github.com/veltzer/terms.

Usage:
    sync_terms.py --check    # exit non-zero if local copies diverge from upstream
    sync_terms.py --sync     # overwrite local copies with upstream
"""

import argparse
import filecmp
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

DEFAULT_REPO_URL = "git@github.com:veltzer/terms.git"
DEFAULT_REF = "main"
SYNCED_DIRS = ("terms.ambiguous", "terms.unambiguous")


def fetch_upstream(tmp: Path) -> Path:
    repo_url = os.environ.get("TERMS_REPO_URL", DEFAULT_REPO_URL)
    ref = os.environ.get("TERMS_REF", DEFAULT_REF)
    print(f"Fetching {repo_url} ({ref})...")
    target = tmp / "terms"
    subprocess.run(
        ["git", "clone", "--depth", "1", "--branch", ref, repo_url, str(target)],
        check=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return target


def diff_dirs(src: Path, dst: Path) -> tuple[list[str], list[str], list[str]]:
    """Return (only_in_upstream, only_in_local, differing) relative paths."""
    only_upstream: list[str] = []
    only_local: list[str] = []
    differing: list[str] = []

    cmp = filecmp.dircmp(src, dst)

    def walk(c: filecmp.dircmp, prefix: str) -> None:
        for name in c.left_only:
            only_upstream.append(f"{prefix}{name}")
        for name in c.right_only:
            only_local.append(f"{prefix}{name}")
        for name in c.diff_files:
            differing.append(f"{prefix}{name}")
        for sub_name, sub in c.subdirs.items():
            walk(sub, f"{prefix}{sub_name}/")

    walk(cmp, "")
    return only_upstream, only_local, differing


def cmd_check(project_root: Path) -> int:
    with tempfile.TemporaryDirectory() as tmp:
        upstream = fetch_upstream(Path(tmp))
        in_sync = True
        for name in SYNCED_DIRS:
            src = upstream / name
            if not src.is_dir():
                print(f"ERROR: {name} not found in upstream repo", file=sys.stderr)
                return 1
            dst = project_root / name
            if not dst.is_dir():
                print(f"  {name}: missing locally")
                in_sync = False
                continue
            only_up, only_loc, diff = diff_dirs(src, dst)
            if not (only_up or only_loc or diff):
                print(f"  {name}: in sync")
                continue
            in_sync = False
            print(f"  {name}: out of sync")
            for f in only_up:
                print(f"    + {f} (only in upstream)")
            for f in only_loc:
                print(f"    - {f} (only locally)")
            for f in diff:
                print(f"    ~ {f} (differs)")
        if in_sync:
            print("OK: all in sync.")
            return 0
        print("FAIL: differences found. Run with --sync to overwrite local copies.", file=sys.stderr)
        return 1


def cmd_sync(project_root: Path) -> int:
    with tempfile.TemporaryDirectory() as tmp:
        upstream = fetch_upstream(Path(tmp))
        for name in SYNCED_DIRS:
            src = upstream / name
            if not src.is_dir():
                print(f"ERROR: {name} not found in upstream repo", file=sys.stderr)
                return 1
            dst = project_root / name
            if dst.exists():
                shutil.rmtree(dst)
            shutil.copytree(src, dst)
            file_count = sum(1 for _ in dst.rglob("*") if _.is_file())
            print(f"  synced {name} ({file_count} files)")
        print("Done.")
        return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.strip().splitlines()[0])
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--check", action="store_true", help="check whether local copies match upstream (exit 1 on divergence)")
    group.add_argument("--sync", action="store_true", help="overwrite local copies with upstream")
    args = parser.parse_args()

    project_root = Path(__file__).resolve().parent.parent
    if args.check:
        return cmd_check(project_root)
    return cmd_sync(project_root)


if __name__ == "__main__":
    sys.exit(main())
