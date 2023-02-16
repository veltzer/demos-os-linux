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
#include <sys/types.h>	// for socket(2), recv(2), connect(2), send(2), open(2)
#include <sys/stat.h>	// for open(2)
#include <sys/socket.h>	// for socket(2), recv(2), connect(2), send(2), inet_addr(3)
#include <strings.h>	// for bzero(3)
#include <stdio.h>	// for printf(3), atoi(3), snprintf(3)
#include <stdlib.h>	// for EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>	// for close(2)
#include <sys/un.h>	// for sockaddr_un
#include <string.h>	// for strlen(3)
#include <netinet/in.h>	// for sockaddr_in, inet_addr(3)
#include <arpa/inet.h>	// for inet_addr(3)
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <trace_utils.h>// for TRACE()

/*
 * This is udp client demo that sends a file to a udp port.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [host] [port] [file]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* host=argv[1];
	const unsigned int port=atoi(argv[2]);
	const char* file=argv[3];

	// lets open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
	TRACE("opened socket with sockfd %d", sockfd);

	// lets create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	// server.sin_addr.s_addr=inet_addr(host);
	// server.sin_port=htons(port);

	// lets bind...
	CHECK_NOT_M1(bind(sockfd, (struct sockaddr *)&server, sizeof(server)));
	TRACE("binded successfully");

	// lets create the address
	struct sockaddr_in peer_addr;
	bzero(&peer_addr, sizeof(peer_addr));
	peer_addr.sin_family=AF_INET;
	// peer_addr.sin_addr.s_addr=INADDR_ANY;
	peer_addr.sin_addr.s_addr=inet_addr(host);
	peer_addr.sin_port=htons(port);

	// lets send
	int fd=CHECK_NOT_M1(open(file, O_RDONLY));
	int ret;
	unsigned int buflen=getpagesize();
	char buf[buflen];
	while((ret=read(fd, buf, buflen))>0) {
		TRACE("read %d bytes", ret);
		int bytes=ret;
		char* pbuf=buf;
		while(bytes>0) {
			int written=CHECK_NOT_M1(sendto(sockfd, buf, bytes, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr)));
			TRACE("sendto %d bytes", written);
			bytes-=written;
			pbuf+=written;
		}
	}
	CHECK_NOT_M1(ret);
	CHECK_NOT_M1(close(fd));
	TRACE("closed the file");
	CHECK_NOT_M1(close(sockfd));
	TRACE("closed the socket");

	TRACE("exiting");
	return EXIT_SUCCESS;
}
