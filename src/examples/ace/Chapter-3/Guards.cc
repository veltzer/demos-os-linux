/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/config-lite.h>
#include <ace/OS_main.h>
#include <ace/OS_Memory.h>
#include <ace/Guard_T.h>
#include <ace/Log_Msg.h>
#include <ace/Thread_Mutex.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

// This file exists primarily to get code into the book to show different
// ways to do the same thing. For complete context and explanation, please
// see APG chapter 12.

class HA_Device_Repository {
public:
	int update_device(int device_id);

private:
	ACE_Thread_Mutex mutex_;
};

class Object {
};
static Object *object;

int HA_Device_Repository::update_device(int /* device_id */) {
	ACE_GUARD_RETURN(ACE_Thread_Mutex, mon, mutex_, -1);

	ACE_NEW_RETURN(object, Object, -1);
	// Use the object.
	// ...
	return(0);
}

#if 0
// This is less-desired way to do this...

int HA_Device_Repository::update_device(int device_id) {
	this->mutex_.acquire();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Updating device %d\n"), device_id));

	// Allocate a new object.
	ACE_NEW_RETURN(object, Object, -1);
	// ...
	// Use the object

	this->mutex_.release();
}

int HA_Device_Repository::update_device(int device_id) {
	// Construct a guard specifying the type of the mutex as
	// a template parameter and passing in the mutex to hold
	// as a parameter.
	ACE_Guard<ACE_Thread_Mutex> guard(this->mutex_);

	// This can throw an exception that is not caught here.
	ACE_NEW_RETURN(object, Object, -1);
	// ..
	// Use the object.
	// ..
	// Guard is destroyed, automatically releasing the lock.
}

#endif

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	HA_Device_Repository rep;
	rep.update_device(42);
	return EXIT_SUCCESS;
}
