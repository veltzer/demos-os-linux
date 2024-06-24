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

#pragma once

#include <firstinclude.h>
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()
#include <unistd.h>	// for read(2), write(2)
#include <stdlib.h>	// for malloc(3), free(3)

/*
 * This is a circular pipe class. It holds a buffer and handles two circular
 * pointers to it.
 *
 * Data already in this pipe starts with pos_read and upto pos_write. So reading
 * from the pipe is from pos_read and writing to it is from pos_write.
 */

// release the next line if you want to debug the pipe...
// #define PIPE_DEBUG

class CircularPipe{
private:
	size_t size;
	char* buf;
	size_t pos_read;
	size_t pos_write;

public:
	inline CircularPipe(const size_t isize) {
		size=isize;
		buf=(char*)CHECK_NOT_NULL(malloc(size));
		pos_read=0;
		pos_write=0;
	}
	inline~CircularPipe() {
		free((void*)buf);
	}
	/* return the occupied room of a pipe */
	inline size_t data() {
		if (pos_read <= pos_write)
			return pos_write-pos_read;
		else
			return size-pos_read+pos_write;
	}
	/* return the empty room of a pipe */
	inline size_t room() {
		if (pos_read <= pos_write)
			return size-pos_write+pos_read-1;
		else
			return pos_read-pos_write-1;
	}
	inline bool haveData() {
		return data()>0;
	}
	inline bool haveRoom() {
		return room()>0;
	}
	inline bool canPush() {
		return haveRoom();
	}
	inline bool canPull() {
		return haveData();
	}
	/* read data into the pipe (at pos_read) */
	inline bool push(int fd) {
		size_t count;
		if (pos_read <= pos_write) {
			count = size-pos_write;
			if (pos_read==0) {
				count--;
			}
		} else {
			count = pos_read-pos_write-1;
		}
		ssize_t len=CHECK_NOT_M1(read(fd, buf+pos_write, count));
#ifdef PIPE_DEBUG
		printf("read: rp=%d, wp=%d, c=%d, l=%d\n", pos_read, pos_write, count, len);
#endif	/* PIPE_DEBUG */
		pos_write+=len;
		pos_write%=size;
		return len==0;
	}
	/* write data from the pipe (at pos_write) */
	inline void pull(int fd) {
		// printf("pos_read is %d\n",pos_read);
		// printf("pos_write is %d\n",pos_write);
		size_t count;
		if (pos_read <= pos_write) {
			count = pos_write-pos_read;
		} else {
			count = size-pos_read;
		}
		ssize_t len=CHECK_NOT_M1(write(fd, buf+pos_read, count));
#ifdef PIPE_DEBUG
		printf("write: rp=%d, wp=%d, c=%d, l=%d\n", pos_read, pos_write, count, len);
#endif	/* PIPE_DEBUG */
		pos_read+=len;
		pos_read%=size;
	}
};
