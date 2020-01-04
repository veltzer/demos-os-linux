/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for malloc(3), EXIT_SUCCESS
#include <unistd.h>	// for pause(2), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <sys/mman.h>	// for mprotect(2)
#include <err_utils.h>	// for CHECK_ASSERT()

/*
 * This example shows that regular strings (char*) are not protected
 * while const char* strings are protected (put in read only
 * memory). It also shows the use of mprotect(2) to change protection
 * on regions of memory.
 */

char mystring[100]="asdkjfhasdfkjhasdfsadfsadfasd";
const char* myotherstring="Hello, World!";

int main(int argc, char** argv, char** envp) {
	fprintf(stderr, "mystring is %p\n", mystring);
	fprintf(stderr, "myotherstring is %p\n", myotherstring);
	fprintf(stderr, "mypid is %d\n", getpid());
	// while(true) {
	// int ret=pause();
	// CHECK_ASSERT(ret==-1 && errno==EINTR);
	// }
	unsigned long modulu=(unsigned long)myotherstring%4096;
	void* protectme=(void*)(myotherstring-modulu);
	mprotect(protectme, 4096, PROT_READ|PROT_WRITE|PROT_EXEC);
	char* p=(char*)myotherstring;
	*(p+4)='e';
	fprintf(stderr, "myotherstring is %s\n", myotherstring);
	/*
	 * char* p=(char*)malloc(10);
	 * fprintf(stderr,"after malloc with p=%p\n",p);
	 */
	/*
	 * for(int i=0;i<1000000;i++) {
	 * *p=(char)random();
	 * if(i%1000==0) {
	 * fprintf(stderr,"Yes!, worked for i=%d\n",i);
	 * }
	 * p++;
	 * }
	 */
	return EXIT_SUCCESS;
}
