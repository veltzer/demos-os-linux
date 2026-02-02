#!/bin/bash -eu
dbus-send --dest="org.freedesktop.ExampleName" \
	"/org/freedesktop/sample/object/name" \
	"org.freedesktop.ExampleInterface.ExampleMethod" \
	"int32:47" "string:'hello world'" "double:65.32" \
	"array:string:'1st item','next item','last item'" \
	"dict:string:int32:'one',1,'two',2,'three',3" \
	"variant:int32:8" \
	"objpath:/org/freedesktop/sample/object/name"
