/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __CLIENTCONTEXT_H_
#define __CLIENTCONTEXT_H_

#include <firstinclude.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Synch.h>

typedef ACE_Hash_Map_Manager<const char*, void*, ACE_Null_Mutex> Map;

// Client-specific context information.
class ClientContext {
public:
	void *get_attribute(const char *name);
	void set_attribute(const char *name, void *value);

private:
	Map attributeMap_;
};

#endif	/* __CLIENTCONTEXT_H_ */
