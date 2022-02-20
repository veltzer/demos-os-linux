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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>	// for std::cout, std::endl

int main(int argc, char** argv, char** envp) {
	/*
	 * PCM* m=PCM::getInstance();
	 * // program counters, and on a failure just exit
	 * if(m->program()!=PCM::Success)
	 * return EXIT_FAILURE;
	 * SystemCounterState before_sstate=getSystemCounterState();
	 * long sum=0;
	 * for(int i=0;i++;i<1000000) {
	 * for(int j=0;j++;j<10000000) {
	 * int mul;
	 * if(i%2==0) {
	 * mul=-1;
	 * }
	 * sum+=mul*j*j;
	 * }
	 * }
	 * SystemCounterState after_sstate=getSystemCounterState();
	 * std::cout << "Instructions per clock:" << getIPC(before_sstate,after_sstate)
	 * << "L3 cache hit ratio:" << getL3CacheHitRatio(before_sstate,after_sstate)
	 * << "Bytes read:" << getBytesReadFromMC(before_sstate,after_sstate)
	 * << std::endl;
	 */
	return EXIT_SUCCESS;
}
