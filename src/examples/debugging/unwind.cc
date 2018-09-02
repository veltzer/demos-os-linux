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
#include <stdio.h>	// for printf(3), fprintf(3), stderr:object
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <libunwind.h>	// for unw_cursor_t:object, unw_context_t:object, unw_getcontext(), unw_init_local(), unw_step(),
// unw_get_reg(), unw_word_t:object, unw_get_proc_name()
#define HAVE_DECL_BASENAME 1
#include <libiberty/demangle.h>	// for cplus_demangle(3)

/*
 * This example uses libunwind to print a stack trace.
 *
 * References:
 * http://publicclu2.blogspot.co.il/2013/05/call-stack-trace-backtrace-generation.html
 *
 * Notes:
 * - the factorial example doesn't work right because GCC is smart enough to understand
 * that this is not real recursion and makes it a loop instead. Fibonacci is enough to
 * confuse the compiler.
 * - as you can see you do not need to compile with debug info for this to work.
 * - you do need to unmangle the names if you are working with C++.
 *
 * EXTRA_LINK_CMDS=pkg-config --libs libunwind-generic
 * EXTRA_LINK_FLAGS=-liberty
 */

void do_backtrace() {
	unw_cursor_t cursor;
	unw_context_t context;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);
	while (unw_step(&cursor) > 0) {
		unw_word_t offset, pc;
		char fname[64];
		unw_get_reg(&cursor, UNW_REG_IP, &pc);
		fname[0] = '\0';
		(void)unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
		char* fname_demangled=cplus_demangle(fname, DMGL_PARAMS);
		if(fname_demangled==NULL) {
			fname_demangled=fname;
		}
		printf ("%p : (%s+%p) [%p]\n", (void*)pc, fname_demangled, (void*)offset, (void*)pc);
	}
}

int factorial(int val) __attribute__((noinline));
int factorial(int val) {
	if(val==0) {
		do_backtrace();
		return 1;
	} else {
		return factorial(val-1)*val;
	}
}

int fibonacci(int val) {
	if(val==0 || val==1) {
		do_backtrace();
		return 1;
	} else {
		return fibonacci(val-1)+fibonacci(val-2);
	}
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [number]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const int val=atoi(argv[1]);
	printf("factorial(%d) is %d\n", val, factorial(val));
	printf("fibonacci(%d) is %d\n", val, fibonacci(val));
	return EXIT_SUCCESS;
}
