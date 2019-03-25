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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for atexit(3), on_exit(3), exit(3)
#include <unistd.h>	// for sysconf(3)
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_M1()

/*
 * This example shows how to use the cleanup framework provided by the standard C library
 * notice that cleanup functions are called in reverse order of their registration
 *
 * Notes:
 * - the same function can be registered multiple times (with atexit(3) or on_exit(3)).
 * - the limit to number of registrations seems to be very high (millions ?!?).
 * - the API to on_exit(3) seems to be a bit better since it allows to pass a pointer
 * and get the exit code which atexit does not allow.
 * - on the other hand the documentation states that atexit(3) is preffered on Linux
 * and some consider on_exit(3) to be deprecated.
 * - calling _exit(2) will end the software immediately and so no handlers will be
 * called.
 *
 * TODO:
 * - show that after forking the child will also do the cleanups.
 * - show what happens if you get a fatal signal.
 * - show what happens to C++ object on the stack when exit(3) is called (do
 * the destructors get called?!?)
 */

void endfunc1() {
	fprintf(stderr, "Hey! I am doing some cleanup work 1\n");
	fprintf(stderr, "I was registered with atexit(3)\n");
}
void endfunc2() {
	fprintf(stderr, "Hey! I am doing some cleanup work 2\n");
	fprintf(stderr, "I was registered with atexit(3)\n");
}
void endfunc3(int exit_code, void* ptr) {
	fprintf(stderr, "Hey! I am doing some cleanup work 3\n");
	fprintf(stderr, "I was registered with on_exit(3)\n");
	fprintf(stderr, "exit_code is %d\n", exit_code);
	fprintf(stderr, "ptr is %p\n", ptr);
}

int main(int argc, char** argv, char** envp) {
	// lets find out how many atexit functions can we register at a maximum...
	long val_atexit_max=CHECK_NOT_M1(sysconf(_SC_ATEXIT_MAX));
	fprintf(stderr, "ATEXIT_MAX is %ld\n", val_atexit_max);
	// lets register some functions...
	CHECK_ZERO(atexit(endfunc1));
	CHECK_ZERO(atexit(endfunc1));
	CHECK_ZERO(atexit(endfunc2));
	CHECK_ZERO(on_exit(endfunc3, NULL));
	CHECK_ZERO(on_exit(endfunc3, NULL));
	// this will end the software immediately
	// _exit(7);
	exit(17);
	return EXIT_SUCCESS;
}
