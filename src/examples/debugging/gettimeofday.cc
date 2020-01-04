/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for fprintf(3), stderr, printf(3)
#include <sys/time.h>	// for gettimeofday(2)
#include <unistd.h>	// for usleep(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <stdlib.h>	// for EXIT_FAILURE, EXIT_SUCCESS, exit(3)

/*
 * This example shows how to use gettimeofday
 * This example also shows how accurate this method of measurement is...
 */

void print_timeval(struct timeval *tv, const char *name) {
	printf("%s - sec is %lu\n", name, tv->tv_sec);
	printf("%s - usec is %lu\n", name, tv->tv_usec);
}

unsigned long diff_timeval_in_micro(struct timeval *tv1, struct timeval *tv2) {
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
	return(diff);
}

int main(int argc, char** argv, char** envp) {
	struct timeval tv_start, tv_end;

	CHECK_NOT_M1(gettimeofday(&tv_start, NULL));
	CHECK_NOT_M1(usleep(50 * 1000));
	CHECK_NOT_M1(gettimeofday(&tv_end, NULL));
	print_timeval(&tv_start, "start");
	print_timeval(&tv_end, "end");
	unsigned long diff=diff_timeval_in_micro(&tv_start, &tv_end);
	printf("diff is %lu\n", diff);
	return EXIT_SUCCESS;
}
