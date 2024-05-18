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
#include <sched.h>	// for sched_getcpu(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3), system(3)
#include <unistd.h>	// for usleep(3), sysconf(3)
#include <cpufreq.h>	// for cpufreq_get_freq_kernel(3), cpufreq_get_freq_hardware(3)
#include <unistd.h>	// for sleep(3), syscall(2)
#include <sys/syscall.h>// for syscall(2)
#include <err_utils.h>	// for CHECK_ZERO()
#include <lowlevel_utils.h>	// for getticks(), get_mic_diff()
#include <us_helper.h>	// for get_clk_tck(), no_params()
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const, SCHED_FIFO

/*
 * This example shows the basic performance counter of the system.
 * It runs a thread with affinity to one cpu in order to avoid any
 * lack of synchronization regarding the performance counter between
 * cores.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lcpufreq -lpthread
 */
void* worker(void*) {
	const unsigned int times=10;
	unsigned long long previous=getticks();
	printf("performance counter at start is %llu\n", previous);
	for(unsigned int i=0; i<times; i++) {
		CHECK_ZERO(sleep(1));
		// now print the performance counter
		unsigned long long current=getticks();
		unsigned int mic_diff=get_mic_diff(previous, current);
		printf("performance counter is %llu, diff in micros is %d\n", current, mic_diff);
		previous=current;
	}
	return NULL;
}

int main(int argc, char** argv) {
	no_params(argc, argv);
	sched_run_priority(worker, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	return EXIT_SUCCESS;
}
