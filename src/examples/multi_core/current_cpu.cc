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
#include <stdio.h>	// for printf(3)
#include <sched.h>	// for sched_getcpu(2)
#include <unistd.h>	// for sleep(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an example of a process that prints the CPU it is running on
 * and lets you change the CPU that it is on.
 */

int main() {
	pid_t pid=getpid();
	printf("change my cpu using [taskset --cpu-list --pid [list] %d]\n", pid);
	int cpunum=sched_getcpu();
	printf("sched_getcpu() says I'm running on core %d\n", cpunum);
	while(true) {
		printf("Still alive!\n");
		int newcpunum=CHECK_NOT_M1(sched_getcpu());
		if(cpunum!=newcpunum) {
			cpunum=newcpunum;
			printf("I've switched to running on core %d\n", cpunum);
		}
		sleep(1);
	}
	return EXIT_SUCCESS;
}
