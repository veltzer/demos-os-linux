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
#include <sys/socket.h>	// for inet_ntoa(3)
#include <sys/types.h>	// for getifaddrs(3)
#include <ifaddrs.h>	// for getifaddrs(3)
#include <stdio.h>	// for printf(3)
#include <arpa/inet.h>	// for AF_INET, sockaddr_in, inet_ntoa(3), inet_ntop(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <netinet/in.h>	// for inet_ntoa(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <network_utils.h>	// for domain_to_str()

/*
 * This example shows how to get the ips of the current machine.
 *
 * Note:
 * - this example is called "get_ips" for a reason and that is that a machine
 * does not have an ip (singular) but rather ips (plural).
 * - notice that this example could be used as the basis of getting all kinds
 * of information about the currently configured interfaces of a machine.
 */

int main(int argc, char **argv, char** envp) {
	struct ifaddrs *id;
	CHECK_NOT_M1(getifaddrs(&id));
	while (id->ifa_next != NULL) {
		id = id->ifa_next;
		printf ("interface: %s\n", id->ifa_name);
		if (id->ifa_addr != NULL) {
			char afbuf[256];
			domain_to_str(id->ifa_addr->sa_family, afbuf, sizeof(afbuf));
			printf ("\ttype %s\n", afbuf);
			if (id->ifa_addr->sa_family == AF_INET || id->ifa_addr->sa_family == AF_INET6) {
				void* sin;
				if (id->ifa_addr->sa_family == AF_INET) {
					sin = &(((struct sockaddr_in *)(id->ifa_addr))->sin_addr);
				} else {
					sin = &(((struct sockaddr_in6 *)(id->ifa_addr))->sin6_addr);
				}
				char buf[256];
				CHECK_NOT_NULL_CONST(inet_ntop(id->ifa_addr->sa_family, sin, buf, sizeof(buf)));
				printf ("\t%d ip %s\n", id->ifa_addr->sa_family, buf);
			} else {
				printf ("\tis not AF_INET or AF_INET6\n");
			}
		} else {
			printf ("\tis without address\n");
		}
	}
	return EXIT_SUCCESS;
}
