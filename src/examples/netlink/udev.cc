/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <asm/types.h>	// for SOCK_DGRAM
#include <sys/socket.h>	// for socket(2)
#include <linux/netlink.h>	// for AF_NETLINK, NETLINK_KOBJECT_UEVENT
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <unistd.h>	// for read(2)
#include <stdio.h>	// for printf(3)

/*
 * This application should show how to listen to udev events using API...
 *
 * Why would you want to do this ?
 * - you want a real small RT system and don't want udev on it.
 * - you hate the "system" approach that udev takes to running rules.
 * - you want to listen to udev events in parallel to other things with tight integration.
 * - you want to reduce boot time and want to do things in parallel
 *	(listen to drivers loading while doing other initialization work).
 *
 * This demo has nothing to do with real time and can be used on non real time systems as well.
 *
 * Status of this demo
 * - Currently this demo does not work.
 * - I tried to stop udev using:
 * 	$ systemctl stop systemd-udevd.service
 *	$ systemctl status systemd-udevd.service
 * but to no avail.
 * - Tried to run this as root and still to no avail.
 * - Still need to get this one running.
 */

int main() {
	int netlink_socket=CHECK_NOT_M1(socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT));
	const unsigned int SIZE=1024;
	char buf[SIZE];
	while(true) {
		ssize_t s=CHECK_NOT_M1(read(netlink_socket, buf, SIZE));
		printf("s is %zd\n", s);
	}
	CHECK_NOT_M1(close(netlink_socket));
	return EXIT_SUCCESS;
}
