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
#include <unistd.h>	// for close(2), fork(2), lseek(2)
#include <sys/types.h>	// for open(2), lseek(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <multiproc_utils.h>	// for my_system()
#include <sys/param.h>	// for MAXPATHLEN

/*
 * This application simply tests what happens at the driver level when user space
 * calls the fork(2) system call.
 *
 * The results:
 * - no call is made to the device driver at fork(2) time.
 * - this is identical to what happens when user space calls clone(2) to create more threads.
 * - this means that at the driver level you cannot distinguish between multi-threaded and
 * processed applications using your driver.
 * - this means that a driver which can only server one "struct filp*" (supports only a single
 * open(2) call) can serve both multi-processed and multi-threaded applications.
 * - this is good.
 * - in this case files backed by device drivers as different from files backed by regular
 * file systems where on fork(2) you get a different offset and options for parent and child.
 */

int main(int argc, char** argv, char** envp) {
	const char* name="mod_fork";
	char filename[MAXPATHLEN];
	snprintf(filename, MAXPATHLEN, "/dev/%s", name);
	printf("Inserting the driver...\n");
	my_system("sudo sh -c \"echo starting > /dev/kmsg\"");
	my_system("sudo sh -c \"rmmod %s 2> /dev/null; /bin/true\"", name);
	my_system("sudo insmod ./%s.ko", name);
	my_system("sudo chmod 666 /dev/%s", name);

	int fd=CHECK_NOT_M1(open(filename, O_RDWR));
	pid_t pid=CHECK_NOT_M1(fork());
	const int count=10;
	if(pid) {
		// parent
		for(int i=0;i<count;i++) {
			char c;
			CHECK_NOT_M1(read(fd,&c,1));
			sleep(1);
		}
	} else {
		// child
		for(int i=0;i<count;i++) {
			char c;
			CHECK_NOT_M1(read(fd,&c,1));
			sleep(1);
		}
	}
	sleep(2);
	CHECK_NOT_M1(close(fd));
	my_system("sudo sh -c \"rmmod %s 2> /dev/null; /bin/true\"", name);
	return EXIT_SUCCESS;
}
