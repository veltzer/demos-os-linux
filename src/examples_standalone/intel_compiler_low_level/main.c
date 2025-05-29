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
#include <stdio.h> // for printf(3)
#include <stdlib.h> // for EXIT_SUCCESS

/*
* This example shows the synchronization primitives of the intel compiler
*/

int main(int argc,char** argv,char** envp) {
	// don't remove the prints, they make the disassembly easier to understand...
	printf("===================================\n");

	// this is a compiler reordering barrier in the intel compiler...
	// this does not emit any assembly...
	__memory_barrier();

	printf("===================================\n");

	// this is a machine reordering barrier, which is also a compiler barrier
	// (because the documentation says it is, not because every machine barrier
	// is also a compiler reordering barrier)
	__sync_synchronize();

	printf("===================================\n");

	// here is a compare and swap that succeeds...
	int a=4;
	int old_val=__sync_val_compare_and_swap(&a,4,5);
	printf("old_val is %d and a is %d\n",old_val,a);

	printf("===================================\n");

	// here is a compare and swap that fails...
	a=17;
	old_val=__sync_val_compare_and_swap(&a,4,5);
	printf("old_val is %d and a is %d\n",old_val,a);

	printf("===================================\n");
	return EXIT_SUCCESS;
}
