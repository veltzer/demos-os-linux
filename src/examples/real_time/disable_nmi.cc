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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/io.h> // for inb(), outb(), ioperm(2)
#include <err_utils.h> // for CHECK_NOT_M1()

/*
 * This example disables Non-Maskable-Interrupts (NMIs)
 * see:
 * https://stackoverflow.com/questions/55394608/how-do-i-disable-non-maskable-interrupts-programmatically
 */

void NMI_enable() {
	outb(0x70, inb(0x70) & 0x7F);
}

void NMI_disable() {
	outb(0x70, inb(0x70) | 0x80);
}

void NMI_set_permissions() {
	CHECK_NOT_M1(ioperm(0x70, 8, 1));
}

int main(int argc, char** argv, char** envp) {
	NMI_set_permissions();
	NMI_disable();
	return EXIT_SUCCESS;
}
