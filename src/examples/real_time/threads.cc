/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

/*
 * This application should show how to create real time threads...
 *
 * It demos:
 * - setting of priority.
 * - setting guard for the threads (good for debug).
 * - prefetching the threads (automatically).
 * - afining the threads to a cpu.
 * - locking all memory (mlock).
 * - blocking all malloc from that moment onward.
 */

int main(int argc, char** argv, char** envp) {
	return EXIT_SUCCESS;
}
