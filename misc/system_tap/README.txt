This is a directory exploring system tap.

How to install systemtap on ubuntu
==================================
first:
	apt-get install systemtap

then:
	one way:
		wget http://ddebs.ubuntu.com/pool/main/l/linux/linux-image-3.0.0-16-generic-dbgsym_3.0.0-16.29_i386.ddeb
		install it using dpkg --install.

second way (better):
	cat > /etc/apt/sources.list.d/ddebs.list << EOF
		deb http://ddebs.ubuntu.com/ quantal main restricted universe multiverse
		deb http://ddebs.ubuntu.com/ quantal-updates main restricted universe multiverse
	EOF
	apt-key adv --keyserver keyserver.ubuntu.com --recv-keys ECDCAD72428D7C01
	apt-get update
	apt-get install linux-image-$(uname -r)-dbgsym

	* remark - the current ddebs archive DOES NOT carry a dbgsym kernel package for the -lowlatency
	kernels. You will have to use a -generic kernel to use systemtap.

How to run the scripts
======================
- The scripts here should be run by a member of the 'stapusr' group.
- Just as yourself to the three groups: 'stapusr', 'stapsys', 'stapdep'
- They seem to fail in some way when they are not.
- It does not suffice to add the user to the various '*stap*' groups. Here are my groups and sudo
is still required:
	abel$ groups
	mark : mark adm dialout cdrom plugdev lpadmin admin sambashare stapdev stapusr stapsys
- you also need to run have the service 'stap-server' running.
	It cannot be run as root for security reasons.
	So it has to be run as user: ???

Finding scripts to run
======================
It seems that systemtap has undergone lots of source level changes recently and some of the scripts
in this folder no longer work.
Install the 'systemtap-doc' package and find scripts to run in:
/usr/share/doc/systemtap-doc/examples/*

References:
http://www.ibm.com/developerworks/linux/library/l-systemtap/index.html
http://sourceware.org/systemtap/wiki/SystemtapOnUbuntu
