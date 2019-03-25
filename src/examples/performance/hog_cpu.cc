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


#include <stdio.h>

unsigned long long sum_it(unsigned long to) __attribute__((noinline));
unsigned long long sum_it(unsigned long to) {
	unsigned long long sum=0;
	for(unsigned long i=0;i<10000000;i++) {
		for(unsigned long j=0;j<10000000;j++) {
			sum+=j*j*j;
		}
	}
	return sum;
}

int main(int argc, char** argv, char** envp) {
	unsigned long long sum=0;
	for(unsigned long i=0;i<10000000;i++) {
		sum+=sum_it(10000000);
	}
	printf("sum is %llu\n", sum);
	return 0;
}
