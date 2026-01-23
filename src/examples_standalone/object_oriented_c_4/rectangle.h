/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

typedef struct _rectangle rectangle;

rectangle* create_rectangle(const int width, const int height);
int sizeof_rectangle();
void construct_rectangle(rectangle*, const int width, const int height);
void rectangle_destroy(rectangle*);
int rectangle_get_width(const rectangle * r);
int rectangle_get_height(const rectangle * r);
void rectangle_set_width(rectangle * r, const int width);
void rectangle_set_height(rectangle * r, const int height);
int rectangle_area(const rectangle * r);
