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
#include <sys/time.h>	// for setitimer(2)
#include <sys/types.h>	// for wait3(2)
#include <sys/time.h>	// for wait3(2)
#include <sys/resource.h>	// for wait3(2)
#include <sys/wait.h>	// for wait3(2)
#include <time.h>	// for ctime(3), time(2)
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for execl(2), fork(2), pause(2)
#include <sys/types.h>	// for kill(2)
#include <signal.h>	// for kill(2), sigemptyset(2), sigaction(2)
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)
#include <string.h>	// for strlen(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()

static struct itimerval timer;
static int gotusr1=0;
static int gotusr2=0;
static int child1pid;
static int child2pid;

void sigintHandler(int gotsig) {
	CHECK_NOT_M1(kill(child1pid, SIGKILL));
	CHECK_NOT_M1(kill(child2pid, SIGKILL));
	exit(EXIT_SUCCESS);
}

void sigchildHandler(int gotsig) {
	int status;
	pid_t pid=CHECK_NOT_M1(wait3(&status, WNOHANG, NULL));
	while(pid>0) {
		if(WIFEXITED(status))
			printf("Child %d exited. Status: %d\n", pid, WEXITSTATUS(status));
		if(WIFSIGNALED(status))
			printf("Child %d killed. Signal: %d\n", pid, WTERMSIG(status));
		pid=CHECK_NOT_M1(wait3(&status, WNOHANG, NULL));
	}
}

void watchsigHandler(int gotsig) {
	time_t now=time(NULL);
	char* stime=ctime(&now);
	stime[strlen(stime)-1]='\0';
	switch (gotsig) {
	case SIGUSR1:
		gotusr1=1;
		printf("%s: TTL from 1\n", stime);
		break;
	case SIGUSR2:
		gotusr2=1;
		printf("%s: TTL from 2\n", stime);
		break;
	}
	if(gotusr1 && gotusr2) {
		CHECK_NOT_M1(setitimer(ITIMER_REAL, &timer, NULL));
		gotusr1=gotusr2=0;
	}
}

void doChild1() {
	CHECK_NOT_M1(execl("./child", "./child", "1", NULL));
}

void doChild2() {
	CHECK_NOT_M1(execl("./child", "./child", "2", NULL));
}

void startChild1() {
	child1pid=CHECK_NOT_M1(fork());
	if(child1pid==0) {
		doChild1();
	}
}

void startChild2() {
	child2pid=CHECK_NOT_M1(fork());
	if(child2pid==0) {
		doChild2();
	}
}

void timeoutsigHandler(int gotsig) {
	if(gotusr1==0) {
		printf("Signal form child 1 ID %d lost. Restarting\n", child1pid);
		CHECK_NOT_M1(kill(child1pid, SIGKILL));
		startChild1();
	}
	if(gotusr2==0) {
		printf("Signal from child 2 ID %d lost. Restarting\n", child2pid);
		CHECK_NOT_M1(kill(child2pid, SIGKILL));
		startChild2();
	}
}

int main() {
	struct sigaction sigusr, sigchld, sigalrm, sigint;
	sigset_t emptyset;
	sigemptyset(&emptyset);
	sigusr.sa_handler=watchsigHandler;
	sigusr.sa_mask=emptyset;
	sigusr.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGUSR1, &sigusr, NULL));
	CHECK_NOT_M1(sigaction(SIGUSR2, &sigusr, NULL));
	sigchld.sa_handler=sigchildHandler;
	sigchld.sa_mask=emptyset;
	sigchld.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGCHLD, &sigchld, NULL));
	sigalrm.sa_handler=timeoutsigHandler;
	sigalrm.sa_mask=emptyset;
	sigalrm.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGALRM, &sigalrm, NULL));
	sigint.sa_handler=sigintHandler;
	sigint.sa_mask=emptyset;
	sigint.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGINT, &sigint, NULL));
	timer.it_interval.tv_sec=5;
	timer.it_interval.tv_usec=0;
	timer.it_value.tv_sec=5;
	timer.it_value.tv_usec=0;
	CHECK_NOT_M1(setitimer(ITIMER_REAL, &timer, NULL));
	startChild1();
	startChild2();
	while(true) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
	}
	return EXIT_SUCCESS;
}
