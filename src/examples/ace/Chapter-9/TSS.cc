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
#include <ace/config-lite.h>
#include <ace/Synch.h>
#include <ace/Task.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include "ClientContext.hh"

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

void* ClientContext::get_attribute(const char *name) {
	void* value=0;
	attributeMap_.find(name, value);
	return value;
}

void ClientContext::set_attribute(const char* name, void* value) {
	attributeMap_.bind(name, value);
}

class HA_CommandHandler: public ACE_Task<ACE_MT_SYNCH> {
public:
	virtual int svc(void) {
		ACE_thread_t tid=this->thr_mgr()->thr_self();
		// Set our identifier in TSS.
		ACE_DEBUG((LM_DEBUG, "(%t) Setting TSS TID: %u \n", tid));
		this->tss_ctx_->set_attribute("thread_id", &tid);
		while(handle_requests()>0) {
		}
		return 0;
	}
	int handle_requests(void) {
		ACE_thread_t *tid=(ACE_thread_t *)this->tss_ctx_->get_attribute("thread_id");
		ACE_DEBUG((LM_DEBUG, "(%t) Received TSS TID: %u \n", *tid));
		// do work.
		ACE_DEBUG((LM_DEBUG, "Doing some work for thread %t\n"));
		return -1;
	}

private:
	ACE_TSS<ClientContext> tss_ctx_;
};

int main() {
	HA_CommandHandler handler;
	handler.activate(THR_NEW_LWP | THR_JOINABLE, 5);
	handler.wait();
	return EXIT_SUCCESS;
}
