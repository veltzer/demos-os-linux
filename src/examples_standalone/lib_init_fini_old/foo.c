/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h> // for fprintf(3)

extern char *build_id;
extern void _init(void);
extern void _fini(void);

void my_init(void) {
	// call the original...
	_init();
	fprintf(stderr, "I'm in my_init %s\n", __FILE__);
	// how do I get the build id from my own object ? This does not work
	//extern char* build;
	//fprintf(stderr, ".note.gnu.build-id is %s", build);
}


void my_fini(void) {
	fprintf(stderr, "I'm in my_fini %s\n", __FILE__);
	// call the original...
	_fini();
}

int foo(int a, int b) {
	return a+b;
}
