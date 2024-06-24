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

#pragma once

/*
 * This is a helper file for doing performance measurements using
 * the gettimeofday(2) system call.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <sys/time.h>	// for gettimeofday(2), struct timeval
#include <timeval_utils.h>	// for micro_diff()

typedef struct _measure{
	struct timeval t1;
	struct timeval t2;
	int attempts;
	const char* name;
} measure;

static inline void measure_init(measure* m, const char* name, int attempts) {
	m->name=name;
	m->attempts=attempts;
}

static inline void measure_start(measure* m) {
	// printf("measure start: [%s]\n", msg);
	gettimeofday(&m->t1, NULL);
}

static inline void measure_end(measure* m) {
	gettimeofday(&m->t2, NULL);
	// printf("measure end: [%s]\n", msg);
}

static inline void measure_print(measure* m) {
	printf("measure print: time in micro of single [%s]: %lf\n", m->name, micro_diff(&m->t1, &m->t2)/(double)(m->attempts));
}

static inline double measure_micro_diff(measure* m) {
	return micro_diff(&m->t1, &m->t2);
}
