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

#ifndef __us_helper_h
#define __us_helper_h

/*
 * This is an eclectic collection of helper functions for user space
 * programming. Some of them, after a period of maturation, will ultimately
 * find their resting places in other files. This is sometimes an incubator.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <sched.h>	// for sched_getparam(2), sched_getscheduler(2)
#include <stdio.h>	// for printf(3), fprintf(3), snprintf(3), stdout, stderr, stdin
#include <stdlib.h>	// for system(3), exit(3)
#include <stdarg.h>	// for vsnprintf(3), va_start(3), va_list(3), va_end(3)
#include <sys/types.h>	// for getpid(2), gettid(2)
#include <unistd.h>	// for getpid(2), syscall(2), sysconf(2), getpagesize(2)
#include <sys/syscall.h>// for syscall(2), SYS_* constants
#include <string.h>	// for strncpy(3), strerror(3)
#include <sys/time.h>	// for getpriority(2)
#include <sys/resource.h>	// for getpriority(2)
#include <pthread.h>	// for the entire pthread_* API
#include <errno.h>	// for errno
#include <stdbool.h>	// for bool
#include <error.h>	// for error_at_line(3)
#include <err.h>// for err(3)
#include <signal.h>	// for sighandler_t
#include <dirent.h>	// for NAME_MAX:const

/*
 * Stringify macros - helps you turn anything into a string
 * Stolen from the linux kernel.
 * This MUST be done in 2 levels...
 * Need to explain how this works...
 */
#define quote(a) # a
#define stringify(x) quote(x)

/*
 * these were stolen shamelessly from the kernel.
 * It is a good idea to use them so that if you have a compiler
 * that does not have __builtin_expect you would just define likely/unlikely
 * in a way that is appropriate to that compiler, or maybe even just
 * the identity function.
 */
#define mylikely(x) __builtin_expect((x), 1)
#define myunlikely(x) __builtin_expect((x), 0)
/*
 * #define mylikely(x) x
 * #define myunlikely(x) x
 */

/*
 * Macro to calculate size of array
 */
#define ARRAY_SIZEOF(a) (sizeof(a)/sizeof(a[0]))
/*
 * Macro to print the sizeof of a type
 */
#define PRINT_SIZEOF(type) printf("size of " stringify(type) " is %zd\n", sizeof(type))

static inline unsigned int get_clk_tck(void) {
	return sysconf(_SC_CLK_TCK);
}

static inline unsigned int min(unsigned int a, unsigned int b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}

static inline void waitkey(const char *msg) {
	if (msg) {
		fprintf(stdout, "%s...\n", msg);
	} else {
		fprintf(stdout, "Press any key to continue...\n");
	}
	// sc(setvbuf(stdin,NULL,_IONBF,0));
	fgetc(stdin);
}

/*
 * Check that an entire area of memory has a certain value
 */
static inline void memcheck(void *buf, char val, unsigned int size) {
	char *cbuf=(char *)buf;
	unsigned int i;
	for(i=0; i<size; i++) {
		if(cbuf[i]!=val) {
			fprintf(stderr, "ERROR: value at %u is %c and not %c\n", i, cbuf[i], val);
			exit(EXIT_FAILURE);
		}
	}
}

/*
 * A standard max function
 */
static inline int my_max(int a, int b) {
	if(a>b) {
		return a;
	} else {
		return b;
	}
}

/*
 * This function gets the current executables name
 */
extern char* program_invocation_name;
static inline void get_program_name(char* buf, size_t size) {
	snprintf(buf, size, "%s", program_invocation_name);
	// version with /proc
	// ssize_t res=CHECK_NOT_M1(readlink("/proc/self/exe", buf, size));
	// buf[res]='\0';
}

/*
 * A function that checks that no parameters have been passed
 * to an executable.
 */
static inline void no_params(int argc, char** argv) {
	if(argc>1) {
		fprintf(stderr, "%s: usage: %s (without parameters)\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
}

/*
 * Type struct old_linux_dirent which is taken from the readdir(2)
 * manpage
 */
struct old_linux_dirent {
	long d_ino;	/* inode number */
	off_t d_off;	/* offset to this old_linux_dirent */
	unsigned short d_reclen;/* length of this d_name */
	char d_name[NAME_MAX+1];/* filename (null-terminated) */
};

/*
 * Type struct linux_dirent which is taken from getdents(2) manpage
 */
struct linux_dirent {
	unsigned long d_ino;	/* Inode number */
	unsigned long d_off;	/* Offset to next linux_dirent */
	unsigned short d_reclen;/* Length of this linux_dirent */
	char* d_name;	/* Filename (null-terminated) */
};

/*
 * Wrapper for the getdents(2) system call which is not supplied by glibc
 */
static inline int syscall_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) {
	return syscall(SYS_getdents, fd, dirp, count);
}

/*
 * Wrapper for the readdir(2) system call which is not supplied by glibc
 * This is surrounded by #ifdef because on 64bit I had problems locating
 * the 'SYS_readdir' macro.
 */
#ifdef SYS_readdir
static inline int syscall_readdir(unsigned int fd, struct old_linux_dirent *dirp, unsigned int count) {
	return syscall(SYS_readdir, fd, dirp, count);
}
#endif	/* SYS_readdir */

/*
 * A function that translates an address to it's page boundary
 */
static inline void* page_adr(void* adr) {
	unsigned long iptr=(unsigned long)adr;
	return (void*)(iptr-iptr%getpagesize());
}

#endif	/* !__us_helper_h */
