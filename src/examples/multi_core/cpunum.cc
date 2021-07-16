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
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for sysconf(3)
#include <sys/sysinfo.h>// for get_nprocs_conf(3), get_nprocs(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is a simple example showing how to get the number of current cpus...
 * _SC_NPROCESSORS_CONF - means configured CPUs, not neccessarily working
 * _SC_NPROCESSORS_ONLN - means online CPUs. You may not have access to all of
 * them.
 * Remmember that the number of online CPUs can change at any minute.
 *
 * Note: you can see and set some of the variables via getconf(1)
 */

int main(int argc, char** argv, char** envp) {
	printf("sysconf(_SC_NPROCESSORS_ONLN)=%d\n", CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN)));
	printf("sysconf(_SC_NPROCESSORS_CONF)=%d\n", CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_CONF)));
	printf("get_nprocs_conf()=%d\n", get_nprocs_conf());
	printf("get_nprocs()=%d\n", get_nprocs());
	// this one is from Ingo Molnars time warp test...
	int cpus = system("exit `grep ^processor /proc/cpuinfo | wc -l`");
	cpus = WEXITSTATUS(cpus);
	printf("cpus from /proc/cpuinfo=%d\n", cpus);
	return EXIT_SUCCESS;
}
