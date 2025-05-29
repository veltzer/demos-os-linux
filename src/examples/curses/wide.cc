/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ncursesw/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

/*
 * Curses example number 3...
 *
 * This is an example of how to use wide characters in curses...
 *
 * EXTRA_LINK_FLAGS_AFTER=-lncursesw
 */

static const char *myMsg=NULL;
void setErr(const char *msg) {
	myMsg=msg;
}

void printErr(void) {
	printf("ERROR: message was [%s]\n", myMsg);
}

int main() {
	setlocale(LC_ALL, "");

	/*
	 * printf("size of chtype is %d\n",sizeof(chtype));
	 * printf("size of wchar_t is %d\n",sizeof(wchar_t));
	 * printf("size of char is %d\n",sizeof(char));
	 * printf("size of cchar_t is %d\n",sizeof(cchar_t));
	 */
	initscr();
	cbreak();
	noecho();
	// const wchar_t* string=L"מרק";
	bool stop=false;
	while(!stop) {
		wint_t c;
		int res;
		res=get_wch(&c);
		if (res==ERR) {
			stop=true;
			setErr("could not get_wch");
			continue;
		}
		// printw("%d ",c);
		wchar_t str[10];
		swprintf(str, 10, L"%lc", c);
		res=addwstr(str);
		if (res==ERR) {
			stop=true;
			setErr("could not addwstr");
			continue;
		}
		/*
		 * res=addwstr(string);
		 * res=addnwstr(&c,1);
		 * if(res==ERR) {
		 *	stop=true;
		 *	setErr("could not addwstr");
		 *	continue;
		 * }
		 * refresh();
		 */
	}
	endwin();
	printErr();
	return EXIT_SUCCESS;
}
