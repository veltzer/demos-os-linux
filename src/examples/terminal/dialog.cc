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
#include <dialog.h>	// for init_dialog(3), end_dialog(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to use the dialog C library.
 *
 * EXTRA_LINK_FLAGS_AFTER=/usr/lib/x86_64-linux-gnu/libdialog.a -lncursesw
 *
 * TODO:
 * - you may want to write this ?!?
 */

int main() {
	init_dialog(NULL, NULL);
	end_dialog();
	return EXIT_SUCCESS;
}
