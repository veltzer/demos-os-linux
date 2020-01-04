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

#include <firstinclude.h>
#include <sys/types.h>	// for socket(2)
#include <sys/socket.h>	// for socket(2)
#include <stdio.h>	// for printf(3), fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for read(2)
#include <netinet/ip.h>	// for struct iphdr
#include <netinet/tcp.h>// for struct tcphdr
#include <netinet/in.h>	// for IPPROTO_TCP
#include <unistd.h>	// for geteuid(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <security_utils.h>	// for check_root()

/*
 * An example of a sniffer
 *
 * References:
 * http://www.tenouk.com/Module42a.html
 */

int main(int argc, char** argv, char** envp) {
	// lets check that we are running as root
	check_root();
	int fd=CHECK_NOT_M1(socket(PF_INET, SOCK_RAW, IPPROTO_TCP));
	/* single packets are usually not bigger than 8192 bytes but
	 * depend on the media standard of the Network Access layer such as
	 * Ethernet, Token Ring etc
	 */
	const unsigned int length=8192;
	char buffer[length];
	// struct ipheader *ip=(struct ipheader *) buffer;
	// struct udpheader *udp=(struct udpheader *) (buffer + sizeof(struct ipheader));
	while(read(fd, buffer, length) > 0) {
		/* packet=data + ip header + tcp header
		 * Little Endian/Big Endian must be considered here */
		printf("Dump the packet: %s\n", buffer + sizeof(struct iphdr) + sizeof(struct tcphdr));
	}
	return EXIT_SUCCESS;
}
