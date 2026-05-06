#!/usr/bin/env python
"""List the CI container image's published versions on GHCR, grouped by push.

Background: a single `docker push` of a tagged image creates several rows
in GHCR's "versions" list — one for the manifest list (what carries the
tag), plus one or more untagged sub-manifests (per-arch image, provenance
attestation). They all share a created_at within a few seconds.

This script clusters versions by time, labels each cluster as a "release",
and tells you which one is currently live (the cluster whose manifest list
holds a tag).
"""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone

OWNER = "veltzer"
PACKAGE = "demos-os-linux-ci"
# Versions created within this many seconds of each other are treated as
# part of the same `docker push`.
CLUSTER_WINDOW_SECONDS = 60


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
            "If this is a 403, run: gh auth refresh -h github.com -s read:packages"
        )
    return json.loads(proc.stdout)


def parse_ts(s: str) -> datetime:
    return datetime.fromisoformat(s.replace("Z", "+00:00"))


def cluster(versions: list[dict]) -> list[list[dict]]:
    """Group versions into clusters that look like a single push."""
    versions_sorted = sorted(versions, key=lambda v: parse_ts(v["created_at"]))
    groups: list[list[dict]] = []
    for v in versions_sorted:
        ts = parse_ts(v["created_at"])
        if groups and (ts - parse_ts(groups[-1][-1]["created_at"])).total_seconds() <= CLUSTER_WINDOW_SECONDS:
            groups[-1].append(v)
        else:
            groups.append([v])
    groups.reverse()  # newest cluster first
    return groups


def short_digest(name: str) -> str:
    return name.split(":", 1)[1][:12] if ":" in name else name[:12]


def fmt_age(ts: datetime, now: datetime) -> str:
    delta = now - ts
    secs = int(delta.total_seconds())
    if secs < 60:
        return f"{secs}s ago"
    if secs < 3600:
        return f"{secs // 60}m ago"
    if secs < 86400:
        return f"{secs // 3600}h ago"
    return f"{secs // 86400}d ago"


def main() -> None:
    versions = fetch_versions()
    if not versions:
        print(f"No published versions of ghcr.io/{OWNER}/{PACKAGE}.")
        return

    clusters = cluster(versions)
    now = datetime.now(timezone.utc)

    rows: list[tuple[str, str, str, str]] = []
    for idx, group in enumerate(clusters):
        tagged = [v for v in group if v["metadata"]["container"]["tags"]]
        oldest = min(parse_ts(v["created_at"]) for v in group)
        when = f"{oldest:%Y-%m-%d %H:%M}"
        age = fmt_age(oldest, now)
        if tagged:
            tags = ",".join(sorted({t for v in tagged for t in v["metadata"]["container"]["tags"]}))
            status = "LIVE" if idx == 0 else "tagged"
            rows.append((when, age, tags, status))
        else:
            rows.append((when, age, "-", "orphaned"))

    widths = [max(len(r[i]) for r in rows) for i in range(4)]
    for r in rows:
        print(f"{r[0]:<{widths[0]}}  {r[1]:>{widths[1]}}  {r[2]:<{widths[2]}}  {r[3]}")


if __name__ == "__main__":
    main()
