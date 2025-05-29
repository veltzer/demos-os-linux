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
#include <sys/ioctl.h>	// for ioctl(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for no_params()
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is a test to check my own err_utils.hh file.
 */

int main(int argc, char** argv) {
	no_params(argc, argv);
	CHECK_NOT_M1(ioctl(5, 7));
	return EXIT_SUCCESS;
}
