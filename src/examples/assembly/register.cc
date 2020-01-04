/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <lowlevel_utils.h> // for getrdtsc(), getstackpointer(), getframepointer()

/*
 * This is an example of getting a register
 */

static inline unsigned long getbx() {
	unsigned long val;
	#if __i386__
	asm ("movl %%ebx, %0" : "=r" (val));
	#endif	// __i386__
	#if __x86_64__
	val=5;
	#endif	// __x86_64__
	return val;
}

int main(int argc, char** argv, char** envp) {
	printf("ebx is %lu\n", getbx());
	return EXIT_SUCCESS;
}
