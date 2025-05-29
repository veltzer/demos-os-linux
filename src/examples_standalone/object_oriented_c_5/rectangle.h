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

#pragma once

typedef struct _rectangle rectangle;

rectangle* create_rectangle(const int width, const int height);

#define METHOD_OF_TYPE(t) typedef t (*method_ ## t)(rectangle*, ...)
#define TYPE_OF_METHOD(t) method_ ## t
METHOD_OF_TYPE(int);
METHOD_OF_TYPE(void);

enum methods {
	m_get_width, // int
	m_get_height, // int
	m_set_width, // void
	m_set_height, // void
	m_area, // int
	m_destroy, // void
};

#define CALL_METHOD(o, m, t, args...) ((((TYPE_OF_METHOD(t)*)(*((void**)o))))[m])(o, ## args)
