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
#include <pthread.h>	// for pthread_create, pthread_join
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for exit(3)
#include <unistd.h>	// for syscall(2)
#include <sys/types.h>	// for gettid(2)
#include <sys/syscall.h>// for syscall(2)
#include <strings.h>	// for bzero(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()

/*
 * This test shows how to test for bad stack state (distance from
 * end of the stack is too short...)
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

void check_stack() {
	pthread_attr_t at;
	CHECK_ZERO_ERRNO(pthread_getattr_np(pthread_self(), &at));
	void* stackaddr;
	size_t stacksize;
	CHECK_ZERO_ERRNO(pthread_attr_getstack(&at, &stackaddr, &stacksize));
	void* myaddr=&at;
	unsigned int diff=(char*)myaddr-(char*)stackaddr;
	if(diff<stacksize*0.2) {
		fprintf(stderr, "You are too close to the stack edge...\n");
		fprintf(stderr, "======================================\n");
		fprintf(stderr, "stacksize is %zd\n", stacksize);
		fprintf(stderr, "stackaddr %p\n", stackaddr);
		fprintf(stderr, "myaddr %p\n", myaddr);
		fprintf(stderr, "diff %u\n", diff);
		for(int i=10; i>0; i--) {
			fprintf(stderr, "I will die in %2d seconds...\r", i);
			CHECK_ZERO(sleep(1));
		}
		fprintf(stderr, "\n");
		exit(-1);
	}
	CHECK_ZERO_ERRNO(pthread_attr_destroy(&at));
}

typedef struct big_struct {
	char data[7000000];
} my_big_struct;

void call_big() {
	my_big_struct biggie;
	bzero(&biggie, sizeof(my_big_struct));
	check_stack();
	printf("disregard this print %p\n", (void*)&biggie);
}

void* doit(void*) {
	call_big();
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	pthread_t t;
	CHECK_ZERO_ERRNO(pthread_create(&t, NULL, doit, NULL));
	CHECK_ZERO_ERRNO(pthread_join(t, NULL));
	return EXIT_SUCCESS;
}
