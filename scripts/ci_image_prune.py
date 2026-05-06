#!/usr/bin/env python
"""Delete orphaned versions of the CI container image on GHCR.

A push is "orphaned" when none of its version rows are tagged anymore
(its tag was moved by a later push). Nothing pulls these rows; deleting
them tidies the version list without affecting what `:latest` resolves
to.

By default this is a dry run. Pass --yes to actually delete.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone

OWNER = "veltzer"
PACKAGE = "demos-os-linux-ci"
CLUSTER_WINDOW_SECONDS = 60


def parse_ts(s: str) -> datetime:
    return datetime.fromisoformat(s.replace("Z", "+00:00"))


def fetch_versions() -> list[dict]:
    if not shutil.which("gh"):
        sys.exit("gh CLI not found on PATH")
    proc = subprocess.run(
        [
            "gh", "api", "--paginate",
            "-H", "Accept: application/vnd.github+json",
            f"/users/{OWNER}/packages/container/{PACKAGE}/versions",
        ],
        capture_output=True, text=True, check=False,
    )
    if proc.returncode != 0:
        sys.exit(
            f"gh api failed (exit {proc.returncode}):\n{proc.stderr.strip()}\n"
            "If this is a 403, run: gh auth refresh -h github.com -s read:packages,delete:packages"
        )
    return json.loads(proc.stdout)


def cluster(versions: list[dict]) -> list[list[dict]]:
    versions_sorted = sorted(versions, key=lambda v: parse_ts(v["created_at"]))
    groups: list[list[dict]] = []
    for v in versions_sorted:
        ts = parse_ts(v["created_at"])
        if groups and (ts - parse_ts(groups[-1][-1]["created_at"])).total_seconds() <= CLUSTER_WINDOW_SECONDS:
            groups[-1].append(v)
        else:
            groups.append([v])
    return groups


def delete_version(version_id: int) -> bool:
    proc = subprocess.run(
        [
            "gh", "api", "--method", "DELETE",
            "-H", "Accept: application/vnd.github+json",
            f"/users/{OWNER}/packages/container/{PACKAGE}/versions/{version_id}",
        ],
        capture_output=True, text=True, check=False,
    )
    if proc.returncode != 0:
        print(f"  FAILED to delete {version_id}: {proc.stderr.strip()}", file=sys.stderr)
        return False
    return True


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--yes", action="store_true", help="actually delete (default: dry run)")
    args = parser.parse_args()

    versions = fetch_versions()
    if not versions:
        print(f"No published versions of ghcr.io/{OWNER}/{PACKAGE}.")
        return

    clusters = cluster(versions)
    orphaned_clusters = [c for c in clusters if not any(v["metadata"]["container"]["tags"] for v in c)]

    if not orphaned_clusters:
        print("Nothing to prune. All pushes have at least one tagged row.")
        return

    targets: list[dict] = [v for c in orphaned_clusters for v in c]
    verb = "Deleting" if args.yes else "Would delete"
    print(f"{verb} {len(targets)} version row(s) across {len(orphaned_clusters)} orphaned push(es):")
    now = datetime.now(timezone.utc)
    for v in targets:
        ts = parse_ts(v["created_at"])
        age_s = int((now - ts).total_seconds())
        age = f"{age_s // 86400}d" if age_s >= 86400 else f"{age_s // 3600}h"
        digest = v["name"].split(":", 1)[1][:12] if ":" in v["name"] else v["name"][:12]
        print(f"  {ts:%Y-%m-%d %H:%M}  {age:>4} ago  {digest}")

    if not args.yes:
        print()
        print("Dry run. Re-run with --yes to actually delete.")
        return

    failures = sum(1 for v in targets if not delete_version(v["id"]))
    print()
    if failures:
        sys.exit(f"{failures} deletion(s) failed.")
    print(f"Deleted {len(targets)} version row(s).")


if __name__ == "__main__":
    main()
