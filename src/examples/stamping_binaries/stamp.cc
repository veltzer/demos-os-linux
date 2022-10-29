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
#include <stdio.h>	// for snprintf(3), printf(3)
#include <time.h>	// for strptime(3), mktime(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_NOT_M1()

/*
 *	This example shows how to stamp binaries so that you will be able to know exactly who
 *	made them, when, on which machine, what version of software, from what source file and
 *	made what version was used.
 *	- to get the time stamp we use gcc's __DATE__ __TIME__ __FILE__ __LINE__ and related macros.
 *	- the demo also shows how to actually print the version from your program if you want to.
 *	- the demo shows how to stamp EACH compilation unit separately so that you will know what
 *		version of each individual object file is.
 *	- the demo shows how to put stuff in the stamp whether they be strings, numbers or whatever.
 *	- the demo actually demonstrates that the information got to the object or exe file using
 *		various techniques.
 *	- the demo also puts the md5 checksum of the source into the object file which could be used to perfectly ascertain from which exact source was this object produced.
 *
 *	If you want to see more macros that the compiler provides just run:
 *	cpp -dM < /dev/null
 *
 * EXTRA_COMPILE_CMD=echo -DCHECKSUM=`md5sum {source} | cut -f 1 -d " "` -DHOST=`hostname` -DUSER=$USER -DSYSTEM=`uname -a | tr ' ' '-'`
 * EXTRA_COMPILE_FLAGS=-g3
 */

#define STRING_VERSION "1.23.45"
#define NUMERIC_VERSION 1.23.45

// this macro does two things:
// - makes sure that the compiler does not issue "unused variable" or tried to make
// the static variable go away just because no one is using it.
// - make sure that our data goes into it's own section of the object code.
#define SECTION ".compile_info"
#define ATTR __attribute__((section(SECTION), used))

static char buff[10000]={1};

// the static allows us to use a compiled on tag for each file so you can put it in a common
// header and get stamping for all files in your project.
static const char* ATTR id_file="id_file=" __FILE__;
static const char* ATTR id_base_file="id_base_file=" __BASE_FILE__;
static const char* ATTR id_host="id_host=" stringify(HOST);
static const char* ATTR id_user="id_user=" stringify(USER);
static const char* ATTR id_system="id_system=" stringify(SYSTEM);
static const char* ATTR id_date="id_date=" __DATE__;
static const char* ATTR id_time="id_time=" __TIME__;
static const char* ATTR id_timestamp="id_timestamp=" __TIMESTAMP__;
static const char* ATTR id_string_version="id_string_version=" STRING_VERSION;
static const char* ATTR id_numeric_version="id_numeric_version=" stringify(NUMERIC_VERSION);
static const char* ATTR id_checksum="id_checksum=" stringify(CHECKSUM);
static const char* ATTR id_version="id_version=" __VERSION__;

const char* script="src/examples/stamping_binaries/stamp.gdb";

int main(int argc, char** argv, char** envp) {
	printf("date is %s\n", __DATE__);
	printf("time is %s\n", __TIME__);
	printf("id_file is %s\n", id_file);
	printf("buff is %p\n", buff);
	printf("address of id_file is %p\n", &id_file);
	struct tm tm;
	CHECK_NOT_NULL(strptime(__DATE__ " " __TIME__, "%b %d %Y %H:%M:%S", &tm));
	time_t t=mktime(&tm);
	printf("t is %lu\n", t);

	const unsigned int len=1024;
	char cmd[len];

	// lets try to run strings(1) on our own binary to see the data...
	snprintf(cmd, len, "strings %s | grep id_ | grep -v grep", argv[0]);
	printf("\nrunning [%s]\n", cmd);
	CHECK_NOT_M1(system(cmd));

	// lets try to see the actual symbols using objdump(1)...
	snprintf(cmd, len, "objdump -C --section=" SECTION " -x %s | grep id_", argv[0]);
	printf("\nrunning [%s]\n", cmd);
	CHECK_NOT_M1(system(cmd));

	// lets dump our own core
	snprintf(cmd, len, "sudo gcore -o /tmp/core %d", getpid());
	printf("\nrunning [%s]\n", cmd);
	CHECK_NOT_M1(system(cmd));

	// lets run gdb to see if we can see our information from a debugging session
	snprintf(cmd, len, "gdb -c /tmp/core.%d -q %s -x %s", getpid(), argv[0], script);
	printf("\nrunning [%s]\n", cmd);
	CHECK_NOT_M1(system(cmd));

	sleep(3600);
	return EXIT_SUCCESS;
}
