#!/usr/bin/env python

"""
Sync terms.ambiguous/ and terms.single_meaning/ from the shared
veltzer/terms repository into this project. Both directories are
gitignored locally; the source of truth is github.com/veltzer/terms.

Run this after cloning, and any time the shared registry changes.
"""

import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

DEFAULT_REPO_URL = "git@github.com:veltzer/terms.git"
DEFAULT_REF = "main"
SYNCED_DIRS = ("terms.ambiguous", "terms.unambiguous")


def main() -> int:
    repo_url = os.environ.get("TERMS_REPO_URL", DEFAULT_REPO_URL)
    ref = os.environ.get("TERMS_REF", DEFAULT_REF)
    project_root = Path(__file__).resolve().parent.parent

    print(f"Fetching {repo_url} ({ref})...")

    with tempfile.TemporaryDirectory() as tmp:
        clone_target = Path(tmp) / "terms"
        subprocess.run(
            ["git", "clone", "--depth", "1", "--branch", ref, repo_url, str(clone_target)],
            check=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )

        for name in SYNCED_DIRS:
            src = clone_target / name
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


if __name__ == "__main__":
    sys.exit(main())
