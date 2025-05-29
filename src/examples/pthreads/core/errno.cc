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
#include <pthread.h>	// for pthread_t:struct, pthread_create(3), pthread_join(3)
#include <errno.h>	// for errno
#include <stdlib.h>	// for EXIT_SUCCESS, rand(3), srand(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ASSERT()
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)

/*
 * This is a demo which shows that errno and related functions
 * all work on a thread by thread level as they should.
 * Each thread manipulates it's own errno variable with complete disregard
 * to other threads and checks that it is correct.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

const unsigned long long loop=10000000000L;
static void* worker(void* p __attribute__((unused))) {
	int current=errno;
	for(unsigned long long i=0; i<loop; i++) {
		if(i%1000==0) {
			if(rand()%2) {
				current=rand();
				errno=current;
			}
		}
		CHECK_ASSERT(errno==current);
	}
	return NULL;
}

int main() {
	// no errors from either getpid(2) or srand(3)
	srand(getpid());
	const int num=10;
	pthread_t threads[num];
	int ids[num];
	void* rets[num];
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	return EXIT_SUCCESS;
}
