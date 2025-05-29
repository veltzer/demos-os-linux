/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3), fprintf(3)
#include <sys/epoll.h>	// for epoll_create(2), epoll_ctl(2), epoll_wait(2)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for accept4(2)
#include <sys/socket.h>	// for accept4(2)
#include <netinet/in.h>	// for sockaddr_in
#include <unistd.h>	// for read(2), close(2), write(2), getpid(3)
#include <sys/signalfd.h>	// for signalfd(2), signalfd_siginfo
#include <signal.h>	// for sigset_t, sigemptyset(3), sigaddset(3), sigprocmask(2)
#include <string.h>	// for strsignal(3)
#include <assert.h>	// for assert(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_IN_RANGE(), CHECK_INT()
#include <network_utils.h>	// for get_backlog()
#include <epoll_utils.h>// for print_events()
// #define DO_DEBUG
#include <trace_utils.h>// for DEBUG()

/*
 * This is an example of using the epoll(2) API to write an echo server using
 * just a single thread. You can test it using telnet.
 *
 * The server also listens in on signal to enable clean shutdown of the server
 * when receiving SIGUSR1.
 *
 * NOTES:
 * - events arrive together. One epoll_wait can wake you up on multiple events on the same fd.
 * - EPOLLRDHUP is the event delivered when the other side hangs up.
 * - EPOLLOUT is the event delivered writing is possible.
 * If we work edge triggered then it will be delivered only once.
 * We are supposed to check that the entire write is done.
 * - to stop polling on an fd you must first deregister it from epoll AND ONLY THEN close it (obvious).
 * - we are doing async IO here all over. This means that when you are notified that there is data you
 * to read fast (without blocking) and then you get to write fast.
 * - the DEBUG are disabled since the printing to the console could be blocking.
 * enable via DO_DEBUG above.
 *
 * TODO:
 * - check what happens when we write large amounts of data to the output. Will the async write come up short?
 */

int main(int argc, char** argv) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [port]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const unsigned int port=atoi(argv[1]);
	printf("contact me using [telnet localhost %d]\n", port);

	// lets open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	printf("opened socket with sockfd [%d]\n", sockfd);

	// lets make the socket reusable
	int optval=1;
	CHECK_NOT_M1(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));
	printf("setsockopt was ok\n");

	// lets create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(port);

	// lets bind to the socket to the address
	CHECK_NOT_M1(bind(sockfd, (struct sockaddr *)&server, sizeof(server)));
	printf("bind was successful\n");

	// lets listen in
	int backlog=get_backlog();
	printf("backlog is [%d]\n", backlog);
	CHECK_NOT_M1(listen(sockfd, backlog));
	printf("listen was successful\n");

	// create the epoll fd
	// the parameter to epoll_create(2) really doesn't matter
	// see the documentation for more details
	int epollfd=CHECK_NOT_M1(epoll_create1(0));

	// add the listening socket to it
	struct epoll_event sockev;
	sockev.events=EPOLLIN;
	sockev.data.fd=sockfd;
	CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &sockev));

	// add a signal fd to the mix
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	// make sure that the signals we want to handle do not go to their
	// default handlers...
	CHECK_NOT_M1(sigprocmask(SIG_BLOCK, &mask, NULL));
	int sigfd=CHECK_NOT_M1(signalfd(-1, &mask, 0));
	printf("send SIGUSR1 to me to shut me down using [kill -s SIGUSR1 %d]\n", getpid());
	struct epoll_event sigev;
	sigev.events=EPOLLIN;
	sigev.data.fd=sigfd;
	CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, sigfd, &sigev));

	// go into the endless service loop
	bool over=false;
	while(!over) {
		const unsigned int max_events=10;
		struct epoll_event events[max_events];
		int nfds=CHECK_NOT_M1(epoll_wait(epollfd, events, max_events, -1));
		for(int n=0; n<nfds; n++) {
			if(events[n].data.fd==sockfd) {
				struct sockaddr_in local;
				socklen_t addrlen;
				int conn_sock=CHECK_NOT_M1(accept4(sockfd, (struct sockaddr*)&local, &addrlen, SOCK_NONBLOCK));
				struct epoll_event ev;
				ev.events=EPOLLIN|EPOLLET|EPOLLOUT|EPOLLRDHUP;
				ev.data.fd=conn_sock;
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev));
				continue;
			}
			if(events[n].data.fd==sigfd) {
				// we must read the data, otherwise we will be bothered with this signal again
				// and again
				struct signalfd_siginfo fdsi;
				CHECK_INT(read(sigfd, &fdsi, sizeof(struct signalfd_siginfo)), sizeof(struct signalfd_siginfo));
				DEBUG("got signal %d (%s)\n", fdsi.ssi_signo, strsignal(fdsi.ssi_signo));

				over=true;
				continue;
			}
			// this is regular IO handling
			DEBUG("got activity on fd %d", events[n].data.fd);
			char printbuff[1024];
			print_events(printbuff, 1024, events[n].events);
			DEBUG("got events %s", printbuff);
			if(events[n].events & EPOLLIN) {
				const int buflen=1024;
				char buffer[buflen];
				int fd=events[n].data.fd;
				ssize_t len=CHECK_NOT_M1(read(fd, buffer, buflen));
				/*
				 * TODO:
				 * - how do we know that we do not block here?
				 */
				CHECK_INT(write(fd, buffer, len), len);
				DEBUG("read %zd bytes and wrote %zd bytes", len, len);
			}
			if(events[n].events & EPOLLRDHUP) {
				int fd=events[n].data.fd;
				DEBUG("closing the fd %d", fd);
				CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL));
				CHECK_NOT_M1(close(fd));
			}
		}
	}
	CHECK_NOT_M1(close(epollfd));
	CHECK_NOT_M1(close(sockfd));
	CHECK_NOT_M1(close(sigfd));
	return EXIT_SUCCESS;
}
