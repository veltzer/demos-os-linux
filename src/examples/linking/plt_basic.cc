/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3), snprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <proc_utils.h>	// for proc_print_mmap_self()

/*
 * This example shows the addresses of functions not yet resolved which are in the plt.
 * as you can see the addresses of the functions are constant at every invocation and
 * are not affected by ASLR (Address Space Layout Randomization).
 * They are also part of the executable and not part of the shared library. That is
 * the PLT.
 * You can also see that the PLT is part of the executable part of your executable, not
 * any of the data segments (the read only or the read/write ones).
 * That is because you jump to it and from it to the real function you are trying to call.
 *
 * TODO:
 * - disassemble the code at the PLT addresses.
 */

int main(int argc, char** argv, char** envp) {
	typeof(printf)* p_printf=&printf;
	typeof(snprintf)* p_snprintf=&snprintf;
	typeof(atoi)* p_atoi=&atoi;
	printf("&printf is %p\n", p_printf);
	printf("&snprintf is %p\n", p_snprintf);
	printf("&atoi is %p\n", p_atoi);
	proc_print_mmap_self();
	return EXIT_SUCCESS;
}
