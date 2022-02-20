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
#include <stdio.h>	// for putchar(3), printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows how to read binary data from an attached object file.
 *
 * TODO:
 * - take the code here that reads a whole file and add it to my utils.
 */

extern "C" {
extern char _binary_data_txt_start;
extern char _binary_data_txt_end;
}

int main(int argc, char** argv, char** envp) {
	char buffer[16384];
	int fd=CHECK_NOT_M1(open("data.txt", O_RDONLY));
	char* ptr=buffer;
	size_t left=sizeof(buffer);
	ssize_t read_size=CHECK_NOT_M1(read(fd, ptr, left));
	while(read_size>0) {
		ptr+=read_size;
		left-=read_size;
		read_size=CHECK_NOT_M1(read(fd, ptr, left));
	}
	printf("The next line should say [%s]\n", buffer);
	char *p=&_binary_data_txt_start;
	while(p!=&_binary_data_txt_end)
		putchar(*p++);
	return EXIT_SUCCESS;
}
