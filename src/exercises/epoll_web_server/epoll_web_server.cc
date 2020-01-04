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
#include <CircularPipe.hh>	// for CircularPipe:Object
#include <sys/timerfd.h>// for timerfd_create(2), timerfd_settime(2), timerfd_gettime(2)

/*
 * This is a solution to the echo server exercise.
 *
 * enable next line to get debug
 * EXTRA_COMPILE_FLAGS_AFTER_DUMMY=-O0 -g3
 */

static inline void register_fd(int realfd, CircularPipe* cp, int epollfd, int op) {
	struct epoll_event ev;
	ev.events=EPOLLRDHUP;
	if(cp->haveData()) {
		ev.events|=EPOLLOUT;
	}
	if(cp->haveRoom()) {
		ev.events|=EPOLLIN;
	}
	ev.data.fd=realfd;
	CHECK_NOT_M1(epoll_ctl(epollfd, op, realfd, &ev));
}

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
	if(argc!=5) {
		fprintf(stderr, "%s: usage: %s [host] [port] [bufsize] [maxevents]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: for example: %s localhost 8080 4096 100\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// get the parameters
	const char* host=argv[1];
	const unsigned int port=atoi(argv[2]);
	const unsigned int bufsize=atoi(argv[3]);
	const unsigned int maxevents=atoi(argv[3]);

	// open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

	// make the socket reusable
	int optval=1;
	CHECK_NOT_M1(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));

	// create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(port);

	// bind to the socket to the address
	CHECK_NOT_M1(bind(sockfd, (struct sockaddr *)&server, sizeof(server)));

	// listen
	int backlog=get_backlog();
	CHECK_NOT_M1(listen(sockfd, backlog));

	// create the epollfd, any value > 0 will do as parameter
	int epollfd=CHECK_NOT_M1(epoll_create(1));

	// add the listening socket to it
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=sockfd;
	CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev));

	// data structures
	std::map<int, CircularPipe*> fdbuffermap;
	std::map<int, int> fdmap;
	std::map<int, int> timermap;

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
				CircularPipe* cp=new CircularPipe(bufsize);
				fdbuffermap[realfd]=cp;
				register_fd(realfd, cp, epollfd, EPOLL_CTL_ADD);
				int timerfd=CHECK_NOT_M1(timerfd_create(CLOCK_REALTIME, 0));
				ev.events=EPOLLIN;
				ev.data.fd=timerfd;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &ev));
				setup_timer(timerfd);
				fdmap[realfd]=timerfd;
				timermap[timerfd]=realfd;
			}
			// can read
			if(fdbuffermap.find(currfd)!=fdbuffermap.end() && events[n].events & EPOLLIN) {
				int realfd=currfd;
				int timerfd=fdmap.find(realfd)->second;
				CircularPipe* cp=fdbuffermap.find(realfd)->second;
				cp->push(realfd);
				register_fd(realfd, cp, epollfd, EPOLL_CTL_MOD);
				// reset the timer on the timerfd
				setup_timer(timerfd);
			}
			// can write
			if(fdbuffermap.find(currfd)!=fdbuffermap.end() && events[n].events & EPOLLOUT) {
				int realfd=currfd;
				CircularPipe* cp=fdbuffermap.find(realfd)->second;
				cp->pull(realfd);
				register_fd(realfd, cp, epollfd, EPOLL_CTL_MOD);
			}
			// disconnect
			if(fdbuffermap.find(currfd)!=fdbuffermap.end() && events[n].events & EPOLLRDHUP) {
				int realfd=currfd;
				int timerfd=fdmap.find(realfd)->second;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, realfd, NULL));
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, timerfd, NULL));
				CHECK_NOT_M1(close(realfd));
				CHECK_NOT_M1(close(timerfd));
				CircularPipe* cp=fdbuffermap.find(realfd)->second;
				delete cp;
				fdbuffermap.erase(realfd);
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
				CircularPipe* cp=fdbuffermap.find(realfd)->second;
				delete cp;
				fdbuffermap.erase(realfd);
				fdmap.erase(realfd);
				timermap.erase(timerfd);
			}
		}
	}
	return EXIT_SUCCESS;
}
