/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <execinfo.h>	// for backtrace(3), backtrace_symbols(3), backtrace_symbols_fd(3)
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_NULL()
#include <signal_utils.h>	// for signal_register_handler_signal()

/*
 * EXTRA_COMPILE_FLAGS=-g3
 */

/*
 * Please keep this a C function, as it should be usable for C infrastructure as well...
 */
void print_trace(bool full) {
	unsigned int nptrs;
	const unsigned int MAX_FRAMES=100;
	void* buffer[MAX_FRAMES];
	nptrs=backtrace(buffer, MAX_FRAMES);
	if(!full) {
		nptrs-=2;
	}
	fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);
	backtrace_symbols_fd(buffer, nptrs, fileno(stderr));
}

int main() {
	print_trace(true);
	return EXIT_SUCCESS;
}
