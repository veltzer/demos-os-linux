/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/epoll.h>	// for epoll_create(2), epoll_ctl(2), epoll_wait(2)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for accept4(2)
#include <sys/socket.h>	// for accept4(2)
#include <netinet/in.h>	// for sockaddr_in
#include <unistd.h>	// for read(2), close(2), write(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_IN_RANGE(), CHECK_INT()
#include <network_utils.h>	// for get_backlog()
#include <map>	// for std::map<T1,T2>, std::map<T1,T2>::iterator
#include <sys/timerfd.h>// for timerfd_create(2), timerfd_settime(2), timerfd_gettime(2)

/*
 * This is a solution to the echo server exercise with timeouts for connections.
 *
 * This solution does not handle writes correctly.
 */

static inline void setup_timer(int timerfd) {
	struct itimerspec new_value;
	new_value.it_value.tv_sec=10;
	new_value.it_value.tv_nsec=0;
	new_value.it_interval.tv_sec=0;
	new_value.it_interval.tv_nsec=0;
	// dont need the old timer value
	// 0 in the flags means relative time
	CHECK_NOT_M1(timerfd_settime(timerfd, 0, &new_value, NULL));
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [host] [port] [maxevents]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* host=argv[1];
	const unsigned int port=atoi(argv[2]);
	const unsigned int maxevents=atoi(argv[3]);

	// data structures
	std::map<int, int> fdmap;
	std::map<int, int> timermap;

	// lets open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

	// lets make the socket reusable
	int optval=1;
	CHECK_NOT_M1(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));

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

	// lets listen in...
	int backlog=get_backlog();
	CHECK_NOT_M1(listen(sockfd, backlog));

	// create the epoll fd, any value > 0 will do as parameter
	int epollfd=CHECK_NOT_M1(epoll_create(1));

	// add the listening socket to it
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=sockfd;
	CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev));

	// message to the user
	printf("contact me at host %s port %d\n", host, port);
	// go into the endless service loop
	while(true) {
		struct epoll_event events[maxevents];
		int nfds=CHECK_NOT_M1(epoll_wait(epollfd, events, maxevents, -1));
		for(int n=0; n<nfds; n++) {
			int currfd=events[n].data.fd;
			// connect
			if(currfd==sockfd) {
				struct sockaddr_in local;
				socklen_t addrlen=sizeof(local);
				int realfd=CHECK_NOT_M1(accept4(sockfd, (struct sockaddr*)&local, &addrlen, SOCK_NONBLOCK));
				struct epoll_event ev;
				ev.events=EPOLLIN|EPOLLRDHUP;
				ev.data.fd=realfd;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, realfd, &ev));
				int timerfd=CHECK_NOT_M1(timerfd_create(CLOCK_REALTIME, 0));
				ev.events=EPOLLIN;
				ev.data.fd=timerfd;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &ev));
				setup_timer(timerfd);
				fdmap[realfd]=timerfd;
				timermap[timerfd]=realfd;
			}
			// can read
			if(fdmap.find(currfd)!=fdmap.end() && events[n].events & EPOLLIN) {
				int realfd=currfd;
				int timerfd=fdmap.find(realfd)->second;
				const int buflen=1024;
				char buffer[buflen];
				ssize_t len=CHECK_NOT_M1(read(realfd, buffer, buflen));
				// TODO: handle short writes!
				CHECK_INT(write(realfd, buffer, len), len);
				// reset the timer on the timerfd
				setup_timer(timerfd);
			}
			// disconnect
			if(fdmap.find(currfd)!=fdmap.end() && events[n].events & EPOLLRDHUP) {
				int realfd=currfd;
				int timerfd=fdmap.find(realfd)->second;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, realfd, NULL));
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, timerfd, NULL));
				CHECK_NOT_M1(close(realfd));
				CHECK_NOT_M1(close(timerfd));
				fdmap.erase(realfd);
				timermap.erase(timerfd);
			}
			// timeout
			if(timermap.find(currfd)!=timermap.end()) {
				int timerfd=currfd;
				int realfd=timermap.find(timerfd)->second;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, realfd, NULL));
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, timerfd, NULL));
				CHECK_NOT_M1(close(realfd));
				CHECK_NOT_M1(close(timerfd));
				fdmap.erase(realfd);
				timermap.erase(timerfd);
			}
		}
	}
	return EXIT_SUCCESS;
}
