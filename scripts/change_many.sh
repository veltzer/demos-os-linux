#!/bin/bash -e

find src -name "*.c" -or -name "*.cc" | while IFS= read -r x
do
	# echo "doing [$x]"
	if grep -v main "${x}" | grep argv
	then
		continue
	fi
	if grep -v main "${x}" | grep argc
	then
		continue
	fi
	echo "${x}"
	sed -i 's/^int main(int argc, char\*\* argv, char\*\* envp) {$/int main() {/' "${x}"
done
