/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for stdin, printf(3), fgetc(3), fileno(3)
#include <unistd.h>	// for isatty, ttyname
#include <termios.h>	// for tcsetattr, tcgetattr
#include <stdlib.h>	// for EXIT_SUCCESS
#include <trace_utils.h>// for DEBUG()
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * Try to think of the next exercise: try to read just one character from
 * standard input. If you try to do it you will find that the user is prompted
 * for a full line and all you get is that single character. In addition, trying
 * to control the size of the buffer of stdin (say using setvbuf for instance)
 * will not work. Why ?
 *
 * // here are examples of attempts that will not work...
 * char buf[1];
 * // this call does not release until eol...
 * int ret=fread(buf,sizeof(buf),1,stdin);
 * printf("ret is %d\n",ret);
 * // now set the buffereing to no buffering...
 * setvbuf(stdin,NULL,_IONBF,0);
 * // now repeat the same call as above
 * ret=fread(buf,bufsize,1,stdin);
 * printf("ret is %d\n",ret);
 *
 * The reason it does not work because standard input which is the file in
 * question in this example is usually connected to a kernel level terminal
 * device. Terminal devices have buffered input IN THE KERNEL and so only
 * pass off the data once the line is done. This example should be modified
 * to alter the characteristics of the controlling terminal of stdin using
 * the right APIS. 'info libc' and search for terminal to see those API (there
 * is quite a bunch of those).
 *
 * This is an example of how to remove buffering from the terminal and read
 * characters one by one...
 */

void set_noncannon(int desc) {
	struct termios settings;
	CHECK_NOT_M1(tcgetattr(desc, &settings));
	cfmakeraw(&settings);
	// settings.c_lflag&=~ICANON;
	CHECK_NOT_M1(tcsetattr(desc, TCSANOW, &settings));
}

int main(int argc, char** argv, char** envp) {
	// lets take the file descriptor number from stdin which is usually
	// a terminal (unless you redirect it...)
	int filedes=fileno(stdin);
	if (isatty(filedes)) {
		DEBUG("it is a terminal with name [%s]", ttyname(filedes));
	} else {
		DEBUG("it is not a terminal");
	}
	set_noncannon(filedes);
	int c=fgetc(stdin);
	while(c!='e') {
		printf("got [%c]\n", (unsigned char)c);
		c=fgetc(stdin);
	}
	return EXIT_SUCCESS;
}
