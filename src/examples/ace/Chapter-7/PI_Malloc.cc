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

#include <firstinclude.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>
#include <ace/MMAP_Memory_Pool.h>
#include <ace/Malloc_T.h>
#include <ace/Null_Mutex.h>
#include <ace/PI_Malloc.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

typedef ACE_Malloc_T<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex, ACE_PI_Control_Block>
	ALLOCATOR;
typedef ACE_Malloc_LIFO_Iterator_T<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex, ACE_PI_Control_Block>
	MALLOC_LIFO_ITERATOR;

ALLOCATOR *g_allocator;
class Record{
public:
	Record(int id1, int id2, char *name)
		: id1_(id1), id2_(id2) {
		size_t len=ACE_OS::strlen(name) + 1;
		char* buf=reinterpret_cast<char *>(g_allocator->malloc(len));

		ACE_OS::strcpy(buf, name);
		name_=buf;
	}

	~Record() {
		g_allocator->free(name_.addr());
	}

	char *name(void) {
		return name_;
	}

	int id1(void) {
		return id1_;
	}

	int id2(void) {
		return id2_;
	}

private:
	int id1_;
	int id2_;
	ACE_Based_Pointer_Basic<char> name_;
};

void showRecords(void) {
	ACE_DEBUG((LM_DEBUG, "The following records were found:\n"));

	{
		MALLOC_LIFO_ITERATOR iter(*g_allocator);
		for(void* temp=0; iter.next(temp)!=0; iter.advance()) {
			Record *record=reinterpret_cast<Record *>(temp);
			ACE_DEBUG((LM_DEBUG, "Record name: %C|id1:%d|id2:%d\n", record->name(), record->id1(), record->id2()));
		}
	}
}

int addRecords(void) {
	char buf[32];
	for(int i=0; i<10; i++) {
		ACE_OS::sprintf(buf, "%s:%d", "Record", i);
		void* memory=g_allocator->malloc(sizeof(Record));
		if (memory==0) {
			ACE_ERROR_RETURN((LM_ERROR, "%p\n", "Unable to malloc"), -1);
		}
		// Allocate and place record
		Record *newRecord=new(memory) Record(i, i + 1, buf);
		if (g_allocator->bind(buf, newRecord)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, "%p\n", "bind failed"), -1);
		}
	}
	return 0;
}

// Backing file where the data is kept.
#define BACKING_STORE "backing2.store"

int main(int argc, char**) {
	if (argc > 1) {
		ACE_MMAP_Memory_Pool_Options options(ACE_DEFAULT_BASE_ADDR, ACE_MMAP_Memory_Pool_Options::ALWAYS_FIXED);

		ACE_NEW_RETURN(g_allocator, ALLOCATOR(BACKING_STORE, BACKING_STORE, &options), -1);
		ACE_DEBUG((LM_DEBUG, "Mapped to base address %@\n", g_allocator->base_addr()));

		showRecords();
	} else {
		ACE_MMAP_Memory_Pool_Options options(0, ACE_MMAP_Memory_Pool_Options::NEVER_FIXED);

		ACE_NEW_RETURN(g_allocator, ALLOCATOR(BACKING_STORE, BACKING_STORE, &options), -1);
		ACE_DEBUG((LM_DEBUG, "Mapped to base address %@\n", g_allocator->base_addr()));

		addRecords();
	}
	g_allocator->sync();
	delete g_allocator;
	return EXIT_SUCCESS;
}
