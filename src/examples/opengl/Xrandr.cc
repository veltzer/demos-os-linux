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
#include <stdio.h>
#include <assert.h>	// for assert(3)
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	// for usleep(3)
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * OpenGL example number 3...
 * Taken from http://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_Changing_the_Screen_Resolution
 *
 * EXTRA_LINK_FLAGS_AFTER=-lX11 -lXrandr
 */

int main() {
	//
	// CONNECT TO X-SERVER, GET ROOT WINDOW ID
	//
	Display* dpy=XOpenDisplay(NULL);
	assert(dpy!=NULL);
	/*
	 * TODO:
	 * - How do I know if the next function call was successful?
	 */
	Window root=DefaultRootWindow(dpy);
	int event_base_return, error_base_return;
	Bool res=XRRQueryExtension(dpy, &event_base_return, &error_base_return);
	assert(res);
	int major_version_return, minor_version_return;
	Status s=XRRQueryVersion(dpy, &major_version_return, &minor_version_return);
	assert(s);
	printf("major_version_return, minor_version_return is %d,%d\n", major_version_return, minor_version_return);
	//
	// GET POSSIBLE SCREEN RESOLUTIONS
	//
	int num_sizes;
	XRRScreenSize* xrrs=XRRSizes(dpy, 0, &num_sizes);
	//
	// LOOP THROUGH ALL POSSIBLE RESOLUTIONS,
	// GETTING THE SELECTABLE DISPLAY FREQUENCIES
	//
	// to store all possible frequencies...
	short possible_frequencies[64][64];
	for(int i=0; i < num_sizes; i++) {
		printf("%i: %i x %i (%immx%imm)", i, xrrs[i].width, xrrs[i].height, xrrs[i].mwidth, xrrs[i].mheight);
		// print the rates of each size
		int num_rates;
		short* rates=XRRRates(dpy, 0, i, &num_rates);
		for(int j=0; j<num_rates; j++) {
			possible_frequencies[i][j]=rates[j];
			printf(" %i", rates[j]);
		}
	}
	printf("\n");
	//
	// GET CURRENT RESOLUTION AND FREQUENCY
	//
	XRRScreenConfiguration* conf=XRRGetScreenInfo(dpy, root);
	short original_rate=XRRConfigCurrentRate(conf);
	Rotation original_rotation;
	SizeID original_size_id=XRRConfigCurrentConfiguration(conf, &original_rotation);

	printf("CURRENT SIZE ID: %i\n", original_size_id);
	printf("CURRENT ROTATION: %i \n", original_rotation);
	printf("CURRENT RATE: %i Hz\n\n", original_rate);

	assert(num_sizes>1);
	//
	// CHANGE RESOLUTION
	//
	printf("CHANGED TO %i x %i PIXELS, %i Hz\n\n", xrrs[1].width, xrrs[1].height, possible_frequencies[1][0]);
	XRRSetScreenConfigAndRate(dpy, conf, root, 1, RR_Rotate_0, possible_frequencies[1][0], CurrentTime);
	//
	// SLEEP A WHILE
	//
	CHECK_NOT_M1(usleep(6000000));
	//
	// RESTORE ORIGINAL CONFIGURATION
	//
	printf("RESTORING %i x %i PIXELS, %i Hz\n\n", xrrs[original_size_id].width, xrrs[original_size_id].height, original_rate);
	XRRSetScreenConfigAndRate(dpy, conf, root, original_size_id, original_rotation, original_rate, CurrentTime);
	//
	// EXIT
	//
	assert(XCloseDisplay(dpy));
	return EXIT_SUCCESS;
}
