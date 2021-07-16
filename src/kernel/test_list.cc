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
#include <stdio.h>	// for printf(3), fgets(3)
#include <unistd.h>	// for close(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()
#include <us_helper.h>	// for waitkey()
#include <multiproc_utils.h>	// for my_system()
#include <kernel_utils.h>	// for klog_show(), klog_show_clear()
#include "shared.h"

/*
 * This is a test for in kernel lists
 */
int get_number(void) {
	printf("please enter a number> ");
	char str[256];
	CHECK_NOT_NULL(fgets(str, 256, stdin));
	return(atoi(str));
}

int show_menu(void) {
	int selection=-1;
	while(selection<0 || selection >6) {
		printf("What would you like to do ?\n");
		printf("===========================\n");
		printf("0) create\n");
		printf("1) destroy\n");
		printf("2) isempty\n");
		printf("3) add\n");
		printf("4) del\n");
		printf("5) print\n");
		printf("6) exit\n");
		printf("choice> ");
		char str[256];
		CHECK_NOT_NULL(fgets(str, 256, stdin));
		selection=atoi(str);
	}
	return selection;
}

static int codes[]={
	IOCTL_LIST_CREATE,
	IOCTL_LIST_DESTROY,
	IOCTL_LIST_ISEMPTY,
	IOCTL_LIST_ADD,
	IOCTL_LIST_DEL,
	IOCTL_LIST_PRINT,
};

int main(int argc, char** argv, char** envp) {
	const char *filename="/dev/mod_list";
	printf("Inserting the driver...\n");
	my_system("sudo rmmod mod_list");
	my_system("sudo insmod ./mod_list.ko");
	my_system("sudo chmod 666 %s", filename);
	int fd=CHECK_NOT_M1(open(filename, O_RDWR));
	int choice=show_menu();
	while(choice!=6) {
		int arg=0;
		int ioctl_code=codes[choice];
		if (ioctl_code==IOCTL_LIST_ADD) {
			arg=get_number();
		}
		klog_clear();
		CHECK_NOT_M1(ioctl(fd, ioctl_code, arg));
		klog_show_clear();
		// waitkey(NULL);
		choice=show_menu();
	}
	CHECK_NOT_M1(close(fd));
	waitkey(NULL);
	return EXIT_SUCCESS;
}
