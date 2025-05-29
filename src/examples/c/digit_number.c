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
#include <stdio.h>	// for printf(3), scanf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_INT()

int digit_num(long int l) {
	int digits;
	if(l<0) {
		digits=1;
		l=-l;
	} else {
		digits=0;
	}
	while(l>0) {
		l/=10;
		digits++;
	}
	return digits;
}

long int get_number() {
	long int ret;
	CHECK_INT(scanf("%ld", &ret), 1);
	return ret;
}

int main() {
	printf("enter the first number: ");
	long int num1=get_number();
	printf("enter the second number: ");
	long int num2=get_number();
	long int mul=num1*num2;
	printf("num1 is %ld, %d\n", num1, digit_num(num1));
	printf("num2 is %ld, %d\n", num2, digit_num(num2));
	printf("mul is %ld, %d\n", mul, digit_num(mul));
	return EXIT_SUCCESS;
}
