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

#ifndef __atomic_utils_h
#define __atomic_utils_h

/*
 * This file is here to help you with atomic related stuff.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>

// stolen shamelssly from the gnu C library...
#ifndef atomic_full_barrier
# define atomic_full_barrier() asm volatile ("" ::: "memory")
#endif

#endif	/* !__atomic_utils_h */
