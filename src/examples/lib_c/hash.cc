/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <search.h>	// for hcreate(3), hcreate_r(3), hdestroy_r(3), hdestroy(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This demos how to use the has supplied with the standard C library...
 *
 * You can see documentation for these functions at
 * man hsearch or info libc under 'hsearch' or 'hash'.
 */

int main(int argc, char** argv, char** envp) {
	// struct hsearch_data HTAB;
	// CHECK_NOT_M1(hcreate_r(50,&HTAB));
	// next function does not return any status...
	// hdestroy_r(&HTAB);
	CHECK_NOT_M1(hcreate(50));
	// next function does not return any status...
	hdestroy();
	/*
	 * TODO:
	 * - show more of the hash here
	 */
	return EXIT_SUCCESS;
}
