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
#include <sys/types.h>	// for socket(2), bind(2), open(2), listen(2), accept(2), recv(2), setsockopt(2)
#include <sys/socket.h>	// for socket(2), bind(2), listen(2), accept(2), recv(2), setsockopt(2)
#include <strings.h>	// for bzero(3)
#include <arpa/inet.h>	// for ntohs(3)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for read(2), close(2)
#include <netinet/in.h>	// for sockaddr_in
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO(), CHECK_NOT_NULL()
#include <network_utils.h>	// for get_backlog(), print_servent()
#define DO_DEBUG
#include <trace_utils.h>// for DEBUG()

/*
 * This is the most minimal web server you can write using the C language.
 */

const char* my_response=
	"HTTP/1.1 200 OK\n"
	"Content-Type: text/html\n"
	"Connection: Closed\n"
	"\n"
	"<html>\n"
	"<body>\n"
	"<h1>Hello, World!</h1>\n"
	"</body>\n"
	"</html>\n";

int main() {
	// ssize_t datalen;
	// socklen_t fromaddrlen;
	// time_t t;
	// char ibuffer[1000], obuffer[1000];
	//
	unsigned int port=8080;
	DEBUG("contact me at port %d", port);

	// lets get the port number using getservbyname(3)
	// struct servent* p_servent=(struct servent*)CHECK_NOT_NULL(getservbyname(serv_name,serv_proto));
	// print_servent(p_servent);

	// lets open the socket
	int sockfd=CHECK_NOT_M1(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	DEBUG("opened socket with sockfd %d", sockfd);

	// lets make the socket reusable
	int optval=1;
	CHECK_NOT_M1(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));
	DEBUG("setsockopt was ok");

	// lets create the address
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	// server.sin_addr.s_addr=0xff000000;
	// server.sin_port=p_servent->s_port;
	server.sin_port=htons(port);

	// lets bind to the socket to the address
	CHECK_NOT_M1(bind(sockfd, (struct sockaddr *)&server, sizeof(server)));
	DEBUG("bind was successful");

	// lets listen in...
	int backlog=get_backlog();
	DEBUG("backlog is %d\n", backlog);
	CHECK_NOT_M1(listen(sockfd, backlog));
	DEBUG("listen was successful");
	while(true) {
		struct sockaddr_in client;
		socklen_t addrlen;
		int fd=CHECK_NOT_M1(accept(sockfd, (struct sockaddr *)&client, &addrlen));
		DEBUG("accepted fd %d", fd);
		// send a reply on the file descriptor
		CHECK_NOT_M1(write(fd, my_response, strlen(my_response)));
		CHECK_NOT_M1(close(fd));
		sleep(10);
	}
	return EXIT_SUCCESS;
}
