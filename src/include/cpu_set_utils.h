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

#pragma once

/*
 * This is a collection of helper functions to help you deal with cpu
 * sets.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <sched.h>	// for CPU_COUNT(3), CPU_SETSIZE, CPU_ISSET(3)
#include <trace_utils.h>// for INFO()

/*
 * A function to print cpu sets
 */
static inline void cpu_set_print(cpu_set_t *p) {
	int j;
	INFO("CPU_SETSIZE is %d", CPU_SETSIZE);
	INFO("CPU_COUNT is %d", CPU_COUNT(p));
	for(j=0; j<CPU_SETSIZE; j++) {
		if (CPU_ISSET(j, p)) {
			INFO("\tCPU %d", j);
		}
	}
}

/*
 * A function to print cpu sets to a file
 */
static inline void cpu_set_print_file(FILE* pfile, cpu_set_t *p) {
	int j;
	fprintf(pfile, "CPU_SETSIZE is %d\n", CPU_SETSIZE);
	fprintf(pfile, "CPU_COUNT is %d\n", CPU_COUNT(p));
	for(j=0; j < CPU_SETSIZE; j++) {
		if (CPU_ISSET(j, p)) {
			fprintf(pfile, "\tCPU %d\n", j);
		}
	}
}
