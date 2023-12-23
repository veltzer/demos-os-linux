#!/bin/bash -e

# the next line is to make the globbing work right
shopt -s globstar extglob
# for x in $(find syllabi -type f -and -name "*.md")
for x in exercises/**/*.md
do
	aspell --conf-dir=. --conf=.aspell.conf check "${x}"
done
