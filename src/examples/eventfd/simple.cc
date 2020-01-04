/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/eventfd.h>// for eventfd(2)
#include <unistd.h>	// for fork(2), close(2), sleep(3), write(2), read(2)
#include <stdlib.h>	// for exit(3), rand(3), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>	// for fprintf(3), strtoull(3)
#include <signal.h>	// for siginterrupt(2), signal(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_INT(), CHECK_ASSERT()
#include <sys/types.h>	// for waitpid(2)
#include <sys/wait.h>	// for waitpid(2)

/*
 * This program demos parent child communication via an event fd
 * This program was shamelssly stolen and modified from the eventfd
 * manpage
 *
 * Notes:
 * - the child may make several calls to write and the parent will only
 * get one wakeup for these calls. This is because eventfd is made for
 * signaling and one signal is just as good as two. The parent can
 * notice that it has been signaled more than once since the sum of the
 * childs passed values is passed to it.
 * - by sending lots of messages from the child on the eventfd you can see
 * the parent missing some of them.
 * - eventfd can be multiplexed using select, poll or epoll.
 */

int main(int argc, char** argv, char** envp) {
	if (argc !=3) {
		fprintf(stderr, "%s: usage: %s [loop count] [max_rand]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 10000 2\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	// parameters
	unsigned int loop_count=atoi(argv[1]);
	unsigned int max_rand=atoi(argv[2]);

	// create the event fd
	int efd=CHECK_NOT_M1(eventfd(0, EFD_NONBLOCK));

	unsigned long long sum=0;
	pid_t child_pid;
	if((child_pid=CHECK_NOT_M1(fork()))) {
		// parent branch
		unsigned int counter=0;
		bool child_dead=false;
		while(child_dead==false) {
			int status;
			// waitpid can return child_pid, 0 or -1
			// child_pid - means the child is dead
			// 0 - means the child is alive
			// -1 - means an error
			pid_t p=CHECK_NOT_M1(waitpid(child_pid, &status, WNOHANG));
			if(p==child_pid) {
				child_dead=true;
			}
			uint64_t u;
			ssize_t s=read(efd, &u, sizeof(uint64_t));
			if(s==-1) {
				CHECK_ASSERT(errno!=-EAGAIN);
			} else {
				counter++;
				sum+=u;
			}
		}
		// pump whatever is left
		uint64_t u;
		ssize_t s=read(efd, &u, sizeof(uint64_t));
		if(s==-1) {
			CHECK_ASSERT(errno!=-EAGAIN);
		} else {
			counter++;
			sum+=u;
		}
		CHECK_NOT_M1(close(efd));
		printf("parent counter is %u\n", counter);
		printf("parent sum is %llu\n", sum);
	} else {
		// child branch
		// so we will get good random numbers
		srand(getpid());
		for(unsigned int i=0; i<loop_count; i++) {
			uint64_t u=rand()%max_rand;
			CHECK_INT(write(efd, &u, sizeof(uint64_t)), sizeof(uint64_t));
			sum+=u;
		}
		CHECK_NOT_M1(close(efd));
		printf("child sum is %llu\n", sum);
	}
	return EXIT_SUCCESS;
}
