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
#include <stdio.h>	// for printf(3), fprintf(3), stderr, fflush(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <pthread.h>	// for pthread_t, pthread_attr_t, pthread_create(3), pthread_attr_init(3), pthread_attr_destroy(3), pthread_attr_setaffinity_np(3)
#include <sched.h>	// for cpu_set_t, CPU_ZERO(3), CPU_SET(3)
#include <semaphore.h>	// for sem_t:struct, sem_post(3), sem_wait(3), sem_init(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <random_utils.hh>	// for MersenneTwister:object
#include <disassembly_utils.h>	// for disassemble_function()

/*
 * This example explores memory reordering issues by the machine.
 *
 * The heart of this example are two threads. with x=0, y=0.
 * one thread doing
 * x=1, ry=y
 * and the other
 * y=2, rx=x
 * Each is doing a write and then a read.
 * If, at the end of the procedure rx==rx==0 then you can deduce
 * that at least one pair of machine instruction executed
 * by one of the threads was reordered.
 *
 * References:
 * http://preshing.com/20120515/memory-reordering-caught-in-the-act
 *
 * So that we could disassemble this easily...
 * EXTRA_COMPILE_FLAGS=-g3
 * We are multi threaded in this example
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

typedef struct _threaddata {
	sem_t beginSema1;
	sem_t beginSema2;
	sem_t endSema;
	int x, y;
	int r1, r2;
	volatile int vx, vy;
	volatile int vr1, vr2;
	unsigned int iterations;
} threaddata;

#define CREATE_FUNCS(name, code, code_before, code_after) \
	void *thread1_ ## name(void *param) { \
		threaddata* pd=(threaddata*)param; \
		MersenneTwister random(1); \
		for(unsigned int i=0; i<pd->iterations; i++) { \
			/* Wait for signal */ \
			sem_wait(&pd->beginSema1); \
			/* Random delay */ \
			while (random.integer() % 8 != 0) {} \
			/* ----- THE TRANSACTION! ----- */ \
			code_before; \
			pd->x = 1; \
			code; \
			pd->r1 = pd->y;	\
			code_after; \
			/* Notify transaction complete */ \
			sem_post(&pd->endSema);	\
		} \
		/* Never returns */ \
		return NULL; \
	} \
	void *thread2_ ## name(void *param) { \
		threaddata* pd=(threaddata*)param; \
		MersenneTwister random(2); \
		for(unsigned int i=0; i<pd->iterations; i++) { \
			/* Wait for signal */ \
			sem_wait(&pd->beginSema2); \
			/* Random delay */ \
			while (random.integer() % 8 != 0) {} \
			/* ----- THE TRANSACTION! ----- */ \
			code_before; \
			pd->y = 1; \
			code; \
			pd->r2 = pd->x;	\
			code_after; \
			/* Notify transaction complete */ \
			sem_post(&pd->endSema);	\
		} \
		/* Never returns */ \
		return NULL; \
	} \
	void *thread1v_ ## name(void *param) { \
		threaddata* pd=(threaddata*)param; \
		MersenneTwister random(1); \
		for(unsigned int i=0; i<pd->iterations; i++) { \
			/* Wait for signal */ \
			sem_wait(&pd->beginSema1); \
			/* Random delay */ \
			while (random.integer() % 8 != 0) {} \
			/* ----- THE TRANSACTION! ----- */ \
			code_before; \
			pd->vx = 1; \
			code; \
			pd->vr1 = pd->vy; \
			code_after; \
			/* Notify transaction complete */ \
			sem_post(&pd->endSema);	\
		} \
		/* Never returns */ \
		return NULL; \
	} \
	void *thread2v_ ## name(void *param) { \
		threaddata* pd=(threaddata*)param; \
		MersenneTwister random(2); \
		for(unsigned int i=0; i<pd->iterations; i++) { \
			/* Wait for signal */ \
			sem_wait(&pd->beginSema2); \
			/* Random delay */ \
			while (random.integer() % 8 != 0) {} \
			/* ----- THE TRANSACTION! ----- */ \
			code_before; \
			pd->vy = 1; \
			code; \
			pd->vr2 = pd->vx; \
			code_after; \
			/* Notify transaction complete */ \
			sem_post(&pd->endSema);	\
		} \
		/* Never returns */ \
		return NULL; \
	}

// lets compile the functions
// nothing
CREATE_FUNCS(nothing, , , );
// compiler barrier
CREATE_FUNCS(compiler_barrier, asm volatile ("" ::: "memory"), , );
// sfence (does not work)
CREATE_FUNCS(sfence, asm volatile ("sfence" ::: "memory"), , );
// lfence (does not work)
CREATE_FUNCS(lfence, asm volatile ("lfence" ::: "memory"), , );
// sfence+lfence (does not work)
CREATE_FUNCS(slfence, asm volatile ("sfence" ::: "memory"); asm volatile ("lfence" ::: "memory"), , );
// correct sfence+lfence (does work)
// CREATE_FUNCS(cslfence, , , );
// mfence (does work)
CREATE_FUNCS(mfence, asm volatile ("mfence" ::: "memory"), , );
// lock orl using compiler builtin (does work)
CREATE_FUNCS(sync_synchronize, __sync_synchronize(), , );

