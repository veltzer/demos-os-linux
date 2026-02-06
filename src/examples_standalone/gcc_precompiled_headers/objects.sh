#!/bin/bash -eu

((x=0))
while [[ "${x}" -lt 10 ]]
do
	echo -n "main${x}.precompiled.o main${x}.noprecomp.o "
	((x=x+1))
done
