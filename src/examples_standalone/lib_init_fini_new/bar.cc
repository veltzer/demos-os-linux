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
#include <trace_utils.h>// for TRACE()

void bar_init(void) __attribute__((constructor));
void bar_init(void) {
	TRACE("start");
}
void bar_init105(void) __attribute__((constructor(105)));
void bar_init105(void) {
	TRACE("start");
}
void bar_init104(void) __attribute__((constructor(104)));
void bar_init104(void) {
	TRACE("start");
}
void bar_init103(void) __attribute__((constructor(103)));
void bar_init103(void) {
	TRACE("start");
}

void bar_fini(void) __attribute__((destructor));
void bar_fini(void) {
	TRACE("start");
}
void bar_fini105(void) __attribute__((destructor(105)));
void bar_fini105(void) {
	TRACE("start");
}
void bar_fini104(void) __attribute__((destructor(104)));
void bar_fini104(void) {
	TRACE("start");
}
void bar_fini103(void) __attribute__((destructor(103)));
void bar_fini103(void) {
	TRACE("start");
}

int bar(int a, int b) {
	return a+b;
}
