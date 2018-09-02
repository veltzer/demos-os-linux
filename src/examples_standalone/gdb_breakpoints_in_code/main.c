/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <signal.h>	// for raise(2), SIGTRAP

/*
 * This is a simple example that shows how to raise a debugger breakpoint.
 * Even one which is not set in the debugger.
 */

int main(int argc,char** argv,char** envp) {
	volatile int i=5;
	asm("int $3");
	i=7;
	raise(SIGTRAP);
	i=9;
	return EXIT_SUCCESS;
}
