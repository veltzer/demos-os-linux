/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3)
#include <unistd.h>	// for sleep(3), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <multiproc_utils.h>	// for my_system()

/*
 * This example explores how the pid/tid space looks if threads other than the main thread
 * create threads.
 *
 * The result is that they all have similar tids, same pid and same ppid. This means
 * they look as if all threads were created by the main thread.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

typedef struct _thread_data {
	int num;
	int level;
	int highest;
} thread_data;

void* create_threads(void* arg) {
	thread_data* pd=(thread_data*)arg;
	int num=pd->num;
	int level=pd->level;
	int highest=pd->highest;
	pthread_t threads[num];
	thread_data td[num];

	if(level>0) {
		for(int i=0; i<num; i++) {
			td[i].num=num;
			td[i].level=level-1;
			td[i].highest=0;
			CHECK_ZERO_ERRNO(pthread_create(threads+i, NULL, create_threads, td+i));
		}
	}
	if(highest) {
		// lets all threads be created
		sleep(1);
		// print status of all threads
		my_system("ps -L -p %d -o lwp,pid,ppid", getpid());
	} else {
		sleep(2);
	}
	if(level>0) {
		for(int i=0; i < num; i++) {
			CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
		}
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	thread_data td;
	td.highest=1;
	printf("creating threads hierarchically...\n");
	td.num=2;
	td.level=2;
	create_threads(&td);
	printf("creating threads only by main thread...\n");
	td.num=4;
	td.level=1;
	create_threads(&td);
	return EXIT_SUCCESS;
}
