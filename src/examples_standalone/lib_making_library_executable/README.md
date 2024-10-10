This is a demo of how to make a library executable.

If you want to see an example of such an idea just run your own libc library:
=============================================================================
fermat$ /lib/libc.so.6
GNU C Library (Ubuntu EGLIBC 2.12.1-0ubuntu9) stable release version 2.12.1, by Roland McGrath et al.
Copyright (C) 2010 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.
Compiled by GNU CC version 4.4.5.
Compiled on a Linux 2.6.35 system on 2010-11-09.
Available extensions:
	crypt add-on version 2.1 by Michael Glad and others
	GNU Libidn by Simon Josefsson
	Native POSIX Threads Library by Ulrich Drepper et al
	BIND-8.2.3-T5B
libc ABIs: UNIQUE IFUNC
For bug reporting instructions, please see:
<http://www.debian.org/Bugs/>.
fermat$

The ideas for this come from:
http://gcc.gnu.org/ml/gcc-help/2003-07/msg00232.html
and the sources of the glibc library.
