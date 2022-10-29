/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/Task.h>
#include <ace/Synch.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class HA_Device_Repository {
public:
	HA_Device_Repository() : owner_(0) {
	}
	int is_free(void) {
		return(this->owner_==0);
	}
	int is_owner(ACE_Task_Base *tb) {
		return(this->owner_==tb);
	}
	ACE_Task_Base* get_owner(void) {
		return(this->owner_);
	}
	void set_owner(ACE_Task_Base *owner) {
		this->owner_=owner;
	}
	int update_device(int device_id);

private:
	ACE_Task_Base* owner_;
};

class HA_CommandHandler : public ACE_Task_Base {
public:
	enum {
		NUM_USES=10
	};
	HA_CommandHandler(HA_Device_Repository & rep, ACE_Condition_Thread_Mutex& wait, ACE_Thread_Mutex & mutex) : rep_(rep), waitCond_(wait), mutex_(mutex) {
	}
	virtual int svc(void);

private:
	HA_Device_Repository& rep_;
	ACE_Condition_Thread_Mutex& waitCond_;
	ACE_Thread_Mutex& mutex_;
};

int HA_CommandHandler::svc(void) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Handler Thread running\n")));
	for(int i=0; i<NUM_USES; i++) {
		this->mutex_.acquire();
		while(!this->rep_.is_free()) {
			this->waitCond_.wait();
		}
		this->rep_.set_owner(this);
		this->mutex_.release();
		this->rep_.update_device(i);
		ACE_ASSERT(this->rep_.is_owner(this));
		this->rep_.set_owner(0);
		this->waitCond_.signal();
	}
	return(0);
}

int HA_Device_Repository::update_device(int device_id) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Updating device %d\n"), device_id));
	ACE_OS::sleep(1);
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	HA_Device_Repository rep;
	ACE_Thread_Mutex rep_mutex;
	// FUZZ: disable check_for_lack_ACE_OS
	ACE_Condition_Thread_Mutex wait(rep_mutex);
	// FUZZ: enable check_for_lack_ACE_OS
	HA_CommandHandler handler1(rep, wait, rep_mutex);
	HA_CommandHandler handler2(rep, wait, rep_mutex);
	handler1.activate();
	handler2.activate();
	handler1.wait();
	handler2.wait();
	return EXIT_SUCCESS;
}
