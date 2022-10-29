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
#include <stdio.h>	// for fprintf(3)
#include <pthread.h>	// for pthread_create(3), pthread_join(3), pthread_spin_init(3), pthread_spin_destroy(3), pthread_spin_lock(3), pthread_spin_unlock(3), pthread_attr_init(3), pthread_attr_setaffinity_np(3)
#include <unistd.h>	// for sysconf(3), usleep(3)
#include <sched.h>	// for CPU_ZERO(3), CPU_SET(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <list>	// for std::list<T>
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()

/*
 * This is a solution to the synchronized queue exercise.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

template <typename T> class SynchronizedQueue {
private:
	std::list<T> mylist;
	pthread_mutex_t mymutex;
	pthread_cond_t mycond;
	unsigned int waiters;

public:
	SynchronizedQueue() {
		CHECK_ZERO_ERRNO(pthread_mutex_init(&mymutex, NULL));
		CHECK_ZERO_ERRNO(pthread_cond_init(&mycond, NULL));
		waiters=0;
	}
	void put(const T& t) {
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&mymutex));
		mylist.push_back(t);
		// only wake up one getter if there are any getters
		// We only wake up one because we only put one data
		// element on the queue. If another putter comes
		// along he will wake up another one...
		if(waiters>0) {
			CHECK_ZERO_ERRNO(pthread_cond_signal(&mycond));
		}
		// a simpler piece of code, which is less efficients, could be:
		// CHECK_ZERO_ERRNO(pthread_cond_broadcast(&mycond));
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&mymutex));
	}
	T get() {
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&mymutex));
		waiters++;
		while (mylist.empty()) {
			CHECK_ZERO_ERRNO(pthread_cond_wait(&mycond, &mymutex));
		}
		// the list is not empty so we can take an element
		T t=mylist.front();
		mylist.pop_front();
		waiters--;
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&mymutex));
		return t;
	}
};

// testing code starts here

typedef struct _thread_data {
	unsigned int num;
	unsigned int count;
	unsigned int max_sleep;
	unsigned int max_msg;
	bool producer;
	SynchronizedQueue<unsigned int>* queue;
} thread_data;

void *worker(void *p) {
	thread_data* td=(thread_data *)p;
	if(td->producer) {
		for(unsigned int i=0; i<td->count; i++) {
			td->queue->put(rand()%td->max_msg);
			CHECK_NOT_M1(usleep(rand()%td->max_sleep));
		}
		td->queue->put(td->max_msg);
	} else {
		while(true) {
			unsigned int i=td->queue->get();
			// TRACE("I got [%d]",i);
			if(i==td->max_msg) {
				break;
			}
		}
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	SynchronizedQueue<unsigned int> queue;
	const unsigned int count=10000;
	const unsigned int max_sleep=100;
	const unsigned int max_msg=100;
	const unsigned int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const unsigned int thread_num=cpu_num;
	pthread_t* threads=new pthread_t[thread_num];
	pthread_attr_t* attrs=new pthread_attr_t[thread_num];
	thread_data* tds=new thread_data[thread_num];
	cpu_set_t* cpu_sets=new cpu_set_t[thread_num];
	for(unsigned int i=0; i<thread_num; i++) {
		tds[i].num=i;
		tds[i].producer=(i%2==0);
		tds[i].count=count;
		tds[i].max_sleep=max_sleep;
		tds[i].max_msg=max_msg;
		tds[i].queue=&queue;
		CPU_ZERO(cpu_sets+i);
		CPU_SET(i%cpu_num, cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs+i, sizeof(cpu_set_t), cpu_sets+i));
		CHECK_ZERO_ERRNO(pthread_create(threads+i, attrs+i, worker, tds+i));
	}
	for(unsigned int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	delete[] threads;
	delete[] attrs;
	delete[] tds;
	delete[] cpu_sets;
	return EXIT_SUCCESS;
}
