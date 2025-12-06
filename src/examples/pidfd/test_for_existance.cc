/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Test PIDFD_THREAD support
 * Compile: g++ -o test_pidfd test_pidfd.cc -pthread
 */

#include <firstinclude.h>
#include <err_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <errno.h>
#include <string.h>
#include <linux/pidfd.h>

int main(void)
{
	struct utsname u;
	uname(&u);
	printf("Kernel: %s\n", u.release);
	pid_t self_tid = gettid();
	pid_t self_pid = getpid();
	printf("PID: %d, TID: %d\n", self_pid, self_tid);
	printf("Same thread group: %s\n", self_pid == self_tid ? "yes (main thread)" : "no");
	printf("Test 1: pidfd_open(pid, 0) on self...\n");
	int pidfd = CHECK_NOT_M1(syscall(SYS_pidfd_open, self_pid, 0));
	printf("SUCCESS: pidfd=%d\n", pidfd);
	CHECK_NOT_M1(close(pidfd));
#ifdef PIDFD_THREAD
	printf("Test 2: pidfd_open(tid, PIDFD_THREAD) on main thread...\n");
	pidfd = CHECK_NOT_M1(syscall(SYS_pidfd_open, self_tid, PIDFD_THREAD));
	printf("SUCCESS: pidfd=%d\n", pidfd);
	CHECK_NOT_M1(close(pidfd));
#endif // PIDFD_THREAD
	printf("Test 3: Checking kernel config...\n");
	FILE *f = CHECK_NOT_NULL_FILEP(fopen("/proc/config.gz", "r"));
	CHECK_ZERO_ERRNO(fclose(f));
	return EXIT_SUCCESS;
}
