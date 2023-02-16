/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdarg.h>	// for va_start(3), va_end(3)
#include <syslog.h>	// for vsyslog(3)
#include <stdio.h>	// for fprintf(3), vfprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a demo of how to do a full tracer, with:
 * - compiler builtins (__FILE__).
 * - either stderr (via vfprintf) or syslog (via vsyslog).
 * - var args macro (...)
 * - backtracing (NOT DONE).
 * - indented messages (NOT DONE).
 *
 * In this example I use syslog defined priorities so as not to build a convesion
 * mechanism from my own priority levels to syslog priorities.
 *
 * TODO:
 * - add backtracing.
 * - add indentation.
 * - make all the logging go through one call to vsyslog.
 * - log to stderr instead of syslog.
 */

const bool log_stderr=true;

inline void my_trace(int level, const char* file, const int line, const char* func, const char* fmt, ...) {
	if(log_stderr) {
		// the current implemention is quite simplistic...
		fprintf(stderr, "level is %d\n", level);
		fprintf(stderr, "file is %s\n", file);
		fprintf(stderr, "line is %d\n", line);
		fprintf(stderr, "func is %s\n", func);
		// now lets print the actual message...
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
	} else {
		// the current implemention is quite simplistic...
		syslog(level, "level is %d\n", level);
		syslog(level, "file is %s\n", file);
		syslog(level, "line is %d\n", line);
		syslog(level, "func is %s\n", func);
		// now lets print the actual message...
		va_list args;
		va_start(args, fmt);
		vsyslog(level, fmt, args);
		va_end(args);
	}
}
inline void my_trace(int level, const char* file, const int line, const char* func, const char* fmt, ...) __attribute__((format(printf, 5, 6)));

// now wrap this up to get the right compiler macros...
// this is a pre-processor varargs type of tracer...
#define MY_TRACE(level, fmt, args ...) my_trace(level, __FILE__, __LINE__, __func__, fmt, ## args)

class A {
public:
	void thisMethod(int a, int b) {
		MY_TRACE(LOG_DEBUG, "did you know that %d+%d=%d?\n", a, b, a+b);
	}
};

int main(int argc, char** argv, char** envp) {
	A a;
	a.thisMethod(5, 6);
	return EXIT_SUCCESS;
}
