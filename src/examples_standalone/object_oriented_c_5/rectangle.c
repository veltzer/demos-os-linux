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

#include "rectangle.h"
#include <stdlib.h>
#include <stdio.h>

typedef void (*method)(rectangle*, ...);

static method vtable[];

typedef struct _rectangle {
	method* vtable;
	int width;
	int height;
} rectangle;

rectangle* create_rectangle(const int height, const int width) {
	rectangle* this=(rectangle*)malloc(sizeof(rectangle));
	this->vtable=vtable;
	this->height=height;
	this->width=width;
	return this;
}

static void destroy(rectangle* r) {
	free(r);
}

static int get_width(const rectangle * r) {
	return r->width;
}

static int get_height(const rectangle * r) {
	return r->height;
}

static void set_width(rectangle * r, const int width) {
	r->width=width;
}

static void set_height(rectangle * r, const int height) {
	r->height=height;
}

static int area(const rectangle * r) {
	return r->height*r->width;
}

static method vtable[]={
	(method)get_width,
	(method)get_height,
	(method)set_width,
	(method)set_height,
	(method)area,
	(method)destroy,
};
