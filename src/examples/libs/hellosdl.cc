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
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags sdl gl glu
 * EXTRA_LINK_CMD=pkg-config --libs sdl gl glu
 */

const int SCREEN_WIDTH=640;
const int SCREEN_HEIGHT=480;
const int SCREEN_BPP=16;

static SDL_Surface *surface;

void Quit(int returnCode) {
	SDL_Quit();
	exit(returnCode);
}

void handleKeyPress(SDL_keysym *keysym) {
	switch(keysym->sym) {
	case SDLK_ESCAPE:
		Quit(0);
		break;
	default:
		break;
	}
}

int resizeWindow(int width, int height) {
	if (height==0) {
		height=1;
	}
	GLfloat ratio=(GLfloat)width / (GLfloat)height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return true;
}

int initGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return true;
}

int drawGLScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glBegin(GL_TRIANGLES);	/* Drawing Using Triangles */
	glColor3f(1.0f, 0.0f, 0.0f);	/* Red */
	glVertex3f(0.0f, 1.0f, 0.0f);	/* Top Of Triangle */
	glColor3f(0.0f, 1.0f, 0.0f);	/* Green */
	glVertex3f(-1.0f, -1.0f, 0.0f);	/* Left Of Triangle */
	glColor3f(0.0f, 0.0f, 1.0f);	/* Blue */
	glVertex3f(1.0f, -1.0f, 0.0f);	/* Right Of Triangle */
	glEnd();/* Finished Drawing The Triangle */
	glTranslatef(3.0f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.5f, 1.0f);
	glBegin(GL_QUADS);
	/* Draw A Quad */
	glVertex3f(1.0f, 1.0f, 0.0f);	/* Top Right Of The Quad */
	glVertex3f(-1.0f, 1.0f, 0.0f);	/* Top Left Of The Quad */
	glVertex3f(-1.0f, -1.0f, 0.0f);	/* Bottom Left Of The Quad */
	glVertex3f(1.0f, -1.0f, 0.0f);	/* Bottom Right Of The Quad */
	glEnd();/* Done Drawing The Quad */
	SDL_GL_SwapBuffers();
	return true;
}

int main(int argc, char** argv, char** envp) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		Quit(1);
	}
	const SDL_VideoInfo *videoInfo=SDL_GetVideoInfo();
	if (!videoInfo) {
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		Quit(1);
	}
	int videoFlags=SDL_OPENGL;	/* Enable OpenGL in SDL */
	videoFlags|=SDL_GL_DOUBLEBUFFER;/* Enable double buffering */
	if (videoInfo->hw_available) {
		videoFlags|=SDL_HWSURFACE;
	} else {
		videoFlags|=SDL_SWSURFACE;
	}
	if (videoInfo->blit_hw) {
		videoFlags|=SDL_HWACCEL;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	surface=SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags);
	if (!surface) {
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		Quit(1);
	}
	initGL();
	resizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	drawGLScene();
	bool done=false;
	while(!done) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				handleKeyPress(&event.key.keysym);
				break;
			case SDL_QUIT:
				done=true;
				break;
			default:
				break;
			}
		}
	}
	Quit(0);
	return EXIT_SUCCESS;
}
