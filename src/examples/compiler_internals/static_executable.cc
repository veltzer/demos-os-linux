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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <multiproc_utils.h>	// for my_system()

/*
 * This example shows how to link a C++ executable in a static way.
 * If you want to see the results then just ldd(1) this executable.
 * note that you could provide a '-static' flag to g++ which will make
 * ALL DSOs static and the entire executable static. This is not what
 * this example shows. This example deals with making libstdc++ static.
 *
 * This is what causes the static link:
 * EXTRA_LINK_FLAGS=-static-libstdc++
 */

int main(int argc, char** argv, char** envp) {
	my_system("ldd %s", argv[0]);
	return EXIT_SUCCESS;
}
