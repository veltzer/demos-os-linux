/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for fprintf(3), stderr, printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, exit(3)
#include <unistd.h>	// for usleep(3)
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * OpenGL example number 2...
 * Taken from:
 * http://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_Programming_Animations_with_GLX_and_Xlib
 *
 * EXTRA_LINK_FLAGS=-lX11 -lGL -lGLU
 */

// GLOBAL IDENTIFIERS
static Display* dpy;
static Window root, win;
static GLint att[]={
	GLX_RGBA,
	GLX_DEPTH_SIZE,
	24,
	GLX_DOUBLEBUFFER,
	None
};
static XVisualInfo* vi;
static GLXContext glc;
static Colormap cmap;
static XSetWindowAttributes swa;
static XWindowAttributes wa;

static float TimeCounter, LastFrameTimeCounter, DT, prevTime=0.0, FPS;
static struct timeval tv, tv0;
static int Frame=1, FramesPerFPS;

static GLfloat rotation_matrix[16];
static float rot_z_vel=50.0, rot_y_vel=30.0;
static bool do_debug=true;

// DRAW A CUBE
void DrawCube(float size) {
	if(do_debug) {
		printf("DrawCube\n");
	}
	glBegin(GL_QUADS);

	glColor3f(0.7, 0.0, 0.0);
	glVertex3f(-size, -size, -size);
	glVertex3f(+size, -size, -size);
	glVertex3f(+size, +size, -size);
	glVertex3f(-size, +size, -size);

	glVertex3f(-size, -size, +size);
	glVertex3f(+size, -size, +size);
	glVertex3f(+size, +size, +size);
	glVertex3f(-size, +size, +size);

	glColor3f(0.0, 0.0, 0.7);

	glVertex3f(-size, -size, -size);
	glVertex3f(-size, -size, +size);
	glVertex3f(-size, +size, +size);
	glVertex3f(-size, +size, -size);

	glVertex3f(+size, -size, -size);
	glVertex3f(+size, -size, +size);
	glVertex3f(+size, +size, +size);
	glVertex3f(+size, +size, -size);

	glColor3f(0.0, 0.7, 0.0);

	glVertex3f(-size, -size, -size);
	glVertex3f(-size, -size, +size);
	glVertex3f(+size, -size, +size);
	glVertex3f(+size, -size, -size);

	glVertex3f(-size, +size, -size);
	glVertex3f(-size, +size, +size);
	glVertex3f(+size, +size, +size);
	glVertex3f(+size, +size, -size);

	glEnd();
}

void RotateCube() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(rot_y_vel*DT, 0.0, 1.0, 0.0);
	glRotatef(rot_z_vel*DT, 0.0, 0.0, 1.0);
	glMultMatrixf(rotation_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
}

void ExposeFunc() {
	float aspect_ratio;
	char info_string[256];
	// RESIZE VIEWPORT
	XGetWindowAttributes(dpy, win, &wa);
	glViewport(0, 0, wa.width, wa.height);
	aspect_ratio=(float)(wa.width) / (float)(wa.height);
	// SETUP PROJECTION & MODELVIEW
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.50*aspect_ratio, 2.50*aspect_ratio, -2.50, 2.50, 1., 100.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(10., 0., 0., 0., 0., 0., 0., 0., 1.);
	glMultMatrixf(rotation_matrix);
	// DRAW CUBE
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawCube(1.0);
	// DISPLAY TIME, FPS etc.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (float)wa.width, 0, (float)wa.height, -1., 1.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	sprintf(info_string, "%4.1f seconds * %4.1f fps at %i x %i", TimeCounter, FPS, wa.width, wa.height);
	glRasterPos2i(10, 10);
	glCallLists(strlen(info_string), GL_UNSIGNED_BYTE, info_string);
	sprintf(info_string, "<up,down,left,right> rotate cube * <F1> stop rotation ");
	glRasterPos2i(10, wa.height-32);
	glCallLists(strlen(info_string), GL_UNSIGNED_BYTE, info_string);
	// SWAP BUFFERS
	glXSwapBuffers(dpy, win);
}

