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

#ifndef __THREAD_SPECIFIC_H
#define __THREAD_SPECIFIC_H

#include <firstinclude.h>
#include <ace/Guard_T.h>
#include <ace/Thread_Mutex.h>

// Define a class that will be stored in thread-specific data. Note
// that as far as this class is concerned it's just a regular C++
// class. The ACE_TSS wrapper transparently ensures that objects of
// this class will be placed in thread-specific storage. All calls on
// ACE_TSS::operator->() are delegated to the appropriate method in
// the Errno class.

class Errno {
public:
	int error(void) {
		return this->errno_;
	}
	void error(int i) {
		this->errno_=i;
	}
	int line(void) {
		return this->lineno_;
	}
	void line(int l) {
		this->lineno_=l;
	}
	// Errno::flags_ is a static variable, so we've got to protect it
	// with a mutex since it isn't kept in thread-specific storage.
	int flags(void) {
		ACE_GUARD_RETURN(ACE_Thread_Mutex, ace_mon, Errno::lock_, -1);
		return Errno::flags_;
	}
	void flags(int f) {
		ACE_GUARD(ACE_Thread_Mutex, ace_mon, Errno::lock_);
		Errno::flags_=f;
	}

private:
	// =errno_ and lineno_ will be thread-specific data so they don't
	// need a lock.
	int errno_;
	int lineno_;
	static int flags_;
	// flags_ needs a lock.
	static ACE_Thread_Mutex lock_;
};

#endif	/* __THREAD_SPECIFIC_H */
