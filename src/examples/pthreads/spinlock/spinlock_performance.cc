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
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <pthread.h>	// for pthread_spin_init(3), pthread_spin_lock(3), pthread_spin_unlock(3), pthread_spin_destroy(3), pthread_create(3), pthread_join(3), pthread_mutex_init(3), pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_destroy(3)
#include <Stat.hh>	// for Stat:Object
#include <sched.h>	// for CPU_ZERO(3), CPU_SET(3)
#include <unistd.h>	// for usleep(3)
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()

/*
 * This example explores the performance of spin locks.
 *
 * The idea is to be able to run as many threads as you want affined to any
 * cores that you want. This way you will be able to see the great difference
 * in performance between using spin locks to synchronize two threads running
 * on the same core versus different cores.
 *
 * If you run with the parameters that determine bad behaviour (many spin locking
 * threads on the same CPU) then you will see the time slice of the operating
 * system in the histograms that are produced.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

typedef struct _threaddata{
	unsigned int mutex;
	unsigned int attempts;
	unsigned int sleep_in;
	unsigned int sleep_out;
	pthread_spinlock_t lock;
	pthread_mutex_t mtx;
} threaddata;

static void* worker(void* p) {
	threaddata* td=(threaddata*)p;
	measure m;
	Stat s(40, 100, 40*100/2);
	measure_init(&m, "", 1);
	for(unsigned int i=0; i<td->attempts; i++) {
		measure_start(&m);
		if (td->mutex) {
			CHECK_ZERO_ERRNO(pthread_mutex_lock(&td->mtx));
		} else {
			CHECK_ZERO_ERRNO(pthread_spin_lock(&td->lock));
		}
		measure_end(&m);
		CHECK_NOT_M1(usleep(td->sleep_in));
		if (td->mutex) {
			CHECK_ZERO_ERRNO(pthread_mutex_unlock(&td->mtx));
		} else {
			CHECK_ZERO_ERRNO(pthread_spin_unlock(&td->lock));
		}
		CHECK_NOT_M1(usleep(td->sleep_out));
		s.accept(measure_micro_diff(&m));
	}
	s.print();
	return NULL;
}

int main(int argc, char** argv) {
	if(argc<6) {
		fprintf(stderr, "%s: usage: %s [mutex] [attempts] [sleep_in] [sleep_out] [core] [core] [...]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: for bad performance:\n", argv[0]);
		fprintf(stderr, "%s: example: %s 0 1000 10 10 1 1\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: for good performance:\n", argv[0]);
		fprintf(stderr, "%s: example: %s 0 1000 10 10 1 2\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: to see it improve with mutex:\n", argv[0]);
		fprintf(stderr, "%s: example: %s 1 1000 10 10 1 2\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	threaddata td;
	td.mutex=atoi(argv[1]);
	td.attempts=atoi(argv[2]);
	td.sleep_in=atoi(argv[3]);
	td.sleep_out=atoi(argv[4]);
	CHECK_ZERO_ERRNO(pthread_spin_init(&td.lock, PTHREAD_PROCESS_PRIVATE));
	CHECK_ZERO_ERRNO(pthread_mutex_init(&td.mtx, NULL));
	const unsigned int thread_num=argc-5;
	pthread_t* threads=new pthread_t[thread_num];
	cpu_set_t* cpu_sets=new cpu_set_t[thread_num];
	pthread_attr_t* attrs=new pthread_attr_t[thread_num];
	for(unsigned int i=0; i<thread_num; i++) {
		CPU_ZERO(cpu_sets+i);
		CPU_SET(atoi(argv[i+5]), cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs+i, sizeof(cpu_set_t), cpu_sets+i));
		CHECK_ZERO_ERRNO(pthread_create(threads + i, attrs+i, worker, &td));
	}
	for(unsigned int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	CHECK_ZERO_ERRNO(pthread_spin_destroy(&td.lock));
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&td.mtx));
	delete[] threads;
	delete[] cpu_sets;
	delete[] attrs;
	return EXIT_SUCCESS;
}