// CREATE A GL CAPABLE WINDOW
void CreateWindow() {
	if((dpy=XOpenDisplay(NULL))==NULL) {
		fprintf(stderr, "cannot connect to x server\n");
		exit(EXIT_FAILURE);
	}
	root=DefaultRootWindow(dpy);
	if((vi=glXChooseVisual(dpy, 0, att))==NULL) {
		fprintf(stderr, "no matching visual\n");
		exit(EXIT_FAILURE);
	}
	if((cmap=XCreateColormap(dpy, root, vi->visual, AllocNone))==0) {
		fprintf(stderr, "cannot create colormap\n");
		exit(EXIT_FAILURE);
	}
	swa.event_mask=KeyPressMask;
	swa.colormap=cmap;
	win=XCreateWindow(dpy, root, 0, 0, 700, 700, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	XStoreName(dpy, win, "OpenGL Animation");
	XMapWindow(dpy, win);
}

// SETUP GL CONTEXT
void SetupGL() {
	char font_string[128];
	XFontStruct* font_struct;
	// CREATE GL CONTEXT AND MAKE IT CURRENT
	if((glc=glXCreateContext(dpy, vi, NULL, GL_TRUE))==NULL) {
		fprintf(stderr, "cannot create gl context\n");
		exit(EXIT_FAILURE);
	}
	glXMakeCurrent(dpy, win, glc);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.00, 0.00, 0.40, 1.00);
	// FIND A FONT
	for(int font_size=14; font_size < 32; font_size+=2) {
		sprintf(font_string, "-adobe-courier-*-r-normal--%i-*", font_size);
		font_struct=XLoadQueryFont(dpy, font_string);
		if(font_struct!=NULL) {
			glXUseXFont(font_struct->fid, 32, 192, 32);
			break;
		}
	}
	// INITIALIZE ROTATION MATRIX
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, rotation_matrix);
}

// TIME COUNTER FUNCTIONS
void InitTimeCounter() {
	gettimeofday(&tv0, NULL);
	FramesPerFPS=30;
}

void UpdateTimeCounter() {
	LastFrameTimeCounter=TimeCounter;
	gettimeofday(&tv, NULL);
	TimeCounter=(float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));
	DT=TimeCounter-LastFrameTimeCounter;
}

void CalculateFPS() {
	Frame++;
	if((Frame%FramesPerFPS)==0) {
		FPS=((float)(FramesPerFPS)) / (TimeCounter-prevTime);
		prevTime=TimeCounter;
	}
}

// EXIT PROGRAM
void ExitProgram() {
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, glc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
	exit(EXIT_SUCCESS);
}

// CHECK EVENTS
void CheckKeyboard() {
	/* TODO: XKeysymToString is deprecated
	 * if(XCheckWindowEvent(dpy, win, KeyPressMask, &xev)) {
	 * char* key_string=XKeysymToString(XKeycodeToKeysym(dpy, xev.xkey.keycode, 0));
	 * if(strncmp(key_string, "Left", 4)==0) {
	 * rot_z_vel-=200.0*DT;
	 * } else if(strncmp(key_string, "Right", 5)==0) {
	 * rot_z_vel+=200.0*DT;
	 * } else if(strncmp(key_string, "Up", 2)==0) {
	 * rot_y_vel-=200.0*DT;
	 * } else if(strncmp(key_string, "Down", 4)==0) {
	 * rot_y_vel+=200.0*DT;
	 * } else if(strncmp(key_string, "F1", 2)==0) {
	 * rot_y_vel=0.0;
	 * rot_z_vel=0.0;
	 * } else if(strncmp(key_string, "Escape", 5)==0) {
	 * ExitProgram();
	 * }
	 * }
	 */
}

// MAIN PROGRAM
int main(int argc, char** argv, char** envp) {
	CreateWindow();
	SetupGL();
	InitTimeCounter();
	while(true) {
		UpdateTimeCounter();
		CalculateFPS();
		RotateCube();
		ExposeFunc();
		CHECK_NOT_M1(usleep(1000));
		CheckKeyboard();
	}
	return EXIT_SUCCESS;
}
