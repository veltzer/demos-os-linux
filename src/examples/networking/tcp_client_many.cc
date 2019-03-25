/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>	// for socket(2), recv(2), connect(2), send(2)
#include <sys/socket.h>	// for socket(2), recv(2), connect(2), send(2), inet_addr(3)
#include <strings.h>	// for bzero(3)
#include <stdio.h>	// for printf(3), atoi(3), snprintf(3)
#include <stdlib.h>	// for exit(3), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>	// for close(2)
#include <sys/un.h>	// for sockaddr_un
#include <string.h>	// for strlen(3)
#include <netinet/in.h>	// for sockaddr_in, inet_addr(3)
#include <arpa/inet.h>	// for inet_addr(3)
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO()

/*
 * This is a tcp client demo.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

unsigned int numrequests;
unsigned int port;
char* host;

void* worker(void* arg) {
	// get the thread number
	int threadid=*(int*)arg;
	// lets open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	printf("%d: opened socket with sockfd %d\n", threadid, sockfd);

	// lets create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	// server.sin_addr.s_addr=INADDR_ANY;
	server.sin_addr.s_addr=inet_addr(host);
	server.sin_port=htons(port);

	// lets connect...
	CHECK_NOT_M1(connect(sockfd, (struct sockaddr *)&server, sizeof(server)));
	printf("%d: connected successfully\n", threadid);
	for(unsigned int i=0; i<numrequests; i++) {
		// lets send "hello"...
		const char* sbuf="hello";
		unsigned int sbuflen=strlen(sbuf);
		printf("%d: sending [%s]\n", threadid, sbuf);
		CHECK_NOT_M1(send(sockfd, sbuf, sbuflen, 0));

		// lets receive
		unsigned int rbuflen=getpagesize();
		char rbuf[rbuflen];
		int ret=CHECK_NOT_M1(recv(sockfd, rbuf, rbuflen, 0));
		char prbuf[rbuflen+1];
		snprintf(prbuf, ret+1, "%s", rbuf);
		printf("%d: received [%s]\n", threadid, prbuf);
	}
	// lets close...
	CHECK_NOT_M1(close(sockfd));
	printf("%d: closed, exiting\n", threadid);

	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=5) {
		fprintf(stderr, "%s: usage: %s [numthreads] [numrequests] [host] [port]\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	// lets take the parameters
	unsigned int numthreads=atoi(argv[1]);
	numrequests=atoi(argv[2]);
	host=argv[3];
	port=atoi(argv[4]);

	// print the parameters
	printf("numthreads is %d\n", numthreads);
	printf("numrequests is %d\n", numrequests);
	printf("host is %s\n", host);
	printf("port is %d\n", port);

	pthread_t* threads=new pthread_t[numthreads];
	for(unsigned int i=0; i<numthreads; i++) {
		int* p=new int(i);
		CHECK_ZERO_ERRNO(pthread_create(&threads[i], NULL, worker, p));
	}
	for(unsigned int i=0; i<numthreads; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	return EXIT_SUCCESS;
}
