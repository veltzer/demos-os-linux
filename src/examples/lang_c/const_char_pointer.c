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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/mman.h>	// for mprotect(2)
#include <signal_utils.h>	// for signal_segfault_protect()
#include <us_helper.h>	// for page_adr()
#include <proc_utils.h>	// for proc_print_mmap_self_only()
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example explores constant strings in C
 *
 * NOTES
 * - If you define a 'const' pointer in C++ then you can assign any
 * other const pointer of the same type to it.
 * - The compiler issues an error whenever your program tries to write to
 * the const pointer.
 * - You can force acess to a "const char*" string storage via pointer manipulation.
 * The compiler will not stop you there.
 * - If you do try to access that memory space you will get a segfault.
 * - You get the segfault because constant strings are allocated in read only memory.
 * - The fact that constat strings are allocated in read only memory allows the
 * compiler to optimize this memory and never store the same string twice.
 * - That is why other and other2 have the exact same address
 * - If you optimize your program (-O2) then the compiler makes some of your memory
 * access functions go away and so you don't segfault.
 * - You can make read only memory areas be read/write using mprotect(2).
 */

int main(int argc, char** argv, char** envp) {
	const char *str1="this is the content of str1";
	const char *str2="this is the content of str2";
	const char *str3="this is the content of str3";

	proc_print_mmap_self_only();

	printf("str1 is %p (%s)\n", str1, str1);
	printf("str2 is %p (%s)\n", str2, str2);
	printf("str3 is %p (%s)\n", str3, str3);

	// this does NOT generate a compile error!
	printf("doing assignment of one const char* to another\n");
	str1=str3;

	printf("str1 is %p (%s)\n", str1, str1);
	printf("str2 is %p (%s)\n", str2, str2);
	printf("str3 is %p (%s)\n", str3, str3);

	// next line will cause the following compiled time error:
	// --- char_pointer.c:15: error: assignment of read-only location ‘*(str1 + 2u)’
	// str1[2]='y';
	// --> This means const strings cannot be changed directly

	// We should get a segmentation fault for trying the following...
	// The casting is neccessary to avoid a compilation error.
	printf("trying to change the value of the const char* buffer...\n");
	if(signal_segfault_protect()) {
		char *p=(char *)str1;
		sprintf(p, "new content ovriding old");
	}
	// So lets relax the constraints of the mmu
	char *p=(char *)str1;
	CHECK_NOT_M1(mprotect(page_adr((void*)p), getpagesize(), PROT_READ|PROT_WRITE|PROT_EXEC));
	proc_print_mmap_self_only();
	sprintf(p, "new content ovriding old");

	printf("str1 is %p (%s)\n", str1, str1);
	printf("str2 is %p (%s)\n", str2, str2);
	printf("str3 is %p (%s)\n", str3, str3);
	return EXIT_SUCCESS;
}
