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

#include "rectangle.h"
#include <stdlib.h>

typedef struct _rectangle {
	int width;
	int height;
} rectangle;

int sizeof_rectangle() {
	return sizeof(rectangle);
}

rectangle* create_rectangle(const int height, const int width) {
	rectangle* this=(rectangle*)malloc(sizeof(rectangle));
	construct_rectangle(this, height, width);
	return this;
}

void construct_rectangle(rectangle* this, const int height, const int width) {
	this->height=height;
	this->width=width;
}

void rectangle_destroy(rectangle* r) {
	free(r);
}

int rectangle_get_width(const rectangle * r) {
	return r->width;
}

int rectangle_get_height(const rectangle * r) {
	return r->height;
}

void rectangle_set_width(rectangle * r, const int width) {
	r->width=width;
}

void rectangle_set_height(rectangle * r, const int height) {
	r->height=height;
}

int rectangle_area(const rectangle * r) {
	return r->height*r->width;
}
