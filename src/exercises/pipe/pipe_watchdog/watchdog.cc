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
#include <sys/types.h>	// for wait3(2), kill(2), SIGKILL, mkfifo(3)
#include <sys/time.h>	// for wait3(2)
#include <sys/resource.h>	// for wait3(2)
#include <sys/wait.h>	// for wait3(2)
#include <signal.h>	// for kill(2), SIGKILL
#include <unistd.h>	// for unlink(2), close(2), fork(2)
#include <time.h>	// for time(2), ctime(3)
#include <string.h>	// for strlen(3), strerror(3)
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)
#include <sys/stat.h>	// for mkfifo(3)
#include <stdio.h>	// for printf(3)
#include <errno.h>	// for errno(object)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL_CHARP()
#include <multiproc_utils.h>	// for print_status()

static struct itimerval timer;
static int pipefd[2];
static int gotusr1=0;
static int gotusr2=0;
static int child1pid;
static int child2pid;
const char* pipe_name="/tmp/np";

void sigIntHandler(int gotsig) {
	CHECK_NOT_M1(kill(child1pid, SIGKILL));
	CHECK_NOT_M1(kill(child2pid, SIGKILL));
	CHECK_NOT_M1(unlink(pipe_name));
	exit(EXIT_SUCCESS);
}

void sigChildHandler(int gotsig) {
	pid_t pid;
	do {
		int status;
		pid=CHECK_NOT_M1(wait3(&status, WNOHANG, NULL));
		if(pid>0) {
			print_status(status);
		}
	} while(pid>0);
}

void sigUsrxHandler(int gotsig) {
	time_t now;
	char* stime;
	now=CHECK_NOT_M1(time(NULL));
	stime=CHECK_NOT_NULL_CHARP(ctime(&now));
	stime[strlen(stime)-1]='\0';
	switch(gotsig) {
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

void doChild(const char* chld) {
	CHECK_NOT_M1(execl("./child", "./child", chld, NULL));
	fprintf(stderr, "execl child %s failed: %s\n", chld, strerror(errno));
	exit(errno);
}

void startChild1() {
	child1pid=CHECK_NOT_M1(fork());
	// child
	if(child1pid==0) {
		CHECK_NOT_M1(close(pipefd[0]));
		CHECK_NOT_M1(dup2(pipefd[1], 1));
		CHECK_NOT_M1(close(pipefd[1]));
		doChild("1");
		exit(EXIT_SUCCESS);
	}
}

void startChild2() {
	child2pid=CHECK_NOT_M1(fork());
	// child
	if(child2pid==0) {
		CHECK_NOT_M1(close(pipefd[1]));
		CHECK_NOT_M1(dup2(pipefd[0], 0));
		CHECK_NOT_M1(close(pipefd[0]));
		doChild("2");
		exit(EXIT_SUCCESS);
	}
}

void sigAlrmHandler(int gotsig) {
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

int main(int argc, char** argv, char** envp) {
	struct sigaction sigusr, sigchld, sigalrm, sigint;
	sigset_t emptyset;
	CHECK_NOT_M1(mkfifo(pipe_name, 0666));
	sigemptyset(&emptyset);
	sigusr.sa_handler=sigUsrxHandler;
	sigusr.sa_mask=emptyset;
	sigusr.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGUSR1, &sigusr, NULL));
	CHECK_NOT_M1(sigaction(SIGUSR2, &sigusr, NULL));
	sigchld.sa_handler=sigChildHandler;
	sigchld.sa_mask=emptyset;
	sigchld.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGCHLD, &sigchld, NULL));
	sigalrm.sa_handler=sigAlrmHandler;
	sigalrm.sa_mask=emptyset;
	sigalrm.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGALRM, &sigalrm, NULL));
	sigint.sa_handler=sigIntHandler;
	sigint.sa_mask=emptyset;
	sigint.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGINT, &sigint, NULL));
	timer.it_interval.tv_sec=5;
	timer.it_interval.tv_usec=0;
	timer.it_value.tv_sec=5;
	timer.it_value.tv_usec=0;
	CHECK_NOT_M1(setitimer(ITIMER_REAL, &timer, NULL));
	CHECK_NOT_M1(pipe(pipefd));
	startChild1();
	startChild2();
	while(true) {
		CHECK_NOT_M1(sigsuspend(&emptyset));
	}
	return EXIT_SUCCESS;
}
