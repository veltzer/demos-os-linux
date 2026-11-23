# shellcheck shell=bash
eval "$(dmalloc -l logfile.out -i 100 high)"
: "${DMALLOC_OPTIONS:?dmalloc did not set DMALLOC_OPTIONS}"
echo "DMALLOC_OPTIONS is now set to ${DMALLOC_OPTIONS}"
