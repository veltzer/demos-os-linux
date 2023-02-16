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
#include <sys/types.h>	// for open(2), O_RDONLY(int)
#include <sys/stat.h>	// for open(2), O_RDONLY(int)
#include <fcntl.h>	// for open(2), O_RDONLY(int)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <errno.h>	// for errno(3)
#include <string.h>	// for strerror(3)
#include <error.h>	// for error(3)

/*
 * This shows how to create a macro to help you deal with errors returned
 * from system calls.
 *
 * NOTES:
 * - we emit the code of the offending line.
 * - we copy errno immediately to preserve the original error.
 * - we print errors to stderr and not stdout to correctly separate errors and output.
 * - we do not use strerror(3) to print error explanations and not just numbers because err(3) already does this for us.
 * - we surround the macro with 'do {...} while(0)' for preprocessing reasons.
 * - we do not use err(3) to print the error since it does not accept a custom errno.
 * - we do use error(3) to print the error since it does a better job than err(3).
 * - we do not pass newline to error(3) since it will append the reason for the error and a newline itself.
 */

#define CHK_NOT_M1(a) do { int ret=a; if(ret==-1) { int myerrno=errno; error(EXIT_FAILURE, myerrno, "error %s", "" # a); } } while(0)

int main(int argc, char** argv, char** envp) {
	CHK_NOT_M1(open("thisfiledoesnotexist", O_RDONLY));
	return EXIT_SUCCESS;
}
