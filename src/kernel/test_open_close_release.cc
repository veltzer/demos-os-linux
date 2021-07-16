/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for open(2), waitpid(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)
#include <unistd.h>	// for close(2), fork(2)
#include <sys/wait.h>	// for waitpid(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <kernel_utils.h>	// for klog_show(), klog_clear()
#include <multiproc_utils.h>	// for my_system()
#include <us_helper.h>	// for waitkey()

/*
 * This is a test which will explain which methods in the driver are
 * called. Here are the scenarios which are shown:
 * - a single process opening and closing.
 * - a pair of processes both opening and closing the same file.
 * - a process which open a file and dies.
 * - two processes one which dies.
 *
 * This example explains when open and release are called.
 * open - is called when a unique new fd is created based on the device driver file.
 *	It is NOT called when an already existing fd is duplicated like in the
 *	case of fork(2) or dup(2).
 * release - is called when the final fd of the last process is closed. This means
 *	that no userspace process still holds a connection to that fd.
 *	It is NOT called when a close(2) occurs which is not the last one...
 * Common questions are:
 * 1. how do I know within the driver that a certain process
 * that talked to my driver died?
 * 2. how do I know within the driver that a new process
 * was created that is using a file descriptor that I am serving?
 * 3. how do I know within the driver that a file descriptor
 * that I am serving just god dupped?
 * Some attempts at answers:
 * - you don't. file descriptors in UNIX are NOT tied to
 * one process - they may be shared. You only get notification when they
 * are created or finally destroyed.
 * - one strategy is to not share file descriptors. Close them in the parent or child if
 * you fork or only open them after forks.
 * - have every process which closes the descriptor do
 * an ioctl(2) on it before closing notifying you. The same could be done
 * after fork(2) or dup(2). But why would you want that?
 */

// file descriptor
static int d, d2;
// file to be used
static const char *filename="/dev/mod_open_close_release";

void do_open_close(void) {
	int d=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(close(d));
}

void do_close(void) {
	CHECK_NOT_M1(close(d));
}

void do_open_and_forget(void) {
	CHECK_NOT_M1(open(filename, O_RDWR));
}

void do_open_and_segfault(void) {
	CHECK_NOT_M1(open(filename, O_RDWR));
	char *p=(char *)NULL;
	*p=0;
}

pid_t run_in_process(void (*f)(void)) {
	pid_t pid=CHECK_NOT_M1(fork());
	if (pid==0) {
		f();
		exit(EXIT_SUCCESS);
	} else {
		return(pid);
	}
}

int main(int argc, char** argv, char** envp) {
	printf("Inserting the driver...\n");
	my_system("sudo rmmod mod_open_close_release");
	my_system("sudo insmod ./mod_open_close_release.ko");
	my_system("sudo chmod 666 %s", filename);
	printf("Scenario 1 - a single process opening and closing...\n");
	printf("=============================================================\n");
	klog_clear();
	d=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(close(d));
	klog_show();
	waitkey(NULL);
	printf("Scenario 2 - a single process opening twice and closing twice...\n");
	printf("=============================================================\n");
	klog_clear();
	d=CHECK_NOT_M1(open(filename, O_RDWR));
	d2=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(close(d));
	CHECK_NOT_M1(close(d2));
	klog_show();
	waitkey(NULL);
	printf("Scenario 3 - two process opening and closing the same file...\n");
	printf("=============================================================\n");
	klog_clear();
	pid_t c1=run_in_process(do_open_close);
	pid_t c2=run_in_process(do_open_close);
	int status;
	CHECK_NOT_M1(waitpid(c1, &status, 0));
	CHECK_NOT_M1(waitpid(c2, &status, 0));
	klog_show();
	waitkey(NULL);
	printf("Scenario 4 - parent opens, spawns two children who close and closes...\n");
	printf("=============================================================\n");
	klog_clear();
	d=CHECK_NOT_M1(open(filename, O_RDWR));
	c1=run_in_process(do_close);
	c2=run_in_process(do_close);
	CHECK_NOT_M1(close(d));
	CHECK_NOT_M1(waitpid(c1, &status, 0));
	CHECK_NOT_M1(waitpid(c2, &status, 0));
	klog_show();
	waitkey(NULL);
	printf("Scenario 5 - open, dup and two closes...\n");
	printf("=============================================================\n");
	klog_clear();
	d=CHECK_NOT_M1(open(filename, O_RDWR));
	d2=CHECK_NOT_M1(dup(d));
	CHECK_NOT_M1(close(d));
	CHECK_NOT_M1(close(d2));
	klog_show();
	waitkey(NULL);
	printf("Scenario 6 - open and forget to close\n");
	printf("=============================================================\n");
	klog_clear();
	c1=run_in_process(do_open_and_forget);
	CHECK_NOT_M1(waitpid(c1, &status, 0));
	klog_show();
	waitkey(NULL);
	printf("Scenario 7 - open and segfault\n");
	printf("=============================================================\n");
	klog_clear();
	c1=run_in_process(do_open_and_segfault);
	CHECK_NOT_M1(waitpid(c1, &status, 0));
	klog_show();
	waitkey(NULL);
	return EXIT_SUCCESS;
}
