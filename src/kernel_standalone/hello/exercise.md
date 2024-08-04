# First kernel module

Write a simple hello module that does nothing
Write init and exit functions.
Show that you can insert the module into the kernel and remove it from the kernel
Use a sudo user and the commands:
	insmod
	rmmod
	lsmod
	modinfo
use the following commands to view the kernel log:
$ sudo dmesg
or
$ tail /var/log/kern.log

https://github.com/veltzer/demos-linux
the example is in:
	src/kernel_standalone/hello

In order to succeed in insering this module into the kernel you need to either:
1. Sign this module properly (current not supported by the current makefile)
2. Disable the secure boot feature of your system (you can find that in the bios).
