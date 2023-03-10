#!/bin/bash

# This script checks that your machine has whats needs for qemu, kvm and virtualization
#
# References:
# - https://www.tecmint.com/install-qemu-kvm-ubuntu-create-virtual-machines/

vx=$(egrep -c '(vmx|svm)' /proc/cpuinfo)
if [ "$vx" -eq 0 ]
then
	echo "You don't have virtualization enabled"
	echo "Your cpu does not support virtualization"
	echo "There is nothing you can do about that except buy a better CPU"
	exit 1
fi
if ! kvm-ok > /dev/null
then
	echo "kvm is not enabled"
	echo "You need a kernel with KVM enabled."
	echo "In ubuntu it usually is so this is kind of strage..."
	exit 1
fi
if ! systemctl is-active --quiet libvirtd
then
	echo "libvirtd is not active"
	echo "do"
	echo "sudo systemctl enable --now libvirtd"
	echo "sudo systemctl start libvirtd"
	exit 1
fi
if ! getent group kvm | grep -q "\b${USER}\b"
then
	echo "you are not a member of the 'kvm' group"
	echo "try"
	echo "sudo usermod -aG kvm $$USER"
	echo "and re-login..."
	exit 1
fi
if ! getent group libvirt | grep -q "\b${USER}\b"
then
	echo "you are not a member of the 'libvirt' group"
	echo "try"
	echo "sudo usermod -aG libvirt $$USER"
	echo "and re-login..."
	exit 1
fi
