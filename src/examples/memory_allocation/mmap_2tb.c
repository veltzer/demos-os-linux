/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/mman.h>

int main(void) {
	const size_t PAGE = 4096;
	const size_t TIB = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
	const size_t SIZE = 2ULL * TIB;

	const size_t STEP_PAGES = 256;
	const size_t STEP_BYTES = STEP_PAGES * PAGE;
	const struct timespec delay = { .tv_sec = 0, .tv_nsec = 10 * 1000 * 1000 };

	printf("requesting mmap of %zu bytes (%.2f TiB)\n",
		SIZE, (double)SIZE / (double)TIB);
	fflush(stdout);

	char *p = mmap(NULL, SIZE,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
		-1, 0);
	if (p == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %s\n", strerror(errno));
		return 1;
	}

	printf("mmap succeeded at %p — touching one byte every %zu pages, sleeping %ld ms between steps\n",
		(void *)p, STEP_PAGES, delay.tv_nsec / 1000000);
	fflush(stdout);

	size_t total_pages = SIZE / PAGE;
	size_t touched = 0;
	struct timespec t_start;
	clock_gettime(CLOCK_MONOTONIC, &t_start);

	for (size_t off = 0; off < SIZE; off += STEP_BYTES) {
		p[off] = (char)(off & 0xff);
		touched += STEP_PAGES;

		if ((touched % (STEP_PAGES * 64)) == 0) {
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			double elapsed = (now.tv_sec - t_start.tv_sec) +
				(now.tv_nsec - t_start.tv_nsec) / 1e9;
			double progress = (double)touched / (double)total_pages * 100.0;
			double resident_gib = (double)(off + PAGE) / (1024.0 * 1024.0 * 1024.0);
			printf("\r touched %.2f%%  resident ~%.3f GiB  elapsed %.1fs",
				progress, resident_gib, elapsed);
			fflush(stdout);
		}

		nanosleep(&delay, NULL);
	}

	printf("\ndone touching. press enter to munmap and exit.\n");
	getchar();

	if (munmap(p, SIZE) != 0) {
		fprintf(stderr, "munmap failed: %s\n", strerror(errno));
		return 1;
	}
	return 0;
}
