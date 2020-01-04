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

/*
 * This is an example showing a process that always yields the CPU using
 * the official sched_yield(2) system call.
 *
 * NOTES:
 * - some people still think that sleep(3) with an argument of 0 will have
 * the same effect as sched_yield(2). This is not the case in Linux
 * where sleep(3) returns to the caller immediately if a parameter of 0 is given
 * to it.
 * - maybe nanosleep(2) with 0 will have better luck of emulating the
 * sched_yield(2) API.
 * - note that this spends little time in user space (10%?!?) and most
 * of the time doing the scheduling in kernel space. You can see this with top(1).
 * - this is kind of an abuse of the kernel scheduler which is forced
 * to run 90% of the time.
 * - the context switches that this process performs are counted as
 * 'nonvoluntary' which is weird. You can see this in '/proc/[pid]/status'.
 * this is weird as they should have been counted as 'voluntary' context switches.
 */

#include <firstinclude.h>
#include <sched.h>	// for sched_yield(2)

int main(int argc, char** argv, char** envp) {
	while(true) {
		sched_yield();
	}
}
