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
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	printf("I'm here and I'm alive...\n");
	for(int i=0;i<10;i++) {
		// this is the bug, this line will cause the operating system to kill me...:)
		printf("i is %d\n", i);
		if(i==5) {
			char* p=(char*)0;
			*p=0;
		}
		sleep(1);
	}
	printf("You can't see this becuase the process is already dead...\n");
	return EXIT_SUCCESS;
}
