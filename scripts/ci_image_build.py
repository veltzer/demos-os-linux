#!/usr/bin/env python
"""Trigger the GitHub Actions `image` workflow and stream its progress.

Builds and pushes ghcr.io/veltzer/demos-os-linux-ci:latest.
"""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import time

WORKFLOW = "image.yml"


def run(args: list[str], **kw) -> subprocess.CompletedProcess:
    return subprocess.run(args, check=False, text=True, **kw)


def main() -> None:
    if not shutil.which("gh"):
        sys.exit("gh CLI not found on PATH")

    proc = run(["gh", "workflow", "run", WORKFLOW])
    if proc.returncode != 0:
        sys.exit(f"failed to trigger {WORKFLOW}")

    print(f"Triggered {WORKFLOW}. Locating run...")
    # The run takes a moment to register after `workflow run`.
    for _ in range(10):
        time.sleep(2)
        proc = run(
            ["gh", "run", "list", "--workflow", WORKFLOW, "--limit", "1", "--json", "databaseId,status"],
            capture_output=True,
        )
        if proc.returncode != 0:
            continue
        runs = json.loads(proc.stdout or "[]")
        if runs and runs[0]["status"] in ("queued", "in_progress"):
            run_id = runs[0]["databaseId"]
            break
    else:
        sys.exit("could not find a queued/in-progress run after triggering")

    print(f"Watching run {run_id}...")
    proc = run(["gh", "run", "watch", str(run_id), "--exit-status"])
    sys.exit(proc.returncode)


if __name__ == "__main__":
    main()
