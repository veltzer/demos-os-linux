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
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int main(int argc, char** argv, char** envp) {
	struct sockaddr_in srvr;
	memset(&srvr, 0, sizeof(srvr));
	srvr.sin_family=AF_INET;
	srvr.sin_addr.s_addr=inet_addr("127.0.0.1");
	srvr.sin_port=htons(50000);
	int fd=socket(AF_INET, SOCK_STREAM, 0);
	assert(fd>=0);
	assert(connect(fd, (struct sockaddr*)&srvr, sizeof(srvr))==0);
	char buf[64];
	memset(buf, 0, sizeof(buf));
	int res=write(fd, "uptime\n", 7);
	res++;
	int bc=read(fd, buf, sizeof(buf));
	res=write(1, buf, bc);
	close(fd);
	return EXIT_SUCCESS;
}
