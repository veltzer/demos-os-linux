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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	const int bufsize=1000;
	int buf[bufsize];
	for(int i=0;i<bufsize;i++) {
		buf[i]=i;
	}
	const int very_long=1000000000;
	double sum=0;
	for(int i=0;i<very_long;i++) {
		if(rand()%2==0) {
			sum+=buf[i%bufsize];
		} else {
			sum-=buf[i%bufsize];
		}
	}
	printf("sum is %lf\n", sum);
	return 0;
}

