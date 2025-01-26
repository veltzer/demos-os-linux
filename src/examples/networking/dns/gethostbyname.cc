/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	struct hostent *host;
	struct in_addr **addr_list;
	int i;

	if (argc != 2) {
		printf("%s: usage: %s hostname\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}

	// Get host by name
	if ((host = gethostbyname(argv[1])) == NULL) {
		herror("gethostbyname failed");
		return 2;
	}

	// Cast the h_addr_list to in_addr since h_addr_list also has the ip address in long format
	addr_list = (struct in_addr **)host->h_addr_list;

	// Print all IP addresses for this hostname
	for(i = 0; addr_list[i] != NULL; i++) {
		printf("IP address %d: %s\n", i+1, inet_ntoa(*addr_list[i]));
	}

	return EXIT_SUCCESS;
}
