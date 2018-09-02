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
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h> // for EXIT_SUCCESS

void print_message_function(void *ptr);

int main(int argc,char** argv,char** envp) {
	pthread_t thread1;
	pthread_t thread2;
	printf("Starting first run\n");
	pthread_create(&thread1,NULL,(void *)&print_message_function, NULL);
	pthread_create(&thread2,NULL,(void *)&print_message_function, NULL);
	pthread_join (thread2, NULL);
	/* Not reached */
	printf("This should never happen!\n");
	pthread_join (thread1, NULL);
	return EXIT_SUCCESS;
}

void croak(void) {
	int* ip=(int*)17;
	char* p=sbrk(0);
	memset(p-1024,42,1024);
	write(3000,"xxx",3);
	*ip=7;
}

void die(void) {
	volatile int i=12;
	croak();
	i++;
	return;
}

void print_message_function(void *dummy) {
	printf("Morituri te salutant!\n");
	die();
	pthread_exit(EXIT_SUCCESS);
}
