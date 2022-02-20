/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>	// for socket(2), bind(2), recvfrom(2), connect(2)
#include <sys/socket.h>	// for socket(2), bind(2), recvfrom(2), connect(2)
#include <unistd.h>	// for write(2), close(2)
#include <stdio.h>	// for printf(3), sprintf(3)
#include <string.h>	// for strlen(3)
#include <arpa/inet.h>	// for htons(3)
#include <strings.h>	// for bzero(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()

int main(int argc, char** argv, char** envp) {
	socklen_t fromaddrlen;
	struct sockaddr_in fromaddr;
	// Small fix by RazK: fromaddr initialization
	bzero(&fromaddr, sizeof(fromaddr));
	fromaddr.sin_family=AF_INET;
	fromaddr.sin_addr.s_addr=INADDR_ANY;
	time_t t;
	char ibuffer[1000], obuffer[1000];
	int brsock=CHECK_NOT_M1(socket(AF_INET, SOCK_DGRAM, 0));
	/*
	 * int on=1;
	 * CHECK_NOT_M1(setsockopt(brsock, SOL_SOCKET, SO_BROADCAST, &on, sizeof on));
	 */
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(6969);	// should use getservbyname()

	CHECK_NOT_M1(bind(brsock, (struct sockaddr *) &server, sizeof(server)));
	while(true) {
		ssize_t datalen=CHECK_NOT_M1(recvfrom(brsock, ibuffer, sizeof(ibuffer), 0, (struct sockaddr *) &fromaddr, &fromaddrlen));
		ibuffer[datalen-1]='\0';// get ridd of '\n'
		printf("Got==>%s<==\n", ibuffer);
		sprintf(obuffer, "Bad request\n");
		if(strcmp(ibuffer, "date")==0) {
			t=time(NULL);
			sprintf(obuffer, "%s", ctime(&t));
		}
		if(strcmp(ibuffer, "pid")==0) {
			sprintf(obuffer, "%d\n", getpid());
		}
		int sendsock=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, 0));
		fromaddr.sin_port=htons(6996);	// reply port id
		CHECK_NOT_M1(connect (sendsock, (struct sockaddr *) &fromaddr, fromaddrlen));
		ssize_t ret=CHECK_NOT_M1(write(sendsock, obuffer, strlen(obuffer)));
		CHECK_ASSERT(ret==(ssize_t)strlen(obuffer));
		CHECK_NOT_M1(close(sendsock));
	}
	return EXIT_SUCCESS;
}
