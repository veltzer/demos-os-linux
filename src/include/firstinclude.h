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

#pragma once

/* THIS IS C FILE, NO C++ here */

/*
 * This is the first file you should include from user space apps
 */

/*
 * needed for
 * SCHED_IDLE
 * SCHED_BATCH
 * sched_getcpu()
 * O_LARGEFILE
 * DN_* dnotify constants
 * SYS_* syscall constants
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif	// _GNU_SOURCE

/*
 * needed for
 * big files
 */
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif	// _LARGEFILE64_SOURCE

/*
 * needed for
 * REG_EIP from ucontext.h
 */
#ifndef __USE_GNU
#define __USE_GNU
#endif	// __USE_GNU

/*
 * needed for
 * MAP_ANONYMOUS definition from <sys/mman.h>
 */
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif	// _DEFAULT_SOURCE
