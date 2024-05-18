/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <lowlevel_utils.h>	// for getstackpointer(), stack_align_pointer(), stack_vars_direction_up(), stack_function_direction_up()
#include <proc_utils.h>	// for proc_get_stack_adr(), proc_get_start_stack(), proc_print_mmap_self_filter()
#include <pthread_utils.h>	// for pthread_getstack_pointer()

/*
 *      This program tries to find the start address of your stack segment.
 *      The whole stack segment that is, the one that appears in the output
 *      of pmap(1) or /proc/[pid]/maps.
 *
 *      Why would you want this?
 *      - hacking. Hackers love to find the start of the stack so they can know
 *      where do they want to mess things up or maybe call mprotect(2) on.
 *      - you want to see if you are in an infinite recursion.
 *
 *      There are many attempts here to find this address but none of them work
 *      due to Address Space Layout Randomization or ASLR which means that the
 *      kernel + the standard C library work together to make addresses change
 *      at each run to make it hard for hackers to find them (and so programmers
 *      find them hard to find also...).
 *
 *      It turns out that the only way I could find the address is to actually
 *      read /proc/[pid]/maps and get it from there...:)
 *
 *      References:
 *      http://en.wikipedia.org/wiki/Address_space_layout_randomization
 *
 *      NOTES:
 *      - to turn off kernel randomization of address space layout:
 *      echo 0 > /proc/sys/kernel/randomize_va_space
 *      the ubuntu default for this /proc value is 2.
 */

const char* rw(bool v) {
	if(v) {
		return "right";
	} else {
		return "wrong";
	}
}

void print_entry(const char* msg, void* p, void* right_address) {
	printf("%-40s [%p] [%s]\n", msg, p, rw(p==right_address));
}

int main() {
	printf("%-40s [%d]\n", "stack_vars_direction_up", stack_vars_direction_up());
	printf("%-40s [%d]\n", "stack_function_direction_up", stack_function_direction_up());
	printf("\n");
	void* right_address=proc_get_stack_adr();
	print_entry("proc_get_stack_adr", proc_get_stack_adr(), right_address);
	print_entry("getstackpointer", getstackpointer(), right_address);
	print_entry("getstackpointer(aligned)", stack_align_pointer(getstackpointer()), right_address);
	print_entry("proc_get_start_stack", proc_get_start_stack(), right_address);
	print_entry("proc_get_start_stack(aligned)", stack_align_pointer(proc_get_start_stack()), right_address);
	print_entry("pthread_getstack_pointer", pthread_getstack_pointer(), right_address);
	print_entry("pthread_getstack_pointer(aligned)", stack_align_pointer(pthread_getstack_pointer()), right_address);
	printf("This is the correct one...\n");
	proc_print_mmap_self_filter("[stack]");
	return EXIT_SUCCESS;
}
