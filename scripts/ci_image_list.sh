#!/bin/bash -eu

# List published versions of the CI container image
# (ghcr.io/veltzer/demos-os-linux-ci) with their tags and creation times.

owner="veltzer"
package="demos-os-linux-ci"

gh api \
	--paginate \
	-H "Accept: application/vnd.github+json" \
	"/users/${owner}/packages/container/${package}/versions" \
	--jq '.[] | {created: .created_at, tags: (.metadata.container.tags | join(",")), digest: .name}' \
	| jq -r '"\(.created)  \(.digest[0:19])  \(.tags)"' \
	| sort -r
