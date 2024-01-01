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
#include <stdio.h>	// for printf(3), fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <string.h>	// for strlen(3)
#include <unistd.h>	// for sleep(3), read(2), write(2), fsync(2)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <trace_utils.h>// for INFO()
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_INT(), CHECK_ZERO()

/*
 * This example shows that you can continue to use a file after it has
 * been removed from the hard disk. So: as long as a process is using
 * a file its content will not be deleted from the hard drive.
 *
 * The example creates a file, puts content to it and proceeds to read
 * the content every second. While this is going on you can remove
 * the file from the command line and see the program continue to
 * run.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [filename] [content]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /tmp/foo mycontent\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* filename=argv[1];
	const char* content=argv[2];
	size_t len=strlen(content);
	INFO("creating file [%s]", filename);
	int fd=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_INT(write(fd, content, len), len);
	CHECK_NOT_M1(fsync(fd));
	INFO("file [%s] is ready, see it, and delete it while I am running...", filename);
	char* buffer=new char[len+1];
	buffer[len]='\0';
	unsigned int counter=0;
	while(true) {
		CHECK_NOT_M1(lseek(fd, 0, SEEK_SET));
		CHECK_INT(read(fd, buffer, len), len);
		printf("content is %s, counter is %d\n", buffer, counter);
		counter++;
		CHECK_ZERO(sleep(1));
	}
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
