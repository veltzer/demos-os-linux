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
#include <gtk/gtk.h>	// for gtk_init(3), gtk_window_new(3), gtk_widget_show(3), gtk_main(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a simple hello gtk program
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags gtk+-2.0
 * EXTRA_LINK_CMD=pkg-config --libs gtk+-2.0
 * EXTRA_COMPILE_FLAGS=-Wno-deprecated-declarations
 */

int main(int argc, char** argv) {
	gtk_init(&argc, &argv);
	GtkWidget* w=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_show(w);
	gtk_main();
	return EXIT_SUCCESS;
}
