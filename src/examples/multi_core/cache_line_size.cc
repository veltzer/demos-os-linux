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
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for sysconf(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * An example showing how to get the cache line size on linux.
 * DCACHE is the data cache.
 * ICACHE is the instruction cache.
 *
 * There are many other parameters you can get via the sysconf interface. Some hardware
 * related and some software. do 'getconf -a' on the cmdline to see them...
 */

int main(int argc, char** argv, char** envp) {
	printf("sysconf(_SC_LEVEL1_DCACHE_LINESIZE)=%d\n", CHECK_NOT_M1(sysconf(_SC_LEVEL1_DCACHE_LINESIZE)));
	printf("sysconf(_SC_LEVEL1_ICACHE_LINESIZE)=%d\n", CHECK_NOT_M1(sysconf(_SC_LEVEL1_ICACHE_LINESIZE)));
	return EXIT_SUCCESS;
}
