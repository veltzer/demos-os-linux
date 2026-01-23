/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/socket.h>	// for socket(2)
#include <arpa/inet.h>
#include <stdio.h>

/*
 * This is a tcp client demo.
 */

int main() {
	int fd=CHECK_NOT_M1(socket(AF_PACKET, SOCK_RAW, htons(0x88B5)));
	printf("fd is %d\n", fd);
	return EXIT_SUCCESS;
}
