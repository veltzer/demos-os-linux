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

#pragma once

/*
 * This is a helper file for dealing with the linux epoll(2) API
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <err_utils.h>	// for CHECK_IN_RANGE()
#include <sys/epoll.h>	// for EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, ...
#include <stdio.h>	// for snprintf(3)

typedef struct _epoll_val_and_name{
	unsigned int val;
	const char* name;
} epoll_val_and_name;

#define entry(x) { x, # x }
static epoll_val_and_name epoll_tbl[]={
	entry(EPOLLIN),
	entry(EPOLLOUT),
	entry(EPOLLRDHUP),
	entry(EPOLLPRI),
	entry(EPOLLERR),
	entry(EPOLLHUP),
	entry(EPOLLET),
	entry(EPOLLONESHOT),
};
#undef entry

static inline void print_events(char* buffer, size_t size, uint32_t events) {
	char* p=buffer;
	size_t cursize=size;
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(epoll_tbl); i++) {
		int val=epoll_tbl[i].val;
		const char* name=epoll_tbl[i].name;
		if(events & val) {
			size_t ret=CHECK_IN_RANGE(snprintf(p, cursize, "%s ", name), 1, cursize);
			cursize-=ret;
			p+=ret;
		}
	}
}
