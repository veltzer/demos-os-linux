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
#define ACE_TRACE 0
#include <ace/Task.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Another solution to the exercise. This time trying to solve using ACE
 * conditions = number of threads.
 */

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

// number of threads we will use
unsigned int num_threads;
// number of attempts
unsigned int attempts;
// should we write debug messages ?
bool debug;

class SharedResource {
private:
	// This is the ACE mutex that wraps the OS mutex which we will
	// use to prevent simultaneous access to the resource.
	ACE_Condition_Thread_Mutex **cond;
	// This is to protect the methods
	ACE_Thread_Mutex m;
	// This is the actual counter. We will initialize it in the constructor.
	unsigned int LockedCounter;
	// This is the attempt counter. We will initialize it in the constructor.
	unsigned int AttemptCounter;

public:
	SharedResource() {
		cond=new ACE_Condition_Thread_Mutex*[num_threads];
		for(unsigned int i=0; i<num_threads; i++) {
			cond[i]=new ACE_Condition_Thread_Mutex(m);
		}
		LockedCounter=0;
		AttemptCounter=0;
	}
	// No need for any locking for getting the value
	unsigned int getLockedCounter() {
		return(LockedCounter);
	}
	// No need for any locking for getting the value
	unsigned int getAttemptCounter() {
		return(AttemptCounter);
	}
	// This method only attempts one increase
	void attemptIncreaseValue(unsigned int value) {
		if(debug) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) waiting for lock\n")));
		}
		m.acquire();
		AttemptCounter++;
		if(debug) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Trying to update variable modulu=%d value=%d\n"), LockedCounter % num_threads, value));
		}
		while(LockedCounter % num_threads!=value) {
			cond[value]->wait();
		}
		if(debug) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) increasing counter\n")));
		}
		LockedCounter++;
		value++;
		value%=num_threads;
		cond[value]->signal();
		m.release();
	}
};

class HA_CommandHandler : public ACE_Task_Base {
private:
	SharedResource& sharedResource;
	const unsigned int value;

public:
	HA_CommandHandler(SharedResource & sharedResource, const unsigned int value) : sharedResource(sharedResource), value(value) {
	}

	// The real body of the thread
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) start\n")));
		for(unsigned int i=0; i<attempts; i++) {
			sharedResource.attemptIncreaseValue(value);
		}
		return(0);
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [num_threads] [attempts] [debug]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 3 10000 0\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	num_threads=atoi(argv[1]);
	attempts=atoi(argv[2]);
	debug=atoi(argv[3]);

	// real code
	SharedResource sharedResource;
	HA_CommandHandler* handlers[num_threads];
	for(unsigned int i=0; i<num_threads; i++) {
		handlers[i]=new HA_CommandHandler(sharedResource, i);
	}
	for(unsigned int i=0; i<num_threads; i++) {
		handlers[i]->activate();
	}
	for(unsigned int i=0; i<num_threads; i++) {
		handlers[i]->wait();
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) LockedCounter=%u\n"), sharedResource.getLockedCounter()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) GeneralCounter=%u\n"), sharedResource.getAttemptCounter()));
	return EXIT_SUCCESS;
}
