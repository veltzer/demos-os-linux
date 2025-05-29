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
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()

/*
 * This is a demo of thread local storage using gcc __thread storage modifier.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

static __thread int myid;

static void* worker(void* arg) {
	int* pint=(int*)arg;
	myid=*pint;
	TRACE("going to deallocate [%p]", (void*)pint);
	free(pint);
	// now lets pull our id
	TRACE("myid is [%d]", myid);
	return NULL;
}

int main() {
	const unsigned int num=4;
	pthread_t threads[num];
	for(unsigned int i=0; i<num; i++) {
		int* p=(int*)malloc(sizeof(int));
		*p=i;
		TRACE("allocated [%p]", (void*)p);
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, p));
	}
	myid=1000;
	TRACE("myid is [%d]", myid);
	for(unsigned int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	return EXIT_SUCCESS;
}
