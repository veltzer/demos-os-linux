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

#include <firstinclude.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sched_utils.h>// for sched_print_table()

/*
 * This example simply prints all the scheduling policy consts.
 * See sched_utils.h for further info on how this is done.
 *
 * Notes:
 * - It is not an error in that there is a gap in the scheduling
 * constants. That is really how it is in the /usr/include/bits/sched.h file.
 */

int main(int argc, char** argv, char** envp) {
	sched_print_table();
	return EXIT_SUCCESS;
}
