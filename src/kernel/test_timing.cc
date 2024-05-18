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
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/ioctl.h>	// for ioctl(2)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <multiproc_utils.h>	// for my_system()
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <kernel_utils.h>	// for klog_clear(), klog_show_clear()
#include "shared.h"

/*
 * A test that runs ioctl 0 with no data, sleeps for 1 second and then
 * another ioctl 1 with no data.
 */
int main() {
	// file to be used
	const char *filename="/dev/mod_timing";
	fprintf(stderr, "Inserting the driver...\n");
	my_system("sudo rmmod mod_timing");
	my_system("sudo insmod ./mod_timing.ko");
	my_system("sudo chmod 666 %s", filename);

	fprintf(stderr, "Starting\n");
	int d=CHECK_NOT_M1(open(filename, O_RDWR));

	fprintf(stderr, "showing cpus and their frequencies\n");
	klog_clear();
	CHECK_NOT_M1(ioctl(d, IOCTL_TIMING_CLOCK, NULL));
	klog_show_clear();
	// sleep for 1 second to allow us to see the results
	// CHECK_ZERO(sleep(1));

	klog_clear();
	CHECK_NOT_M1(ioctl(d, IOCTL_TIMING_TSC, 1000));
	klog_show_clear();

	klog_clear();
	CHECK_NOT_M1(ioctl(d, IOCTL_TIMING_JIFFIES, 1000));
	klog_show_clear();

	const unsigned int loop=1000000;
	measure m;
	measure_init(&m, "syscall", loop);
	measure_start(&m);
	for (unsigned int i=0; i < loop; i++) {
		CHECK_NOT_M1(ioctl(d, IOCTL_TIMING_EMPTY, NULL));
	}
	measure_end(&m);
	measure_print(&m);

	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
