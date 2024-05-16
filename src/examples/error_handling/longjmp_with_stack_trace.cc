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
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <execinfo.h>
#include <string.h>
#include <demangle_utils.hh>	// for error_demangle()

/*
 * This is an example of using setjmp/longjmp to simulate exception handling
 * in C code. This example expands on the previous example in that it adds stack
 * traces to the "errors" which are generated.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-clobbered
 */

const int max_stack_frames=25;
const int drop_frames_start=3;
const int drop_frames_end=1;
const int max_message_size=256;

struct error_data {
	int size;
	void *array[max_stack_frames];
	char **symbols;
	char message[max_message_size];
};
// The last error reported
static error_data *last_error;
// This buffer saves the context of the jump. We need one for each unique
// point we wish to jump to
static jmp_buf env;

inline void error_create(const char *message) {
	error_data *p=(error_data *)malloc(sizeof(error_data));

	p->size=backtrace(p->array, max_stack_frames);
	p->symbols=backtrace_symbols(p->array, p->size);
	strncpy(p->message, message, max_message_size-1);
	last_error=p;
	longjmp(env, (unsigned long)p);
}

inline void error_print(FILE *f, error_data *p) {
	fprintf(f, "error message is [%s]\n", p->message);
	for(int i=p->size-drop_frames_start; i>=drop_frames_end; i--) {
		char *symbol=p->symbols[i];
		char result_name[256];
		char result_offset[256];
		error_demangle(symbol, result_name, 256, result_offset, 256);
		fprintf(f, "%s,%s,%s\n", symbol, result_name, result_offset);
	}
}

inline void error_print_last(FILE *f) {
	error_print(f, last_error);
}

inline void error_free(error_data *p) {
	free(p->symbols);
	free(p);
}

inline void error_free_last() {
	error_free(last_error);
}

// This function **must** be inlined as if setjmp returns then env will no longer
// be valid. That's why we don't use it (there is no way to guarantee inlining).
inline error_data *error_setjmp() {
	unsigned long ret=setjmp(env);
	if (!ret) {
		return NULL;
	} else {
		return (error_data *)ret;
	}
}

// here is another idea: why not use a macro instead:
#define mac_error_setjmp() ((error_data *)(unsigned long)setjmp(env))

/* this simulates a function which sometimes encounters errors */
void func(int counter) {
	if (counter % 3==0) {
		error_create("some error");
	}
	counter++;
	fprintf(stderr, "this is the continuation of the function\n");
}

void myfunc(int c) {
	func(c);
}

int main(int argc, char** argv, char** envp) {
	for(int c=1; c<10; c++) {
		error_data *p=mac_error_setjmp();
		if (p==NULL) {
			// This is the regular code. We get here when setting doing the
			// setjmp for the first time
			fprintf(stderr, "c is %d\n", c);
			myfunc(c);
		} else {
			// we got an error
			error_print(stderr, p);
			error_free(p);
		}
	}
	return EXIT_SUCCESS;
}
