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
#include <signal.h>	// for signal(2), SIG_ERR
#include <unistd.h>	// for alarm(2), write(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_SIGT(), CHECK_ZERO()
#include <stdio.h>	// for snprintf(3), STDERR_FILENO

/*
 * This is an example of doing progress reports via the SIGALRM signal every second.
 * The main code does a tight calculation loop and the progress reporting to stderr
 * (file descriptor 2) is done via the alarm signal handler.
 */

/*
 * The variables are global to allow the signal handler to access them easily
 * You DONT need to initialize them to 0 since that is the default.
 * The 'volatile' on i is *critical* since it will be accessed asynchronously
 * and the compiler needs to know not to put it in a register since that
 * will mean that we cannot report it's value correctly from the signal
 * handler.
 * The volatile is also *critical* to force the compiler not to optimize away
 * the entire loop in the main function.
 */
volatile unsigned long long i;

/*
 * The time interval for updating the progress display, in seconds.
 */
const int interval=1;

/*
 * Remember that this is a signal handler and calls to fprintf(3) or the like
 * are forbidden so we are forced to use async-safe function (see man 7 signal).
 * That is the reason for the cumbersome code. Hopefylly snprintf(3) is safe
 * enough to use and does not do any memory allocations or the like. write(2)
 * alarm(2) are safe according to the manual.
 */
static void handler(int sig) {
	static int times=0;
	// we have to reschedule the SIGALRM every time since the alarm(2) is a one time deal.
	CHECK_ZERO(alarm(interval));
	char buf[100];
	times+=1;
	int len=snprintf(buf, sizeof(buf), "progress report number [%02d], [%012lld] units of work done...\r", times, i);
	CHECK_NOT_M1(write(STDERR_FILENO, buf, len));
}

int main(int argc, char** argv, char** envp) {
	CHECK_NOT_SIGT(signal(SIGALRM, handler), SIG_ERR);
	CHECK_ZERO(alarm(interval));
	// a very long calculation
	while(true) {
		/* Do some real work here */
		i++;
	}
	return EXIT_SUCCESS;
}
