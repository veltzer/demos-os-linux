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
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, exit(3)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

/*
 * OpenGL example number 1...
 * Taken from http://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib
 *
 * EXTRA_LINK_FLAGS=-lX11 -lGL -lGLU
 */

static Display* dpy;
static Window root;
static GLint att[]={ GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static XVisualInfo* vi;
static Colormap cmap;
static XSetWindowAttributes swa;
static Window win;
static GLXContext glc;
static XWindowAttributes gwa;
static XEvent xev;

void DrawAQuad() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	glBegin(GL_QUADS);
	glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
	glColor3f(0., 1., 0.); glVertex3f(+.75, -.75, 0.);
	glColor3f(0., 0., 1.); glVertex3f(+.75, +.75, 0.);
	glColor3f(1., 1., 0.); glVertex3f(-.75, +.75, 0.);
	glEnd();
}

int main(int argc, char** argv, char** envp) {
	dpy=XOpenDisplay(NULL);
	if(dpy==NULL) {
		fprintf(stderr, "cannot connect to X server\n");
		exit(EXIT_FAILURE);
	}
	root=DefaultRootWindow(dpy);
	vi=glXChooseVisual(dpy, 0, att);
	if(vi==NULL) {
		fprintf(stderr, "no appropriate visual found\n");
		exit(EXIT_FAILURE);
	}
	// printf("tvisual %p selected\n", (void *)vi->visualid); }/* %p creates hexadecimal output like in glxinfo */
	cmap=XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap=cmap;
	swa.event_mask=ExposureMask | KeyPressMask;
	win=XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
	glc=glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	glEnable(GL_DEPTH_TEST);
	while(true) {
		XNextEvent(dpy, &xev);
		if(xev.type==Expose) {
			printf("got expose\n");
			XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);
			DrawAQuad();
			glXSwapBuffers(dpy, win);
		} else if(xev.type==KeyPress) {
			printf("got key press\n");
			glXMakeCurrent(dpy, None, NULL);
			glXDestroyContext(dpy, glc);
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
			exit(EXIT_SUCCESS);
		}
	}
	return EXIT_SUCCESS;
}
