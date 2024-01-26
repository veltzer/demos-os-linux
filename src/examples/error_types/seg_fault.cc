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
#include <stdio.h>	// for fprintf(3), stderr:variable, atoi(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <trace_utils.h>// for INFO()
#include <proc_utils.h>	// for proc_print_mmap_self_only()

/*
 * This is a program showing various ways of creating a segmentation
 * fault:
 * - accessing an illegal address
 * The address "0" is not mapped into any regular processes virtual address space
 * which is a good thing since NULL (which is heavily used in C programs) has the
 * value 0.
 * - calling a function badly.
 * - trying to change a constant string.
 * this is impossible because of the MMU and the fact
 * that text pages are write protected (they are r-x).
 * - trying to write over the code segment.
 * this is impossible because of the MMU and the fact
 * that text pages are write protected (they are r-x).
 * - trying to write over a constant in the data segment.
 */

void func() {
	int i;
	const char* y __attribute__((unused))="123123dsfshfdkj$%^$%&^^%&@";
	for(i=0; i<100; i++) {
		fprintf(stderr, "i is %d\n", i);
	}
}

const int a=6;

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [type]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: type can be 0-4\n", argv[0]);
		return EXIT_FAILURE;
	}
	int choice=atoi(argv[1]);
	if(choice==0) {
		printf("going to call a function badly\n");
		char* f=(char*)func;
		f++;
		f++;
		void (*g)()=(void (*)())f;
		g();
	}
	if(choice==1) {
		printf("going to access the 0 address\n");
		// lets show that the NULL constant is actually the number 0...
		INFO("NULL is %zd", NULL);
		char *p=(char*)0;
		// cppcheck-suppress nullPointer
		p[0]=0;
	}
	if(choice==2) {
		printf("going to write on a read only string\n");
		// cppcheck-suppress stringLiteralWrite
		const char* hello="hello";
		char* p=(char*)hello;
		p[2]='g';
	}
	if(choice==3) {
		printf("going to write over code\n");
		char* p=(char*)func;
		p[2]='g';
	}
	if(choice==4) {
		printf("going to write a constant in the data segment\n");
		int* p=(int*)&a;
		*p=5;
		// printf("p is %p\n", p);
		// proc_print_mmap_self_only();
		// printf("a is %d\n",a);
	}
	fprintf(stderr, "%s: what kind of choice is %d ?!?\n", argv[0], choice);
	return EXIT_SUCCESS;
}
