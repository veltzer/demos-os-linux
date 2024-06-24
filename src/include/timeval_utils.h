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
 * This file is here to help you deal with the Linux 'struct timeval'
 * It seems that there are no such utility functions in the standard
 * C library
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <time.h>	// for struct timeval
#include <stdio.h>	// for fprintf(3), stderr, printf(3)
#include <stdlib.h>	// for exit(3), EXIT_FAILURE

/*
 * An easy function to return how many micros have passed between
 * two timeval structures.
 */
static inline double micro_diff(struct timeval* t1, struct timeval* t2) {
	unsigned long long u1, u2;
	u1=((unsigned long long)t1->tv_sec*1000000)+t1->tv_usec;
	u2=((unsigned long long)t2->tv_sec*1000000)+t2->tv_usec;
	double diff=u2-u1;
	return diff;
}

static inline unsigned long diff_timeval_in_micro(struct timeval *tv1, struct timeval *tv2) {
	if (tv1->tv_sec > tv2->tv_sec) {
		fprintf(stderr, "tv1>tv2\n");
		exit(EXIT_FAILURE);
	} else {
		if (tv1->tv_sec==tv2->tv_sec) {
			if (tv1->tv_usec > tv2->tv_usec) {
				fprintf(stderr, "tv1>tv2\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	unsigned long diff=(tv2->tv_sec - tv1->tv_sec) * 1000;
	diff+=(tv2->tv_usec - tv1->tv_usec) / 1000;
	return diff;
}

static inline void print_timeval(struct timeval *tv, const char *name) {
	printf("%s - sec is %lu\n", name, tv->tv_sec);
	printf("%s - usec is %lu\n", name, tv->tv_usec);
}
