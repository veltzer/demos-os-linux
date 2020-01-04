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
#include <netinet/in.h>	// for inet_ntoa(3)
#include <arpa/inet.h>	// for inet_ntoa(3)
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for read(2), close(2)
#include <sys/types.h>	// for socket(2), setsockopt(2), listen(2), bind(2), accept(2), sendto(2)
#include <sys/socket.h>	// for socket(2), setsockopt(2), listen(2), bind(2), accept(2), sendto(2), inet_ntoa(3)
#include <strings.h>	// for bzero(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

int main(int argc, char** argv, char** envp) {
	int on, got;
	socklen_t rplyFromAddressLen;
	struct sockaddr_in braddress, rplyFromAddress, myAddress;
	char ibuffer[1000], obuffer[1000];
	struct timeval timeout;
	fd_set readfds;

	int brsock=CHECK_NOT_M1(socket(AF_INET, SOCK_DGRAM, 0));

	bzero(&braddress, sizeof(braddress));
	braddress.sin_family=AF_INET;
	braddress.sin_addr.s_addr=INADDR_BROADCAST;
	braddress.sin_port=htons(6969);	// should use getservbyname()

	on=1;
	CHECK_NOT_M1(setsockopt(brsock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)));
	int rplysock=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, 0));

	bzero(&myAddress, sizeof(myAddress));
	myAddress.sin_family=AF_INET;
	myAddress.sin_port=htons(6996);	// reply port id
	myAddress.sin_addr.s_addr=INADDR_ANY;
	CHECK_NOT_M1(bind(rplysock, (struct sockaddr *) &myAddress, sizeof(myAddress)));
	CHECK_NOT_M1(listen(rplysock, SOMAXCONN));

	printf("Type your request: ");
	while(fgets(obuffer, sizeof(obuffer), stdin)) {
		// this will show you the limit of fd_set and select(2) in general...
		// printf("size is %d\n",sizeof(readfds));
		CHECK_NOT_M1(sendto(brsock, obuffer, strlen(obuffer), 0, (struct sockaddr *) &braddress, sizeof(braddress)));
		printf("Request sent\n");
		timeout.tv_sec=5;
		timeout.tv_usec=0;
		FD_ZERO(&readfds);
		FD_SET(rplysock, &readfds);
		int nfds=CHECK_NOT_M1(select(rplysock+1, &readfds, NULL, NULL, &timeout));
		while(nfds>0) {
			int newsock=CHECK_NOT_M1(accept(rplysock, (struct sockaddr *) &rplyFromAddress,
					&rplyFromAddressLen));
			got=CHECK_NOT_M1(read(newsock, ibuffer, sizeof(ibuffer)));
			while(got>0) {
				ibuffer[got-1]='\0';	// get rid of '\n'
				printf("Reply from %s received: %s\n",
					inet_ntoa(rplyFromAddress.sin_addr), ibuffer);
				got=CHECK_NOT_M1(read(newsock, ibuffer, sizeof(ibuffer)));
			}
			CHECK_NOT_M1(close(newsock));
			nfds=CHECK_NOT_M1(select(rplysock+1, &readfds, NULL, NULL, &timeout));
		}
		if(nfds==0) {
			printf("Replies finished on timeout\n");
		}
		FD_SET(rplysock, &readfds);
		printf("Type your request: ");
	}
	printf("\n");
	CHECK_NOT_M1(close(rplysock));
	CHECK_NOT_M1(close(brsock));
	return EXIT_SUCCESS;
}
