#!/bin/bash
(( c=0 ))
while true
do
	echo "Hello, World! $c"
	sleep 1
	(( c+=1 ))
done
