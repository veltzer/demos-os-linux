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
#define ACE_NTRACE 1
#include <ace/Task.h>	// for ACE_Task_Base
#include <sched.h>	// for sched_yield(2)
#include <assert.h>	// for assert(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3), EXIT_FAILURE

/*
 * This is a solution to the exercise.
 *
 * Try to run this with "taskset 1" and more to see the difference in performance.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 *
 */

// number of threads we will use
unsigned int num_threads;
// number of attempts each thread will try
unsigned int attempts=10000;
// should the threads yield one for the other ?
bool yield;
// should we write debug messages ?
bool debug;

class SharedResource{
private:
	// This is the ACE mutex that wraps the OS mutex which we will
	// use to prevent simultaneous access to the resource.
	ACE_Thread_Mutex mutex;
	// This is the actual counter. We will initialize it in the constructor.
	unsigned int LockedCounter;
	// This is the attempt counter. We will initialize it in the constructor.
	unsigned int AttemptCounter;

public:
	SharedResource() {
		LockedCounter=0;
		AttemptCounter=0;
	}

	// No need for any locking for getting the value
	unsigned int getLockedCounter() {
		return LockedCounter;
	}

	// No need for any locking for getting the value
	unsigned int getAttemptCounter() {
		return AttemptCounter;
	}

	// This method only attempts one increase
	void attemptIncreaseValue(unsigned int value) {
		if(debug) {
			ACE_DEBUG((LM_DEBUG, "(%t) waiting for lock\n"));
		}
		// wait until mutex is acquired
		mutex.acquire();
		AttemptCounter++;
		if(debug) {
			ACE_DEBUG((LM_DEBUG, "(%t) Trying to update variable modulu=%d value=%d\n", LockedCounter % num_threads, value));
		}
		if (LockedCounter % num_threads==value) {
			if(debug) {
				ACE_DEBUG((LM_DEBUG, "(%t) increasing counter\n"));
			}
			LockedCounter++;
		}
		mutex.release();
		if(debug) {
			ACE_DEBUG((LM_DEBUG, "(%t) released lock\n"));
		}
		if(yield) {
			// two alternatives for yielding: the ACE and the native OS one...
			ACE_OS::thr_yield();
			/*
			 * int ret=sched_yield();
			 * assert(ret!=-1);
			 */
		}
	}
};

class HA_CommandHandler: public ACE_Task_Base {
private:
	SharedResource& sharedResource;
	const unsigned int value;

public:
	HA_CommandHandler(SharedResource & sharedResource, const unsigned int value) : sharedResource(sharedResource), value(value) {
	}

	// The real body of the thread
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, "(%t) start\n"));
		for(unsigned int i=0; i<attempts; i++) {
			sharedResource.attemptIncreaseValue(value);
		}
		return 0;
	}
};

int main(int argc, char** argv) {
	if(argc!=5) {
		fprintf(stderr, "%s: usage: %s [num_threads] [attempts] [yield] [debug]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 3 10000 0 0\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	num_threads=atoi(argv[1]);
	attempts=atoi(argv[2]);
	yield=atoi(argv[3]);
	debug=atoi(argv[4]);

	// real code starts here
	SharedResource sharedResource;
	HA_CommandHandler** handlers=new HA_CommandHandler*[num_threads];
	for(unsigned int i=0; i<num_threads; i++) {
		handlers[i]=new HA_CommandHandler(sharedResource, i);
	}
	for(unsigned int i=0; i<num_threads; i++) {
		handlers[i]->activate();
	}
	for(unsigned int i=0; i<num_threads; i++) {
		handlers[i]->wait();
	}
	delete[] handlers;
	ACE_DEBUG((LM_DEBUG, "(%t) LockedCounter=%d\n", sharedResource.getLockedCounter()));
	ACE_DEBUG((LM_DEBUG, "(%t) AttemptCounter=%d\n", sharedResource.getAttemptCounter()));
	return EXIT_SUCCESS;
}
