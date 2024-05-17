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
#include <sys/types.h>	// for socket(2), bind(2), listen(2), accept(2), recv(2), setsockopt(2)
#include <sys/socket.h>	// for socket(2), bind(2), listen(2), accept(2), recv(2), setsockopt(2), inet_addr(3)
#include <strings.h>	// for bzero(3)
#include <stdio.h>	// for printf(3), atoi(3)
#include <netdb.h>	// for getservbyname(3)
#include <arpa/inet.h>	// for ntohs(3)
#include <unistd.h>	// for read(2), close(2)
#include <pthread.h>	// for pthread_create(3)
#include <netinet/in.h>	// for sockaddr_in, inet_addr(3)
#include <arpa/inet.h>	// for inet_addr(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO(), CHECK_NOT_NULL()
#include <us_helper.h>	// for my_max()
#include <network_utils.h>	// for get_backlog(), print_servent()
#include <Selector.hh>	// for Selector:Object

/*
 * This is a tcp server listening on two tcp ports using select(2)
 * and pthreads.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

// const unsigned int port=7000;
// const char* serv_name="http-alt";
// const char* serv_proto="tcp";

void *worker(void* arg) {
	int fd=*((int*)arg);
	const unsigned int buflen=1024;
	char buff[buflen];
	ssize_t res;
	do {
		res=CHECK_NOT_M1(recv(fd, buff, buflen, 0));
		ssize_t len=res;
		char* ptr=buff;
		while(len>0) {
			ssize_t curr=CHECK_NOT_M1(send(fd, ptr, len, 0));
			ptr+=curr;
			len-=curr;
		}
	} while(res>0);
	CHECK_NOT_M1(shutdown(fd, SHUT_RDWR));
	CHECK_NOT_M1(close(fd));
	return NULL;
}

int main(int argc, char** argv) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [port1] [port2]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const unsigned int port1=atoi(argv[1]);
	const unsigned int port2=atoi(argv[2]);
	printf("contact me at localhost:%d\n", port1);
	printf("contact me at localhost:%d\n", port2);

	// lets open the socket
	int sockfd1=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	int sockfd2=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

	// lets make the socket reusable
	int optval=1;
	CHECK_NOT_M1(setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));
	CHECK_NOT_M1(setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));

	// lets create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;

	// lets bind to the socket to the address
	server.sin_port=htons(port1);
	CHECK_NOT_M1(bind(sockfd1, (struct sockaddr *)&server, sizeof(server)));
	server.sin_port=htons(port2);
	CHECK_NOT_M1(bind(sockfd2, (struct sockaddr *)&server, sizeof(server)));

	// lets listen in...
	int backlog=get_backlog();
	CHECK_NOT_M1(listen(sockfd1, backlog));
	CHECK_NOT_M1(listen(sockfd2, backlog));
	Selector s;
	while(true) {
		// lets select(2)
		s.null();
		s.addReadFd(sockfd1);
		s.addReadFd(sockfd2);
		s.doSelect();
		struct sockaddr_in client;
		// address length must be properly initialised
		// or the call to accept(2) will fail...
		socklen_t addrlen=sizeof(client);
		if(s.isReadActive(sockfd1)) {
			int fd=CHECK_NOT_M1(accept(sockfd1, (struct sockaddr *)&client, &addrlen));
			// spawn a thread to handle the connection to that client...
			pthread_t thread;
			int* p=new int(fd);
			CHECK_ZERO_ERRNO(pthread_create(&thread, NULL, worker, p));
		}
		if(s.isReadActive(sockfd2)) {
			int fd=CHECK_NOT_M1(accept(sockfd2, (struct sockaddr *)&client, &addrlen));
			// spawn a thread to handle the connection to that client...
			pthread_t thread;
			int* p=new int(fd);
			CHECK_ZERO_ERRNO(pthread_create(&thread, NULL, worker, p));
		}
	}
	return EXIT_SUCCESS;
}
