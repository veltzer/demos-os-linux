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
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
	int fd=open("/proc/uptime", O_RDONLY);
	assert(fd!=-1);
	while(1) {
		size_t bufsize=256;
		char buf[bufsize];
		ssize_t count_read=read(fd, buf, bufsize);
		printf("count_read is %ld\n", count_read);
		assert(count_read>0);
		printf("we got [%s]\n", buf);
		int ret=lseek(fd, SEEK_SET, 0);
		assert(ret!=-1);
	}
}
