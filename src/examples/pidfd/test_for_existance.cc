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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <errno.h>
#include <string.h>

#ifndef PIDFD_THREAD
#define PIDFD_THREAD 0x1
#endif

#ifndef SYS_pidfd_open
#define SYS_pidfd_open 434
#endif

int main(void)
{
	struct utsname u;
	uname(&u);
	printf("Kernel: %s\n", u.release);
	pid_t self_tid = gettid();
	pid_t self_pid = getpid();
	printf("PID: %d, TID: %d\n", self_pid, self_tid);
	printf("Same thread group: %s\n", self_pid == self_tid ? "yes (main thread)" : "no");
	// Test 1: pidfd_open on process (should work on 5.3+)
	printf("\nTest 1: pidfd_open(pid, 0) on self...\n");
	int pidfd = syscall(SYS_pidfd_open, self_pid, 0);
	if (pidfd >= 0) {
		printf("SUCCESS: pidfd=%d\n", pidfd);
		close(pidfd);
	} else {
		printf("FAILED: errno=%d (%s)\n", errno, strerror(errno));
	}
	// Test 2: pidfd_open with PIDFD_THREAD on main thread (tid == pid)
	printf("\nTest 2: pidfd_open(tid, PIDFD_THREAD) on main thread...\n");
	pidfd = syscall(SYS_pidfd_open, self_tid, PIDFD_THREAD);
	if (pidfd >= 0) {
		printf("SUCCESS: pidfd=%d\n", pidfd);
		close(pidfd);
	} else {
		printf("FAILED: errno=%d (%s)\n", errno, strerror(errno));
		if (errno == EINVAL) {
			printf("EINVAL suggests PIDFD_THREAD not supported (need kernel 6.9+)\n");
		}
	}
	// Test 3: Check /proc for kernel config
	printf("\nTest 3: Checking kernel config...\n");
	FILE *f = fopen("/proc/config.gz", "r");
	if (f) {
		printf("/proc/config.gz exists (can check CONFIG_* options)\n");
		fclose(f);
	} else {
		f = fopen("/boot/config-current", "r");
		if (!f) {
			char path[256];
			snprintf(path, sizeof(path), "/boot/config-%s", u.release);
			f = fopen(path, "r");
		}
		if (f) {
			printf("Found kernel config file\n");
			fclose(f);
		} else {
			printf("No kernel config found\n");
		}
	}
	return 0;
}
