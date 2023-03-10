#!/bin/bash

# This script checks that your machine has whats needs for qemu, kvm and virtualization
vx=$(egrep -c '(vmx|svm)' /proc/cpuinfo)
if [ "$vx" -eq 0 ]
then
	echo "You don't have virtualization enabled"
	exit 1
fi
if ! kvm-ok > /dev/null
then
	echo "kvm is not enabled"
	exit 1
fi
if ! systemctl is-active --quiet libvirtd
then
	echo "libvirtd is not active"
	exit 1
fi
