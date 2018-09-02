/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <aio.h>// for aio_read(3), aio_suspend(3), aio_return(3)
#include <strings.h>	// for bzero(3)
#include <stdlib.h>	// for malloc(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL(), CHECK_ZERO()

/*
 * This is an example of asynchroneous IO.
 *
 * Note that you must link with -lrt or this example will not work.
 *
 * References:
 * man aio.h
 * http://www.ibm.com/developerworks/linux/library/l-async/
 *
 * EXTRA_LINK_FLAGS=-lrt
 */

int main(int argc, char** argv, char** envp) {
	const int BUFSIZE=1024;
	int fd=CHECK_NOT_M1(open("/etc/passwd", O_RDONLY));
	/* Allocate a data buffer for the aiocb request */
	void* ptr=CHECK_NOT_NULL(malloc(BUFSIZE+1));
	struct aiocb my_aiocb;
	/* Zero out the aiocb structure (recommended) */
	bzero(&my_aiocb, sizeof(struct aiocb));
	/* Initialize the necessary fields in the aiocb */
	my_aiocb.aio_buf=ptr;
	my_aiocb.aio_fildes=fd;
	my_aiocb.aio_nbytes=BUFSIZE;
	my_aiocb.aio_offset=0;
	CHECK_ZERO(aio_read(&my_aiocb));
	// busy wait (bad! - but you can use this API for peeking once in a while)
	// while(aio_error(&my_aiocb)==EINPROGRESS);
	// sleeping wait via API
	const struct aiocb * const cblist[]={ &my_aiocb };
	CHECK_ZERO(aio_suspend(cblist, 1, NULL));
	int ret=CHECK_NOT_M1(aio_return(&my_aiocb));
	printf("got ret of %d\n", ret);
	char* buf=(char*)my_aiocb.aio_buf;
	buf[ret]='\0';
	printf("%s\n", buf);
	return EXIT_SUCCESS;
}
