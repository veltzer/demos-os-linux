/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS
#include <boost/thread.hpp>	// for boost::thread
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()

/*
 * A solution to the exchanger/randezvous exercise...
 *
 * Notes:
 * - The reason we don't "null" the Exchanger when the second thread
 * comes in and gives it's value is that the first thread is still
 * in the lock. If the second thread by chance will get into the lock
 * (again) before the first thread wakes up, it may accidentaly wake
 * itself up.
 * - The same reason (above) is why we have the delay loop at the
 * begining of the 'get' method and why we are using two conditions
 * instead of just one.
 *
 * References:
 * http://stackoverflow.com/questions/3209918/how-to-implement-an-exchanger-rendezvous-pattern-in-c
 *
 * the next one should be "-lboost_thread" but ubuntu has no symlink to that...
 * EXTRA_LINK_FLAGS=-l:libboost_thread.so.1.62.0 -l:libboost_system.so.1.62.0
 */

template <class T> class Exchanger {
private:
	boost::mutex m;
	boost::condition_variable cv_main;
	boost::condition_variable cv_overflow;
	T *ptr;
	enum { EMPTY, FIRST_ARRIVED, SECOND_ARRIVED } state;

public:
	Exchanger() : ptr(0), state(EMPTY) {
	}

	void exchange(T &t)
	{
		boost::unique_lock<boost::mutex> lock(m);
		// If we arrive while an actual exchange has
		// started but has not finished, keep out of
		// the way.
		while (state == SECOND_ARRIVED) {
			cv_overflow.wait(lock);
		}
		CHECK_ASSERT((state == EMPTY) || (state == FIRST_ARRIVED));
		switch (state) {
		case EMPTY:
			CHECK_ASSERT(!ptr);
			ptr = &t;
			state = FIRST_ARRIVED;
			while (state == FIRST_ARRIVED) {
				cv_main.wait(lock);
			}
			CHECK_ASSERT(state == SECOND_ARRIVED);
			ptr = 0;
			state = EMPTY;
			// Wake up any threads that happened to get
			// the mutex after the other side of the
			// exchanger notified us but before we woke up.
			cv_overflow.notify_all();
			break;
		case FIRST_ARRIVED:
			CHECK_ASSERT(ptr!=NULL);
			state = SECOND_ARRIVED;
			std::swap(t, *ptr);
			cv_main.notify_one();
			break;
		default:
			break;
		}
	}
};

class Worker {
private:
	Exchanger<unsigned int>& e;
	unsigned int max_sleep_time;
	unsigned int loop_count;
	unsigned int min;
	unsigned int max;

public:
	Worker(Exchanger<unsigned int>& ie, unsigned int imax_sleep_time, unsigned int iloop_count, unsigned int imin, unsigned int imax) :
		e(ie),
		max_sleep_time(imax_sleep_time),
		loop_count(iloop_count),
		min(imin),
		max(imax)
	{
	}
	void operator()() {
		for(unsigned int i=0; i<loop_count; i++) {
			CHECK_NOT_M1(usleep(rand()%max_sleep_time));
			unsigned int myval=min+rand()%(max-min);
			e.exchange(myval);
			// assert that I got something out of my range
			CHECK_ASSERT(myval<min || myval>=max);
		}
	}
};

int main(int argc, char** argv, char** envp) {
	srand(0);
	Exchanger<unsigned int> e;
	unsigned int max_sleep_time=1000;
	unsigned int loop_count=10000;
	Worker w1(e, max_sleep_time, loop_count, 100, 200);
	Worker w2(e, max_sleep_time, loop_count, 200, 300);

	// start the threads
	boost::thread workerThread1(w1);
	boost::thread workerThread2(w2);
	// print something
	std::cout << "created the threads doing a join..." << std::endl;
	// join the threads
	workerThread1.join();
	workerThread2.join();
	return EXIT_SUCCESS;
}
