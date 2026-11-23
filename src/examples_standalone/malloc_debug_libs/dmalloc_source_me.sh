# shellcheck shell=bash
dmalloc_settings="$(dmalloc -l logfile.out -i 100 high)"
eval "${dmalloc_settings}"
: "${DMALLOC_OPTIONS:?dmalloc did not set DMALLOC_OPTIONS}"
echo "DMALLOC_OPTIONS is now set to ${DMALLOC_OPTIONS}"
