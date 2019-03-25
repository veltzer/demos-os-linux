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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>	// for usleep(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * Curses example number 1...
 *
 * This is an example of how to read a character from the keyboard
 * in non-blocking mode...
 *
 * EXTRA_LINK_FLAGS=-lncursesw
 */

int main(int argc, char** argv, char** envp) {
	initscr();
	cbreak();
	noecho();
	while(true) {
		int c=getch();
		if (c==ERR) {
			// there is no input on the keyboard. Wait a while to avoid busy wait...
			// CHECK_NOT_M1(usleep(100000));
		} else {
			// We have a character!!! Print it on the screen...
			echochar(c);
			// printf("char is %d\n",c);
		}
	}
	endwin();
	return EXIT_SUCCESS;
}
