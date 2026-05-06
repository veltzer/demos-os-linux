#!/bin/bash -eu

# Trigger the GitHub Actions "image" workflow that builds and pushes the
# CI container image (ghcr.io/veltzer/demos-os-linux-ci:latest) used by
# the build workflow.

gh workflow run image.yml
echo "Triggered. Streaming the most recent run:"
sleep 3
run_id=$(gh run list --workflow=image.yml --limit 1 --json databaseId --jq '.[0].databaseId')
gh run watch "${run_id}"
