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
#include <stdlib.h>	// for malloc(3), free(3), exit(3), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/mman.h>	// for mprotect(2)
#include <signal.h>	// for SIGSEGV, psiginfo(3), strsignal(3)
#include <malloc.h>	// for memalign(3)
#include <unistd.h>	// for getpagesize(2)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()
#include <signal_utils.h>	// for signal_register_handler_sigaction()

/*
 * This is a demo of the use of mprotect(2) to protect your memory and thus try to
 * find a memory violation.
 */

static void handler(int sig, siginfo_t *si, void *unused) {
	printf("got signal %s\n", strsignal(sig));
	printf("si is %p\n", (void*)si);
	printf("Got SIGSEGV at address: 0x%lx\n", (long) si->si_addr);
	psiginfo(si, "This is a message\n");
	exit(EXIT_FAILURE);
}

const void* mymalloc(unsigned int size, int protect) {
	if(protect) {
		void* buf=CHECK_NOT_NULL(memalign(getpagesize(), size));
		CHECK_NOT_M1(mprotect(buf, size, PROT_READ));
		return buf;
		/*
		 * // does not work for some reason
		 * int ps=getpagesize();
		 * void* ret=malloc(ps+size);
		 * int iret=(int)ret;
		 * void* newaddress=(void*)((iret+ps-1) & ~(ps-1));
		 * size_t sz=size;
		 * CHECK_NOT_M1(mprotect(newaddress,sz,PROT_NONE));
		 * return newaddress;
		 */
	} else {
		return CHECK_NOT_NULL(malloc(size));
	}
}

void do_write(char* buf) {
	int i;
	for(i=0; i<10; i++) {
		buf[i]=0;
	}
}

int do_read(char* buf) {
	int i;
	int sum=0;
	for(i=0; i<10; i++) {
		sum+=buf[i];
	}
	return sum;
}

int main(int argc, char** argv, char** envp) {
	const bool handle_sigv=true;
	if(handle_sigv) {
		signal_register_handler_sigaction(SIGSEGV, handler, 0);
	}
	char* buffer=(char*)mymalloc(10, 1);

	TRACE("before trying to read the memory");
	int sum=do_read(buffer);
	TRACE("sum is %d\n", sum);
	TRACE("before trying to write the memory");
	do_write(buffer);

	free(buffer);
	return EXIT_SUCCESS;
}
