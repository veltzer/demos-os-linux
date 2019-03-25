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
#include <stdio.h>
#include "crash.h"

/*
* OK, we want a way to call the register function (not to mention load the library)
* without messing with source files and re-compiling. Here how it works:
*
* We create a library with a single init function that calls the registration for us
* using a function constructor hook and we'll force the dynamic linker to load it
* in run time using LD_PRELOAD magic.
*
* Pretty, clever, isn't it? :-)
*/

void __attribute__ ((constructor)) debug_auto_init(void);

/* Buffer for assert info that we need to supply */
static unsigned char buf[128];

void debug_auto_init(void) {
	/* Print some meaningfull message so they'll know that we're running */
	fprintf(stderr, "\n*** libcrash automagically loaded. Registering...");
	/* Blast us if we know what's the process name. We just use [AUTOMAGIC] */
	if(register_crash_handler("[AUTOMAGIC]", buf))
		fprintf(stderr, "Failed!\n");
	else
		fprintf(stderr, "Success.\n");
	return;
}

/* THE END */
