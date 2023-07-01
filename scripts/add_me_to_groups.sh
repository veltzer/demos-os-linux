#!/bin/bash -e

# This script adds the current user to all the groups that you need to be
# a member of in order to run all the demos in this repository.

for group in adm cdrom sudo dip plugdev kvm lpadmin lxd sambashare docker libvirt microk8s stapusr stapdev stapsys
do
	if ! sudo usermod -a -G "${group}" "${USER}"
	then
		echo "failed to add user ${USER} to group ${group}"
	else
		echo "added you to group ${group}"
	fi
done
