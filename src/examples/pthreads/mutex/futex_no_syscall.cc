/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <pthread.h>	// for pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_init(3), pthread_mutex_destory(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <multiproc_utils.h>	// for my_system(3)
#include <pthread_utils.h>	// for pthread_print_mutexattr()

/*
 * This example creates a pthread_mutex which is a futex, grabs it and releases
 * it. Run this example. It strace(1)s itself and you will see no system call
 * when acquiring and releasing the lock.
 *
 * Notes:
 * - there is no syscall involved with this futex AT ALL! This is because this futex is private
 *	and therefore not robust. Futexes can be robust in which case you need to tell that at creation
 *	which will notify the kernel about this futex.
 *	by default futexes are private.
 *	This means that even creation of futexes is cheap.
 * - the pthread_mutexattr_getrobust(3), pthread_mutexattr_setrobust(3) are not documented in the
 * manual pages of linux.
 *
 * Problem:
 * - even if I create a process shared mutex the pthread library calls no syscall at mutex_init time!!!
 *	How can this be if the futex is robust?!? investigate...
 * Actually this is not a problem as the futex is robust only if you set it to robust type.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

static int arr_robust[]={
	PTHREAD_MUTEX_STALLED,
	PTHREAD_MUTEX_ROBUST
};

static int arr_pshared[]={
	PTHREAD_PROCESS_SHARED,
	PTHREAD_PROCESS_PRIVATE
};

static int arr_type[]={
	PTHREAD_MUTEX_FAST_NP,
	PTHREAD_MUTEX_RECURSIVE_NP,
	PTHREAD_MUTEX_ERRORCHECK_NP
};

int main(int argc, char** argv, char** envp) {
	if(argc>1) {
		const int size=ARRAY_SIZEOF(arr_type)*ARRAY_SIZEOF(arr_pshared)*ARRAY_SIZEOF(arr_robust);
		pthread_mutex_t mtxs[size];
		pthread_mutexattr_t attrs[size];
		int i=0;
		for(unsigned int irobust=0; irobust<ARRAY_SIZEOF(arr_robust); irobust++) {
			for(unsigned int ipshared=0; ipshared<ARRAY_SIZEOF(arr_pshared); ipshared++) {
				for(unsigned int itype=0; itype<ARRAY_SIZEOF(arr_type); itype++) {
					int robust=arr_robust[irobust];
					int pshared=arr_pshared[ipshared];
					int type=arr_type[itype];
					pthread_mutexattr_t* attr=attrs+i;
					CHECK_ZERO_ERRNO(pthread_mutexattr_init(attr));
					CHECK_ZERO_ERRNO(pthread_mutexattr_setrobust(attr, robust));
					CHECK_ZERO_ERRNO(pthread_mutexattr_setpshared(attr, pshared));
					CHECK_ZERO_ERRNO(pthread_mutexattr_settype(attr, type));
					CHECK_ZERO_ERRNO(pthread_mutex_init(mtxs+i, attr));
					i++;
				}
			}
		}
		printf("started\n");
		for(int i=0; i<size; i++) {
			pthread_mutex_t* mutex=mtxs+i;
			pthread_mutexattr_t* attr=attrs+i;
			pthread_print_mutexattr(attr);
			printf("before critical section\n");
			CHECK_ZERO_ERRNO(pthread_mutex_lock(mutex));
			printf("in critical section\n");
			CHECK_ZERO_ERRNO(pthread_mutex_unlock(mutex));
			printf("after critical section\n");
			CHECK_ZERO_ERRNO(pthread_mutex_destroy(mutex));
		}
		printf("ended\n");
	} else {
		my_system("strace %s argument", argv[0]);
	}
	return EXIT_SUCCESS;
}
