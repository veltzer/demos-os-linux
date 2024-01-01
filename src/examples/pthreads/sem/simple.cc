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

#include <firstinclude.h>
#include <unistd.h>	// for sleep(3)
#include <pthread.h>	// for pthread_mutex_init(3), pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_destroy(3), pthread_create(3), pthread_join(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()
#include <semaphore.h> // for sem_t

/*
 * check whether sem_* functions are futex based (implemented in user space).
 * strace(1) this example to see that these semaphores are implemented
 * in user space in the uncontended case (futex(2) based).
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

int main(int argc, char** argv, char** envp) {
	sem_t sem;
	printf("Hello, World!\n");
	CHECK_NOT_M1(sem_init(&sem, 0, 1));
	CHECK_NOT_M1(sem_wait(&sem));
	CHECK_NOT_M1(sem_post(&sem));
	CHECK_NOT_M1(sem_destroy(&sem));
	printf("Goodbye, Cruel World!\n");
	return EXIT_SUCCESS;
}
