/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/OS_NS_string.h>	// for ACE_OS::strlen,strcpy
#include <ace/MMAP_Memory_Pool.h>	// for the allocator
#include <ace/Malloc_T.h>	// for ACE_Malloc
#include <ace/Null_Mutex.h>	// for ACE_Null_Mutex

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

typedef ACE_Malloc<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex> ALLOCATOR;
typedef ACE_Malloc_LIFO_Iterator<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex> MALLOC_LIFO_ITERATOR;
ALLOCATOR *g_allocator;

class Record {
private:
	int id1_;
	int id2_;
	char *name_;

public:
	Record(int id1, int id2, char *name)
		: id1_(id1), id2_(id2), name_(0) {
		size_t len=ACE_OS::strlen(name) + 1;
		this->name_=reinterpret_cast<char *>(g_allocator->malloc(len));
		ACE_OS::strcpy(this->name_, name);
	}
	~Record() {
		g_allocator->free(name_);
	}
	char *name(void) {
		return(name_);
	}
	int id1(void) {
		return(id1_);
	}
	int id2(void) {
		return(id2_);
	}
};

void showRecords() {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("The following records were found:\n")));
	MALLOC_LIFO_ITERATOR iter(*g_allocator);
	for(void* temp=0; iter.next(temp)!=0; iter.advance()) {
		Record *record=reinterpret_cast<Record *>(temp);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Record name: %C|id1:%d|id2:%d\n"), record->name(), record->id1(), record->id2()));
	}
}

int addRecords() {
	for(int i=0; i<10; i++) {
		char buf[64];
		ACE_OS::sprintf(buf, "%s:%d", "Record", i);
		void *memory=g_allocator->malloc(sizeof(Record));
		if (memory==0) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("Unable to malloc")), -1);
		}
		// Allocate and place record
		Record *newRecord=new(memory) Record(i, i + 1, buf);
		ACE_UNUSED_ARG(newRecord);
		if (g_allocator->bind(buf, newRecord)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("bind failed")), -1);
		}
	}
	return(0);
}

// Backing file where the data is kept.
#define BACKING_STORE ACE_TEXT("backing.store")

int ACE_TMAIN(int argc, ACE_TCHAR *[]) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Starting Program\n")));
	ACE_NEW_RETURN(g_allocator, ALLOCATOR(BACKING_STORE), -1);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("After g_allocator\n")));
	if (argc > 1) {
		showRecords();
	} else {
		addRecords();
		g_allocator->sync();
	}
	delete g_allocator;
	return(0);
}
