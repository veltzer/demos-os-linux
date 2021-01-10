/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __Selector_hh
#define __Selector_hh

#include <firstinclude.h>
#include <us_helper.h>	// for my_max()
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/select.h>	// for select(2), FD_ZERO(), FD_SET(), FD_ISSET()

/*
 * This is a class that eases the work with the select(2) system call.
 */

class Selector {
private:
	fd_set rfds;
	fd_set wfds;
	int fd_max;

public:
	inline Selector() {
	}
	inline void null() {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		fd_max=0;
	}
	inline void addReadFd(int fd) {
		FD_SET(fd, &rfds);
		fd_max=my_max(fd_max, fd);
	}
	inline void addWriteFd(int fd) {
		FD_SET(fd, &wfds);
		fd_max=my_max(fd_max, fd);
	}
	inline bool isReadActive(int fd) {
		return FD_ISSET(fd, &rfds);
	}
	inline bool isWriteActive(int fd) {
		return FD_ISSET(fd, &wfds);
	}
	inline void doSelect() {
		CHECK_NOT_M1(select(fd_max+1, &rfds, &wfds, NULL, NULL));
	}
};

#endif	/* !__Selector_hh */
