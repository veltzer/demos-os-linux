/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __security_utils
#define __security_utils

/*
 * This file provides helper function to deal with security and permission
 * issues.
 */

#include <firstinclude.h>
#include <unistd.h>	// for geteuid(2)
#include <sys/types.h>	// for geteuid(2)
#include <err_utils.h>	// for CHECK_MSG_ZERO()

/*
 * check that the current process is running as root
 */
static inline void check_root() {
	CHECK_MSG_ZERO(geteuid(), "you are not root, maybe you should try sudo(1)?");
}

#endif	/* !__security_utils */
