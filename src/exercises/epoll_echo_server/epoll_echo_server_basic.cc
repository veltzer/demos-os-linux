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
#include <stdio.h>	// for printf(3)
#include <sys/epoll.h>	// for epoll_create(2), epoll_ctl(2), epoll_wait(2)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for accept4(2)
#include <sys/socket.h>	// for accept4(2)
#include <netinet/in.h>	// for sockaddr_in
#include <unistd.h>	// for read(2), close(2), write(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_IN_RANGE(), CHECK_INT()
#include <network_utils.h>	// for get_backlog()

/*
 * This is a solution to the echo server exercise.
 *
 * NOTES:
 * - events arrive together. One epoll_wait can wake you up on multiple events on the same fd.
 * - EPOLLRDHUP is the event delivered when the other side hangs up.
 * - EPOLLOUT is the event delivered when writing is done. If we work edge triggered then
 * it will be delivered only once. We are supposed to check that the entire write is done.
 * - to stop polling on an fd you must first deregister it from epoll AND ONLY THEN close it (obvious).
 * - we are doing async IO here all over. This means that when you are notified that there is data you
 * want to read fast (without blocking) and then you want to write fast.
 * - this solution relies on the fact that we can write writhout blocking. If we come up short then
 * we crash. This is not really a good idea. See an improvement to this exercise for a better solution.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [host] [port]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* host=argv[1];
	const unsigned int port=atoi(argv[2]);
	printf("contact me at host %s port %d\n", host, port);

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

	// create the epoll fd, any value > 0 will do as parameter
	int epollfd=CHECK_NOT_M1(epoll_create(1));

	// add the listening socket to it
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=sockfd;
	CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev));
	// go into the endless service loop
	while(true) {
		const unsigned int max_events=10;
		struct epoll_event events[max_events];
		int nfds=CHECK_NOT_M1(epoll_wait(epollfd, events, max_events, -1));
		for(int n=0; n<nfds; n++) {
			int currfd=events[n].data.fd;
			// someone is trying to connect
			if(currfd==sockfd) {
				struct sockaddr_in local;
				socklen_t addrlen=sizeof(local);
				int conn_sock=CHECK_NOT_M1(accept4(sockfd, (struct sockaddr*)&local, &addrlen, SOCK_NONBLOCK));
				struct epoll_event ev;
				ev.events=EPOLLIN|EPOLLRDHUP;
				ev.data.fd=conn_sock;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev));
			} else {
				if(events[n].events & EPOLLRDHUP) {
					CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, currfd, NULL));
					CHECK_NOT_M1(close(currfd));
				} else {
					if(events[n].events & EPOLLIN) {
						const int buflen=1024;
						char buffer[buflen];
						ssize_t len=CHECK_NOT_M1(read(currfd, buffer, buflen));
						// PROBLEM: see the problem in the next line?
						// what if we get a shorter write than len or error=-1
						// with errno=EWOULDBLOCK?
						CHECK_INT(write(currfd, buffer, len), len);
					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
