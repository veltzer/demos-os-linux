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
#include <network_utils.h>	// for get_backlog(), print_servent()
#include <trace_utils.h>// for TRACE()
#include <pthread_utils.h>	// for gettid(2)

/*
 * This is a demo of a simple echo socket server implementation in pure C
 * in Linux
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

// const unsigned int port=7000;
const char* serv_name="http-alt";
const char* serv_proto="tcp";

void *worker(void* arg) {
	int fd=*((int*)arg);
	TRACE("thread %d starting", gettid());
	TRACE("thread %d got fd %d", gettid(), fd);
	const unsigned int buflen=1024;
	char buff[buflen];
	char prbuff[buflen+1];
	ssize_t res=CHECK_NOT_M1(recv(fd, buff, buflen, 0));
	snprintf(prbuff, res+1, "%s", buff);
	TRACE("thread %d received %s", gettid(), prbuff);
	while(res!=0) {
		TRACE("thread %d sending %s", gettid(), buff);
		CHECK_NOT_M1(send(fd, buff, res, 0));
		res=CHECK_NOT_M1(recv(fd, buff, buflen, 0));
		snprintf(prbuff, res+1, "%s", buff);
		TRACE("thread %d received %s", gettid(), prbuff);
	}
	CHECK_NOT_M1(shutdown(fd, SHUT_RDWR));
	CHECK_NOT_M1(close(fd));
	TRACE("thread %d ending", gettid());
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [host] [port]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* host=argv[1];
	const unsigned int port=atoi(argv[2]);
	printf("contact me at host %s port %d\n", host, port);

	// lets get the port number using getservbyname(3)
	// struct servent* p_servent=(struct servent*)CHECK_NOT_NULL(getservbyname(serv_name,serv_proto));
	// print_servent(p_servent);

	// lets open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	printf("opened socket with sockfd %d\n", sockfd);

	// lets make the socket reusable
	int optval=1;
	CHECK_NOT_M1(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));
	printf("setsockopt was ok\n");

	// lets create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	// server.sin_addr.s_addr=inet_addr(host);
	server.sin_addr.s_addr=INADDR_ANY;
	// server.sin_addr.s_addr=0xff000000;
	// server.sin_port=p_servent->s_port;
	server.sin_port=htons(port);

	// lets bind to the socket to the address
	CHECK_NOT_M1(bind(sockfd, (struct sockaddr *)&server, sizeof(server)));
	printf("bind was successful\n");

	// lets listen in...
	int backlog=get_backlog();
	printf("backlog is %d\n", backlog);
	CHECK_NOT_M1(listen(sockfd, backlog));
	printf("listen was successful\n");
	while(true) {
		struct sockaddr_in client;
		socklen_t addrlen;
		int fd=CHECK_NOT_M1(accept(sockfd, (struct sockaddr *)&client, &addrlen));
		printf("accepted fd %d\n", fd);
		// spawn a thread to handle the connection to that client...
		pthread_t thread;
		int* p=new int(fd);
		CHECK_ZERO_ERRNO(pthread_create(&thread, NULL, worker, p));
	}
	return EXIT_SUCCESS;
}
