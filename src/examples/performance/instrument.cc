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
#include <unistd.h>	// for sleep(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO()

/*
 * This is a demo of how to use the instrumentation feature of the gnu compiler.
 *
 * Notes:
 * - notice that the flags passed to the compiler ask it not to instrument the instrumenting
 * functions for obvious infinite recursion reasons.
 * - If you, for instance, use your own debug functions inside the the instrumentation
 * functions here and these debug functions are also instrumented then you will get
 * an infinited recursion.
 * - This is not a big problem in practice since the instrumentation subsystem of a large
 * system is usually a separate piece of code which is compiled sepertely, does not call
 * code from the main code and is compiled without the instrumentation flags.
 * - It seems that printf is inlined also and so you have to tell the compiler not to instrument
 * that as well.
 * - Since I do not wish to instrument main then I put an attribute on it not to be
 * instrumented.
 *
 * EXTRA_COMPILE_FLAGS=-finstrument-functions -finstrument-functions-exclude-function-list=printf,sleep
 * OPTION_WITHOUT_FUNCTION_ATTRIBUTES=-finstrument-functions -finstrument-functions-exclude-function-list=__cyg_profile_func_enter,__cyg_profile_func_exit,printf
 *
 * TODO:
 * - show how to get the arguments to the function which is being instrumented. use the
 * reference for that.
 *
 * References:
 * http://linuxgazette.net/151/melinte.html
 */

extern "C" void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
extern "C" void __cyg_profile_func_enter(void *this_fn, void *call_site) {
	printf("entering %p,%p\n", this_fn, call_site);
}
extern "C" void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
extern "C" void __cyg_profile_func_exit(void *this_fn, void *call_site) {
	printf("exiting %p,%p\n", this_fn, call_site);
}

void long_task(void) {
	// CHECK_ZERO(sleep(1));
	sleep(1);
}

int main(int argc, char** argv, char** envp) __attribute__((no_instrument_function));
int main(int argc, char** argv, char** envp) {
	long_task();
	return EXIT_SUCCESS;
}
