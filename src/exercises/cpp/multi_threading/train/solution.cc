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
#include <iostream>	// for cout, endl
#include <stdlib.h>	// for EXIT_SUCCESS
#include <boost/thread.hpp>	// for boost::thread
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()
#include <pthread.h>	// for pthread_barrier_init()

using namespace std;

/*
 * A solution to the train exercise...
 * References:
 * http://stackoverflow.com/questions/3209918/how-to-implement-an-exchanger-rendezvous-pattern-in-c
 *
 * EXTRA_LINK_FLAGS_AFTER=-lboost_thread -lboost_system -lpthread
 */

class Worker1{
private:
	unsigned int counter;
	pthread_barrier_t* bar;

public:
	Worker1(unsigned int counter, pthread_barrier_t* bar) : counter(counter), bar(bar) {
	}
	void operator()() {
		// sleep for a random amount of time, simulating some work done by
		// this thread
		// pthread_barrier_wait(bar);
		// position+=1;
		// position%=size;
	}
};
class Worker2{
private:
	unsigned int counter;
	pthread_barrier_t* bar;

public:
	Worker2(unsigned int counter, pthread_barrier_t* bar) : counter(counter), bar(bar) {
	}
	void operator()() {
		pthread_barrier_wait(bar);
		/*
		 * for(int i=0;i<counter;i++) {
		 *	//sleep for a random amount of time, simulating some work done by
		 *	//this thread
		 *	pthread_barrier_wait(bar);
		 *	position+=1;
		 *	position%=size;
		 * }
		 */
	}
};
class Worker3{
private:
	unsigned int counter;
	pthread_barrier_t* bar;

public:
	Worker3(unsigned int counter, pthread_barrier_t* bar) : counter(counter), bar(bar) {
	}
	void operator()() {
		pthread_barrier_wait(bar);
		pthread_barrier_wait(bar);
	}
};
class Worker4{
private:
	unsigned int counter;
	pthread_barrier_t* bar;

public:
	Worker4(unsigned int counter, pthread_barrier_t* bar) : counter(counter), bar(bar) {
	}
	void operator()() {
		pthread_barrier_wait(bar);
		pthread_barrier_wait(bar);
		pthread_barrier_wait(bar);
	}
};

int main() {
	pthread_barrier_t bar;
	unsigned int counter=100;
	CHECK_ZERO_ERRNO(pthread_barrier_init(&bar, NULL, 4));
	Worker1 w1(counter, &bar);
	Worker2 w2(counter, &bar);
	Worker3 w3(counter, &bar);
	Worker4 w4(counter, &bar);

	// start the threads
	boost::thread wt1(w1);
	boost::thread wt2(w2);
	boost::thread wt3(w3);
	boost::thread wt4(w4);
	// print something
	cout << "created the threads doing a join..." << endl;
	// join the threads
	wt1.join();
	wt2.join();
	wt3.join();
	wt4.join();
	CHECK_ZERO_ERRNO(pthread_barrier_destroy(&bar));
	return EXIT_SUCCESS;
}
