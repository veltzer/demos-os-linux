#!/bin/bash -eu

while IFS= read -r x
do
	# echo "doing [$x]"
	if awk '!/main/ && /argv/ {print; found=1} END {exit !found}' "${x}"
	then
		continue
	fi
	if awk '!/main/ && /argc/ {print; found=1} END {exit !found}' "${x}"
	then
		continue
	fi
	echo "${x}"
	sed -i 's/^int main(int argc, char\*\* argv, char\*\* envp) {$/int main() {/' "${x}"
done < <(find src -name "*.c" -or -name "*.cc" || true)
