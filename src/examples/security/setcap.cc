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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/capability.h>	// for cap_get_proc(3), cap_set_flag(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT(), CHECK_NOT_NULL()

/*
 * This is an example showing setting of capabilities
 *
 * EXTRA_LINK_FLAGS=-lcap
 */

int main(int argc, char** argv, char** envp) {
	CHECK_ASSERT(CAP_IS_SUPPORTED(CAP_NET_BIND_SERVICE));
	cap_t caps=(cap_t)CHECK_NOT_NULL(cap_get_proc());
	cap_value_t cap_list[1];
	cap_list[0] = CAP_NET_BIND_SERVICE;
	CHECK_NOT_M1(cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET));
	CHECK_NOT_M1(cap_set_proc(caps));
	CHECK_NOT_M1(cap_free(caps));
	CHECK_NOT_M1(setuid(1000));
	return EXIT_SUCCESS;
}
