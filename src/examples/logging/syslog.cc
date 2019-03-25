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
#include <syslog.h>	// for openlog(3), syslog(3), closelog(3), setlogmask(3)
#include <unistd.h>	// for sleep(3)
#include <stdarg.h>	// for vsyslog(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <stdio.h>	// for fprintf(3)
#include <err_utils.h>	// for CHECK_ZERO()
#include <multiproc_utils.h>	// for my_system()

/*
 * This example shows syslog basic usage. At the end it demostrates that the log
 * actually reached the system log.
 *
 * It seems that the openlog and closelog are not mandatory...
 * If you leave them out then you will not get your own name as the application
 * name and so you get defaults for parameters passed to openlog.
 *
 * How is syslog implemented? Either as a socket or udp socket. It is listening
 * on /dev/log as a udp socket and logs the messages sent to it.
 *
 * A question that is often raised is what is the performance of syslog compared to
 * printf to a file - the answer is faster since it is sending the data through the socket
 * and does not wait for it to be written to disk. Each call to syslog will actually
 * generate two different system calls: time(2) and then send(2). The time(2) call is
 * in order to log the time that the message was sent at (source time). Then the time
 * is formatted in the form of a date and sent, along with the data, to syslog.
 * You can see all of this if you strace(1) this example. There seems to be no way to
 * tell syslog(3) not to do this timing thing and just send the message.
 *
 * So how long does it take to call syslog? Two system calls worth (pretty long but
 * shorter than writing to a file).
 *
 * See a different example of how to use syslog as a general logging and tracing facility
 * using the vsyslog function.
 *
 * Notice the use of setlogmask in order to control which logs actually go to the log.
 *
 */

/*
 * this is a debug function for me to find out what the syslog constants for logging
 * levels are...
 */
#define PRINT_LEVEL(a) fprintf(stderr, # a " is %d\n", a)
void print_syslog_numeric_values(void) {
	PRINT_LEVEL(LOG_EMERG);
	PRINT_LEVEL(LOG_ALERT);
	PRINT_LEVEL(LOG_CRIT);
	PRINT_LEVEL(LOG_ERR);
	PRINT_LEVEL(LOG_WARNING);
	PRINT_LEVEL(LOG_NOTICE);
	PRINT_LEVEL(LOG_INFO);
	PRINT_LEVEL(LOG_DEBUG);
}

// string representation of the various log levels for pretty output...
const char* str_log[8]={
	"LOG_EMERG",
	"LOG_ALERT",
	"LOG_CRIT",
	"LOG_ERR",
	"LOG_WARNING",
	"LOG_NOTICE",
	"LOG_INFO",
	"LOG_DEBUG"
};
const int pri_log[8]={
	LOG_EMERG,
	LOG_ALERT,
	LOG_CRIT,
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG
};

int main(int argc, char** argv, char** envp) {
	print_syslog_numeric_values();
	// the named
	const char* myname="myname";
	// record the process id doing the logging, regular user process logging...
	openlog(myname, LOG_PID, LOG_USER);
	// show the open file descriptor map after opening syslog
	// at this point the socket is still not created...
	my_system("ls -l /proc/%d/fd | grep socket", getpid());
	// lets set our loglevel
	setlogmask(LOG_UPTO(LOG_WARNING));
	// lets send different messages with different log levels
	// we will cycle through the different log levels...
	for(int i=0; i < 200; i++) {
		int entry=i%8;
		syslog(pri_log[entry], "this is a message %d of loglevel %s", i, str_log[entry]);
	}
	// lets send the same message many times to show that
	// notice that the filtering of same messages IS NOT done at the source
	// syslog will ACTUALLY send the message 200 times, the syslogd application
	// is the one that will notice that the same message was received and will
	// only print it once and then, after a delay or upon receiving a different
	// message, will print the number of times that that message appeared...
	// you can verify this by stracing this demo and seeing 200 different calls
	// to send(2).
	for(int i=0; i < 200; i++) {
		syslog(LOG_ERR, "did you know that 2+2=4?");
	}
	// lets send a different message to make syslogd(1) print the number of times
	// the message was sent...
	syslog(LOG_ERR, "goodbye, syslog");
	// show the open file descriptor map after opening syslog
	// the socket is now here - it seems that syslog is lazy, you can control
	// this laziness if you pass LOG_NDELAY to openlog...
	my_system("ls -l /proc/%d/fd | grep socket", getpid());
	// close the connection with syslogd(1)
	closelog();
	// show that the socket is not there after closelog(3)
	my_system("ls -l /proc/%d/fd | grep socket", getpid());
	// let syslogd (the process) a chance to actually put the data in the log
	CHECK_ZERO(sleep(1));
	// show my own log (diregard other logs from other programs)
	my_system("cat /var/log/syslog | grep %s | grep %d", myname, getpid());
	return EXIT_SUCCESS;
}
