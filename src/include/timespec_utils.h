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

#ifndef __timespec_utils_h
#define __timespec_utils_h

/*
 * This file is here to help you deal with the Linux 'struct timespec'
 * It seems that there are no such utility functions in the standard
 * C library
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <time.h>	// for struct timespec, strftime(3), localtime_r(3), gmtime_r(3)
#include <stdio.h>	// for snprintf(3)
#include <err_utils.h>	// for CHECK_ASSERT(), CHECK_NOT_ZERO(), CHECK_NOT_NULL()

/* The number of nsecs per sec. */
const long long NSEC_PER_SEC=1000000000L;
/* The number of msecs per sec. */
const long long MSEC_PER_SEC=1000000;

/*
 * Add a number of nanoseconds to a timespec
 */
static inline void timespec_add_nanos(struct timespec* t, long long nanos_count) {
	long long nsec=t->tv_nsec+nanos_count;
	t->tv_nsec=nsec%NSEC_PER_SEC;
	t->tv_sec+=nsec/NSEC_PER_SEC;
}
/*
 * Add a number of seconds to a timespec
 */
static inline void timespec_add_secs(struct timespec* t, long long secs_count) {
	t->tv_sec+=secs_count;
}

/*
 * Add one timespec to another
 */
static inline void timespec_add(struct timespec* to, struct timespec* from) {
	long long nsec=to->tv_nsec+from->tv_nsec;
	to->tv_nsec=nsec%NSEC_PER_SEC;
	to->tv_sec+=from->tv_sec+nsec/NSEC_PER_SEC;
}

static inline void timespec_assert_ge(struct timespec* x, struct timespec* y) {
	if(x->tv_sec>y->tv_sec) {
		return;
	} else {
		if(x->tv_sec==y->tv_sec) {
			if(x->tv_nsec>=y->tv_nsec) {
				return;
			} else {
				CHECK_ASSERT(0);
			}
		} else {
			CHECK_ASSERT(0);
		}
	}
}

/*
 * Return the diff in nanoseconds between x and y
 */
static inline unsigned long long timespec_diff_nano(struct timespec* t_end, struct timespec* t_start) {
	return (t_end->tv_sec-t_start->tv_sec)*NSEC_PER_SEC+(t_end->tv_nsec-t_start->tv_nsec);
}

/*
 * subtract start from end and put the difference into result.
 */
static inline void timespec_sub(struct timespec *end, struct timespec *start, struct timespec *result) {
	result->tv_sec = end->tv_sec - start->tv_sec;
	result->tv_nsec = end->tv_nsec - start->tv_nsec;
	if (result->tv_nsec < 0) {
		--result->tv_sec;
		result->tv_nsec += NSEC_PER_SEC;
	}
}

static inline void timespec_set_sec(struct timespec* t, long long secs) {
	t->tv_sec=secs;
}

static inline void timespec_set_nsec(struct timespec* t, long long nsec) {
	t->tv_nsec=nsec;
}

static inline void timespec_clear(struct timespec* t) {
	t->tv_sec=0;
	t->tv_nsec=0;
}

static inline void timespec_set(struct timespec* t, long long secs, long long nsecs) {
	t->tv_sec=secs;
	t->tv_nsec=nsecs;
}

static inline void timespec_copy(struct timespec* to, struct timespec* from) {
	to->tv_sec=from->tv_sec;
	to->tv_nsec=from->tv_nsec;
}

static inline int timespec_snprintf(char* str, size_t size, struct timespec* t, int tz) {
	// this is very simple printing
	// return snprintf(str, size, "%ld.%09ld: ", t->tv_sec, t->tv_nsec);
	// this is a more complicated one
	struct tm mytm;
	if(tz) {
		CHECK_NOT_NULL(localtime_r(&t->tv_sec, &mytm));
	} else {
		CHECK_NOT_NULL(gmtime_r(&t->tv_sec, &mytm));
	}
	char mybuf[256];
	const char* format="%Y-%m-%d %H:%M:%S";
	CHECK_NOT_ZERO(strftime(mybuf, sizeof(mybuf), format, &mytm));
	return snprintf(str, size, "%s.%09ld", mybuf, t->tv_nsec);
}

static inline unsigned long long timespec_to_nanos(const struct timespec* t) {
	return t->tv_sec*NSEC_PER_SEC+t->tv_nsec;
}

static inline unsigned long long timespec_to_micros(const struct timespec* t) {
	return t->tv_sec*MSEC_PER_SEC+t->tv_nsec/1000;
}

#endif	/* !__timespec_utils_h */