void run(bool doVolatile, void *(*start_routine1)(void *), void *(*start_routine2)(void *), threaddata* pd, int core1, int core2, const char* test_name) {
	printf("running test [%s], volatile is [%d]...", test_name, doVolatile);
	fflush(stdout);
	// Spawn the threads
	cpu_set_t cpuset1;
	CPU_ZERO(&cpuset1);
	CPU_SET(core1, &cpuset1);
	pthread_attr_t attrs1;
	CHECK_ZERO_ERRNO(pthread_attr_init(&attrs1));
	CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(&attrs1, sizeof(cpu_set_t), &cpuset1));
	pthread_t thread1;
	CHECK_ZERO_ERRNO(pthread_create(&thread1, &attrs1, start_routine1, pd));
	CHECK_ZERO_ERRNO(pthread_attr_destroy(&attrs1));

	cpu_set_t cpuset2;
	CPU_ZERO(&cpuset2);
	CPU_SET(core2, &cpuset2);

	pthread_attr_t attrs2;
	CHECK_ZERO_ERRNO(pthread_attr_init(&attrs2));
	CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(&attrs2, sizeof(cpu_set_t), &cpuset2));
	pthread_t thread2;
	CHECK_ZERO_ERRNO(pthread_create(&thread2, &attrs2, start_routine2, pd));
	CHECK_ZERO_ERRNO(pthread_attr_destroy(&attrs2));

	int detected = 0;
	for(unsigned int i=0; i<pd->iterations; i++) {
		// Reset x and y
		if(doVolatile) {
			pd->vx = 0;
			pd->vy = 0;
		} else {
			pd->x = 0;
			pd->y = 0;
		}
		// Signal both threads
		CHECK_ZERO_ERRNO(sem_post(&pd->beginSema1));
		CHECK_ZERO_ERRNO(sem_post(&pd->beginSema2));
		// Wait for both threads
		CHECK_ZERO_ERRNO(sem_wait(&pd->endSema));
		CHECK_ZERO_ERRNO(sem_wait(&pd->endSema));
		// Check if there was a reorder
		if(doVolatile) {
			if (pd->vr1 == 0 && pd->vr2 == 0) {
				detected++;
				// printf("%d reorders detected after %d iterations\n", detected, i);
			}
		} else {
			if (pd->r1 == 0 && pd->r2 == 0) {
				detected++;
				// printf("%d reorders detected after %d iterations\n", detected, i);
			}
		}
	}
	if(detected>0) {
		printf("test failed, %d reorders detected\n", detected);
	} else {
		printf("success!\n");
	}
	CHECK_ZERO_ERRNO(pthread_join(thread1, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread2, NULL));
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s core1 core2 iterations\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: to see reordering use\n", argv[0]);
		fprintf(stderr, "%s: example: %s 0 1 40000\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: to see them go away use\n", argv[0]);
		fprintf(stderr, "%s: example: %s 0 0 40000\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	int core1=atoi(argv[1]);
	int core2=atoi(argv[2]);
	unsigned int iterations=atoi(argv[3]);

	// show some disassembly...
	// disassemble_function("thread1Func");
	// disassemble_function("thread2Func");

	// initialize the thread data
	threaddata pd;
	pd.iterations=iterations;
	CHECK_ZERO_ERRNO(sem_init(&pd.beginSema1, 0, 0));
	CHECK_ZERO_ERRNO(sem_init(&pd.beginSema2, 0, 0));
	CHECK_ZERO_ERRNO(sem_init(&pd.endSema, 0, 0));

	run(false, thread1_nothing, thread2_nothing, &pd, core1, core2, "nothing");
	run(false, thread1_compiler_barrier, thread2_compiler_barrier, &pd, core1, core2, "compiler_barrier");
	run(false, thread1_sfence, thread2_sfence, &pd, core1, core2, "sfence");
	run(false, thread1_lfence, thread2_lfence, &pd, core1, core2, "lfence");
	run(false, thread1_slfence, thread2_slfence, &pd, core1, core2, "slfence");
	run(false, thread1_mfence, thread2_mfence, &pd, core1, core2, "mfence");
	run(false, thread1_sync_synchronize, thread2_sync_synchronize, &pd, core1, core2, "sync_synchronize");
	run(true, thread1v_nothing, thread2v_nothing, &pd, core1, core2, "nothing");
	run(true, thread1v_compiler_barrier, thread2v_compiler_barrier, &pd, core1, core2, "compiler_barrier");
	run(true, thread1v_sfence, thread2v_sfence, &pd, core1, core2, "sfence");
	run(true, thread1v_lfence, thread2v_lfence, &pd, core1, core2, "lfence");
	run(true, thread1v_slfence, thread2v_slfence, &pd, core1, core2, "slfence");
	run(true, thread1v_mfence, thread2v_mfence, &pd, core1, core2, "mfence");
	run(true, thread1v_sync_synchronize, thread2v_sync_synchronize, &pd, core1, core2, "sync_synchronize");
	return EXIT_SUCCESS;
}
