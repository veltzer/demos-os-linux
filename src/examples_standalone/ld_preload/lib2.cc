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
#include <dlfcn.h>	// for dlopen(3), dlclose(3), dlsym(3)
#include <trace_utils.h>// TRACE()
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_ZERO()

/*
 * Static handle to the library and to the function
 */

// static void* handle=NULL;
static double(*psin)(double);

/* Initialization function for this library
 * This function will load the real library using dlopen et al...
 */
static void __attribute__((constructor)) init(void) {
	TRACE("start");
	// handle=dlopen("/lib/tls/i686/cmov/libm.so.6", RTLD_LAZY);
	// handle=dlopen("/lib/libm-2.12.1.so", RTLD_LAZY);
	// handle=CHECK_NOT_NULL(dlopen(NULL, RTLD_LAZY | RTLD_NEXT));
	void* sym=CHECK_NOT_NULL(dlsym(RTLD_NEXT, "sin"));
	psin=(double(*)(double))sym;
	TRACE("end");
}

static void __attribute__((destructor)) fini(void) {
	TRACE("start");
	// CHECK_ZERO(dlclose(handle));
	TRACE("end");
}

extern "C" double sin(double x) {
	return psin(x)*2;
}
