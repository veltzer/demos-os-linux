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

#ifndef TASK_H
#define TASK_H

#include <firstinclude.h>
#include <ace/Task.h>
#include <ace/Barrier.h>

/*
 * This is our basic thread-pool Task. We have a choice of pool size
 * on the open() and the usual svc() and close() methods.
 *
 * A new addition is the ACE_Barrier object. This will allow the
 * synchronization of our svc() methods so that they all start at the
 * "same" time. The normal case may allow one thread to start working
 * earlier than others. There's no real harm in it but you can get
 * better "work by thread" statistics if they start out together.
 */
class Task : public ACE_Task<ACE_MT_SYNCH> {
public:

	typedef ACE_Task<ACE_MT_SYNCH> inherited;

	Task(void);
	~Task(void);

	/*
	 * I really wanted this to be called open() but that was already
	 * claimed by the Task framework. start() will kick off our thread
	 * pool for us.
	 */
	int start(int threads=1);

	virtual int svc(void);

	virtual int close(u_long flags=0);

protected:
	ACE_Barrier *barrier_;
};

#endif
