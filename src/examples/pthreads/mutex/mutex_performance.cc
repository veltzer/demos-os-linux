/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <pthread.h>	// for pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_init(3), pthread_mutex_destory(3)
#include <semaphore.h>	// for sem_init(3), sem_wait(3), sem_post(3)
#include <sys/types.h>	// for ftok(3), semget(3), semctl(3), semop(3)
#include <sys/ipc.h>	// for ftok(3), semget(3), semctl(3), semop(3)
#include <sys/sem.h>	// for semget(3), semctl(3), semop(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO(), CHECK_NOT_M1()
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * This demo shows the difference between regular pthread mutex (which is a
 * futex) and an expensive one.
 *
 * The idea is a single thread application that just measures a million lock/unlock
 * operations on each type of lock. Simple and effective.
 *
 * We measure ALL kinds of semaphores and mutexes here.
 *
 * Results:
 * the main finding is that
 * regular lock/unlock costs about 90 nanos on modern hardware while SYSV IPC costs
 * 10 times more. In all other aspects all other types of locks (recursive, non
 * recursive, shared, non shared) perform about the same.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

void do_work(pthread_mutex_t* mutex, sem_t* sem, int semid, const char* name) {
	const unsigned int loop=1000000;
	measure m;
	measure_init(&m, name, loop);
	measure_start(&m);
	for(unsigned int i=0; i < loop; i++) {
		if(mutex) {
			CHECK_ZERO_ERRNO(pthread_mutex_lock(mutex));
			CHECK_ZERO_ERRNO(pthread_mutex_unlock(mutex));
		}
		if(sem) {
			CHECK_ZERO(sem_wait(sem));
			CHECK_ZERO(sem_post(sem));
		}
		if(semid!=-1) {
			struct sembuf sops;
			sops.sem_num=0;
			sops.sem_op=1;
			sops.sem_flg=0;
			CHECK_NOT_M1(semop(semid, &sops, 1));
			sops.sem_num=0;
			sops.sem_op=-1;
			sops.sem_flg=0;
			CHECK_NOT_M1(semop(semid, &sops, 1));
		}
	}
	measure_end(&m);
	measure_print(&m);
}

static pthread_mutex_t mutex_fast;
static pthread_mutex_t mutex_recursive;
static pthread_mutex_t mutex_errorcheck;
static sem_t sem_nonshared;
static sem_t sem_shared;
static int semid;

void* work(void* param) {
	do_work(&mutex_fast, NULL, -1, "fast mutexes");
	do_work(&mutex_recursive, NULL, -1, "recursive mutexes");
	do_work(&mutex_errorcheck, NULL, -1, "error checking mutexes");
	do_work(NULL, &sem_nonshared, -1, "non shared semaphores");
	do_work(NULL, &sem_shared, -1, "shared semaphores");
	do_work(NULL, NULL, semid, "SYSV IPC semaphores");
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	key_t key=CHECK_NOT_M1(ftok("/etc/passwd", 'x'));
	semid=CHECK_NOT_M1(semget(key, 1, IPC_CREAT | 0666));
	CHECK_NOT_M1(semctl(semid, 0, SETVAL, 1));

	CHECK_ZERO(sem_init(&sem_nonshared, 0, 1));
	CHECK_ZERO(sem_init(&sem_shared, 1, 1));

	pthread_mutexattr_t attr;
	CHECK_ZERO_ERRNO(pthread_mutexattr_init(&attr));
	CHECK_ZERO_ERRNO(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP));
	CHECK_ZERO_ERRNO(pthread_mutex_init(&mutex_fast, &attr));
	CHECK_ZERO_ERRNO(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP));
	CHECK_ZERO_ERRNO(pthread_mutex_init(&mutex_recursive, &attr));
	CHECK_ZERO_ERRNO(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP));
	CHECK_ZERO_ERRNO(pthread_mutex_init(&mutex_errorcheck, &attr));
	sched_run_priority(work, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&mutex_fast));
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&mutex_recursive));
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&mutex_errorcheck));
	return EXIT_SUCCESS;
}
