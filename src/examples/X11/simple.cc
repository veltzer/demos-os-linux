/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <X11/Xlib.h>
#include <err_utils.h>

/*
 * Simple X11 exapmle
 *
 * EXTRA_LINK_FLAGS_AFTER=-lX11
 */

int main() {
	Display* display = (Display*)CHECK_NOT_NULL(XOpenDisplay(NULL));
	int screen = DefaultScreen(display);
	Window window = XCreateSimpleWindow(
		display,
		RootWindow(display, screen),
		10, 10, 400, 300, 1,
		BlackPixel(display, screen),
		WhitePixel(display, screen)
		);
	XSelectInput(display, window, ExposureMask | KeyPressMask);
	XMapWindow(display, window);
	while (true) {
		XEvent event;
		XNextEvent(display, &event);
		if (event.type == Expose) {
			/* Handle window exposure */
			XFillRectangle(display, window, DefaultGC(display, screen), 20, 20, 100, 50);
		} else if (event.type == KeyPress) {
			/* Handle key press */
			break;
		}
	}
	XCloseDisplay(display);
	return EXIT_SUCCESS;
}
