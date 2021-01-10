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
#include <stdio.h>	// for printf(3), fgets(3)
#include <unistd.h>	// for sleep(3), getpagesize(2)
#include <stdlib.h>	// for malloc(3), atoi(3), EXIT_SUCCESS
#include <sys/mman.h>	// for mlockall(2), munlockall(2)
#include <malloc.h>	// for malloc_stats(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()
#include <proc_utils.h>	// for proc_print_mem_stats_self()

/*
 * This example demostrates that malloc doesnt actually allocate
 * physical memory and that just touching the memory slowly allocates
 * it. It also demostrates:
 * - how to print your own memory usage via the libproc library.
 * - how to get the system page size (getpagesize(2)).
 * - how to use the mlockall(2) system call and it's effect on malloc.
 *
 * Notes:
 * - you have to have the right limits set in order to call mlockall. On a standard
 * Ubuntu system ulimit. A standard workaround in an ubuntu system like this is
 * to run the process as root (via sudo or the like...).
 *
 * You can also look at how this program is working using this:
 * while [[ true ]]; do ps -C malloc_lazy.exe -o comm,min_flt,rss; sleep 1;done
 *
 * EXTRA_LINK_FLAGS=-lprocps
 */

int main(int argc, char** argv, char** envp) {
	const unsigned int page_number=2000;
	int page_size=getpagesize();
	char *p=(char *)malloc(page_size * page_number);
	unsigned int page_counter=0;
	bool over=false;
	unsigned int pagenum;
	while(!over) {
		printf("What do you want to do ?\n");
		printf("1) touch some pages\n");
		printf("2) touch all pages\n");
		printf("3) allocate more ram (without touching it)\n");
		printf("4) allocate more ram (with calloc)\n");
		printf("5) allocate more ram (with touching it)\n");
		printf("6) call mlockall (no future)\n");
		printf("7) call mlockall (with future)\n");
		printf("8) call munlockall\n");
		printf("9) just print stats\n");
		printf("10) stats\n");
		printf("11) exit\n");
		char buf[256];
		CHECK_NOT_NULL(fgets(buf, sizeof(buf), stdin));
		int result=atoi(buf);
		switch(result) {
		case 1:
			// get page number from user...
			printf("how many pages to touch ?\n");
			CHECK_NOT_NULL(fgets(buf, sizeof(buf), stdin));
			pagenum=atoi(buf);
			for(unsigned int i=0; i < pagenum; i++) {
				p[page_counter * page_size]=0;
				page_counter++;
			}
			proc_print_mem_stats_self();
			break;
		case 2:
			for(unsigned int i=0; i < page_number; i++) {
				p[i * page_size]=0;
			}
			proc_print_mem_stats_self();
			break;
		case 3:
			p=(char *)malloc(page_size * page_number);
			proc_print_mem_stats_self();
			break;
		case 4:
			p=(char *)calloc(page_size, page_number);
			proc_print_mem_stats_self();
			break;
		case 5:
			p=(char *)malloc(page_size * page_number);
			memset(p, 5, page_size*page_number);
			proc_print_mem_stats_self();
			break;
		case 6:
			CHECK_NOT_M1(mlockall(MCL_CURRENT));
			proc_print_mem_stats_self();
			break;
		case 7:
			CHECK_NOT_M1(mlockall(MCL_FUTURE));
			proc_print_mem_stats_self();
			break;
		case 8:
			CHECK_NOT_M1(munlockall());
			proc_print_mem_stats_self();
			break;
		case 9:
			proc_print_mem_stats_self();
			break;
		case 10:
			malloc_stats();
			break;
		case 11:
			over=true;
			break;
		default:
			printf("I don't know what you mean by that...\n");
			break;
		}
	}
	return EXIT_SUCCESS;
}
