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
#include <cstdlib>	// for EXIT_SUCCESS
#include <iostream>	// for cout, endl
#include <unistd.h>	// for sleep(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <pthread_utils.h>	// for gettid()
#include "mythread.hh"
#include "mymutex.hh"

using namespace std;

/*
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

void* MyThread::realsvc(void* arg) {
	MyThread* t=(MyThread*)arg;
	t->svc();
	return NULL;
}

MyThread::MyThread() : myid(-1) {
}

MyThread::~MyThread() {
}

void MyThread::start() {
	CHECK_ZERO_ERRNO(pthread_create(&myid, NULL, realsvc, (void*)this));
}

void MyThread::join() {
	CHECK_ZERO_ERRNO(pthread_join(myid, NULL));
}

class ImpThread: public MyThread {
private:
	int limit;
	int sleep_time;
	MyMutex& m;

public:
	ImpThread(int ilimit, int isleep_time, MyMutex& mm) : limit(ilimit), sleep_time(isleep_time), m(mm) {
	}

protected:
	virtual void svc() {
		pid_t tid=gettid();
		cout << "thread " << tid << " starting" << endl;
		for(int i=0; i<limit; i++) {
			m.lock();
			cout << "Hello from thread " << tid << " num is " << i << endl;
			m.unlock();
			CHECK_ZERO(sleep(sleep_time));
		}
		cout << "thread " << tid << " ending" << endl;
	}
};

int main() {
	MyMutex m;
	ImpThread thr1(10, 1, m);
	ImpThread thr2(5, 2, m);
	thr1.start();
	thr2.start();
	thr1.join();
	thr2.join();
	return EXIT_SUCCESS;
}
