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
#include <multiproc_utils.h>	// for my_system()
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * You can nm this file and see the weak symbol that is produced
 * by the compiler.
 */

template<class T> class A {
	public:
		int foo(T t) __attribute__((noinline));
};

template<class T> int A<T>::foo(T t) {
	int sum=0;
	for(unsigned int i=0;i<100;i++) {
		sum+=i*i;
	}
	return sum;
}

template class A<int>;

int main(int argc, char** argv, char** envp) {
	//A<int> a;
	//int sum=a.foo(0);
	//return sum;
	my_system("nm %s -C | grep foo", argv[0]);
	return EXIT_SUCCESS;
}
