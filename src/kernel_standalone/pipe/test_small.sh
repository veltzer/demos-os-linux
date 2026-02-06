#!/bin/bash -eu

# this is a test for the pipe device

cat /dev/pipe0 &
((x=0))
while [[ "${x}" -lt 100 ]]
do
	cat ./*.c > /dev/pipe0
	((x=x+1))
done
