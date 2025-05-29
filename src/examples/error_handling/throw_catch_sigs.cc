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
#include <iostream>	// for cerr, endl
#include <signal.h>	// for sigemptyset(2), sigaddset(2), sigprocmask(2), raise(3), kill(2)
#include <stdlib.h>	// for exit(3), EXIT_FAILURE, EXIT_SUCCESS
#include <sys/types.h>	// for kill(2), getpid(2)
#include <unistd.h>	// for getpid(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <signal_utils.h>	// for signal_register_handler_signal()

using namespace std;

/*
 * This demp demostrates C++ style exception handling in response to OS signals.
 *
 * Things to notice:
 * - the non busy loop for waiting for signals in the main thread
 * - the need to unblock the signal in the signal handler
 * - the fact that raise(3) and kill(2) do not work exactly the same.
 * - the fact that the signal is blocked inside the signal handler while other signals are not.
 * this effects the protection you get on your data structures.
 * - if you want more than one signal cached then you should use the sigqueue(2) instead of the kill(2) interface and then you can also pass parameters to that handler (limit is according to ulimit).
 * - Only one signal is "cached" while you are blocking that signal (the kernel holds a mask of pending signals for you).
 * - I provided a SigQueue executable to allow you to do that. Use it to see the signals queued and the value that you provide cought by the process.
 */

/*
 * static void printStatus() {
 *	sigset_t old;
 *	CHECK_NOT_M1(sigprocmask(SIG_BLOCK,NULL,&old));
 *	int ret=CHECK_NOT_M1(sigismember(&old,SIGFPE));
 *	cerr << "ret is " << ret << endl;
 * }
 */

/*
 * This method unblocks a signal so that it could be gotten again.
 * The important thing to understand is that in a signal handler for signal X
 * the same signal is blocked. This means that since we throw an exception from
 * a signal handler (which actually does a longjmp) then this situation will not
 * change and so we need to unblock the signal if we want to use C++ exception handling
 * or longjmp
 */
static void unblock(int signum) {
	sigset_t sigs;

	sigemptyset(&sigs);
	sigaddset(&sigs, signum);
	CHECK_NOT_M1(sigprocmask(SIG_UNBLOCK, &sigs, NULL));
}

static void SignalHandler(int sig) {
	// before we throw an exception or do a longjmp we need to unblock
	// the signal or the kernel will think we are still in the signal handler
	unblock(sig);
	throw exception();
}

static void doBadCode(int i) {
	int x, y;
	char *p;
	switch (i) {
	/*
	 * This is a standard FPE exception
	 */
	case 0:
		cerr << "before division by zero" << endl;
		// turning x into float here would not work since it
		// will not cause an exception
		x=0;
		// cppcheck-suppress zerodiv
		y=1 / x;
		cerr << "y is " << y << endl;
		cerr << "this is never reached" << endl;
		break;

	/*
	 * This is a segmentation fault using a straight illegal memory access
	 */
	case 1:
		cerr << "Lets access some illegal memory address" << endl;
		p=0;
		// cppcheck-suppress nullPointer
		p[0]=0;
		break;

	/*
	 * This is a floating point exception by using raise(3)
	 */
	case 2:
		cerr << "Lets do a simulation of some bad code using raise(3)" << endl;
		CHECK_NOT_M1(raise(SIGFPE));
		break;

	/*
	 * This is a floating point exception by using kill(2)
	 */
	case 3:
		cerr << "Lets do a simulation of some bad code using kill(2)" << endl;
		CHECK_NOT_M1(kill(getpid(), SIGFPE));
		break;
	}
}

int main() {
	// set up the signal handler (only need to do this once)
	signal_register_handler_signal(SIGFPE, SignalHandler);
	signal_register_handler_signal(SIGSEGV, SignalHandler);
	for(int c=0; c<10; c++) {
		cerr << "c is " << c << endl;
		try {
			doBadCode(c % 2);
		} catch (exception& e) {
			cerr << "Got exception, lets continue anyway" << endl;
		}
	}
	return EXIT_SUCCESS;
}
