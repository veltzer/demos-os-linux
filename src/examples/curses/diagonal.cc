/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ncursesw/curses.h>
#include <locale.h>	// for setlocale(3), LC_ALL

/* This is an ncurses playground in C/C++ so that I could test exactly what
 * ncurses is and how it works
 *
 * EXTRA_LINK_FLAGS_AFTER=-lncursesw
 */

static WINDOW* stdinit() {
	// initialize the screen
	WINDOW* stdscr=initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	clear();
	return stdscr;
}

static void demo_diagonal(WINDOW* stdscr) {
	for(int i=0;i<20;i++) {
		//these are the same
		//mvaddch(i,i,'a');
		mvwaddch(stdscr,i,i,'a');
	}
	refresh();
	// wait until key press
	getch();
	endwin();
}

static void demo_get_width_height(WINDOW* stdscr) {
	int nh, nw;
	getmaxyx(stdscr,nh,nw);
	endwin();
	printf("nh is [%d]\n", nh);
	printf("nw is [%d]\n", nw);
}

static void demo_old(WINDOW* stdscr) {
	chtype c=0;
	int nh, nw;
	getmaxyx(stdscr,nh,nw);
	while(true) {
		for(int x=0;x<nw;x++) {
			for(int y=0;y<nh;y++) {
				mvwaddch(stdscr,y,x,c);
				c++;
			}
		}
		refresh();
		getch();
		break;
	}
	endwin();
}

int main(int argc,char** argv,char** envp) {
	//setlocale(LC_ALL,"");
	WINDOW* stdscr=stdinit();
	if(1)
		demo_diagonal(stdscr);
	if(0)
		demo_get_width_height(stdscr);
	if(0)
		demo_old(stdscr);
	return 0;
}
