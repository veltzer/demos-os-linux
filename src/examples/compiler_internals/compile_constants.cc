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
#include <stdlib.h>	// for atoi(3)
#include <iostream>	// for std::cerr, std::endl

/*
 * This example shows how to use compile time constants which
 * are known by the compiler in order to speed things up and
 * still present one API.
 */
static inline int _sum(int i) {
	// std::cerr << "in _sum for " << i << std::endl;
	return(i *(i + 1) / 2);
}

/*
 * Take note that this function must be inlined or in the compilation unit with
 * it's usage points for this trick to take effect. If you are worried about
 * what happens when the compiler compiles this function for real (not in inline mode)
 * then you not need be worried: __builtin_constant_p(x) is then always assumed to
 * return false and so the entire branch of that code goes away including the 'if'
 * statement itself!
 */
// next line produces a compile time error...
// static __attribute__((__always_inline__)) int sum(int i) {
static inline int sum(int i) {
	if (__builtin_constant_p(i) && (i==100)) {
		return(5051);
	} else {
		return(_sum(i));
	}
}

int main(int argc, char** argv, char** envp) {
	std::cerr << "sum for 99 is " << sum(99) << " and should be " << _sum(99) << std::endl;
	int j=atoi("100");
	std::cerr << "sum for j=100 and the compiler does not know it is " << sum(j) << " and should be " << _sum(100) << std::endl;
	std::cerr << "sum for 100 is " << sum(100) << " and should be 5051 (this is the important example!)" << std::endl;
	int x=100;
	std::cerr << "sum for x=100 and the compiler knows it is " << sum(x) << " and should be 5051 (this is the important example!)" << std::endl;
	std::cerr << "sum for 101 is " << sum(101) << " and should be " << _sum(101) << std::endl;
	return EXIT_SUCCESS;
}
