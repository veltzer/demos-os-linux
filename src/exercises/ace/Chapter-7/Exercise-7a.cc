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
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>
#include <ace/MMAP_Memory_Pool.h>
#include <ace/Malloc_T.h>
#include <ace/Null_Mutex.h>
#include <ace/PI_Malloc.h>
#include <ace/Read_Buffer.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

typedef ACE_Malloc_T<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex, ACE_PI_Control_Block> SHARED_ALLOC;
typedef ACE_Malloc_LIFO_Iterator_T<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex, ACE_PI_Control_Block>
MALLOC_LIFO_RECORD;
SHARED_ALLOC* shared[3];

class Record {
public:
	Record(SHARED_ALLOC* shared, char* name) {
		size_t len=ACE_OS::strlen(name)+1;
		char* buf=reinterpret_cast<char*>(shared->malloc(len));
		ACE_OS::strcpy(buf, name);
		name_=buf;
	}
	~Record() {
		for(int i=0; i<3; i++) {
			shared[i]->free(name_.addr());
		}
	}
	char* name(void) {
		return(name_);
	}

private:
	ACE_Based_Pointer_Basic<char> name_;
};

int PrintMessages(SHARED_ALLOC *shared, int index) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("The following records were found in storage %d:\n"), index + 1));
	MALLOC_LIFO_RECORD record(*shared);
	for(void *temp=0; record.next(temp)!=0; record.advance()) {
		Record *record=reinterpret_cast<Record *>(temp);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("%C\n"), record->name()));
	}
	return(0);
}

int StoreMessages(SHARED_ALLOC* shared, char* buf) {
	void* memory=shared->malloc(sizeof(Record));
	if(memory==0) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("Unable to malloc")), -1);
	}
	// Allocate and place record
	Record* newRecord=new(memory) Record(shared, buf);
	if(shared->bind(buf, newRecord)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("bind failed")), -1);
	}
	return(0);
}

int GetMessageType(char* data) {
	// Read new line from stdin
	static ACE_Read_Buffer rb(ACE_STDIN);

	// read a single line from stdin
	// Allocate a new buffer.
	char* buffer=rb.read('\n');
	if(buffer==0) {
		// return message type zero when EOF is reached
		// Return 0 as message type
		return(0);
	} else {
		int type;
		sscanf(buffer, "%d", &type);
		// Remove the type from the buffer
		ACE_OS::sprintf(data, "%s", buffer + 2);
		return(type);
	}
}

// Backing files where the data is kept.
char *StoreName[3];
#define Address1 ACE_DEFAULT_BASE_ADDR

int ACE_TMAIN(int argc, ACE_TCHAR** argv) {
	ACE_MMAP_Memory_Pool_Options* options[3];
	int index;
	StoreName[0]=(char*)"Exercise_7a-store.0";
	StoreName[1]=(char*)"Exercise_7a-store.1";
	StoreName[2]=(char*)"Exercise_7a-store.2";
	if(argc>1) {
		// Use an existing file
		ACE_MMAP_Memory_Pool_Options option0(ACE_DEFAULT_BASE_ADDR, ACE_MMAP_Memory_Pool_Options::FIRSTCALL_FIXED);
		ACE_MMAP_Memory_Pool_Options option1(ACE_DEFAULT_BASE_ADDR + 100000, ACE_MMAP_Memory_Pool_Options::FIRSTCALL_FIXED);
		ACE_MMAP_Memory_Pool_Options option2(ACE_DEFAULT_BASE_ADDR + 200000, ACE_MMAP_Memory_Pool_Options::FIRSTCALL_FIXED);
		options[0]=&option0;
		options[1]=&option1;
		options[2]=&option2;
		for(int i=0; i<3; i++) {
			ACE_NEW_RETURN(shared[i], SHARED_ALLOC(StoreName[i], StoreName[i], options[i]), -1);
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(shared%d) Mapped to base address %@\n"), i, shared[i]->base_addr()));
		}
		for(int i=0; i<3; i++) {
			PrintMessages(shared[i], i);
		}
	} else {
		ACE_MMAP_Memory_Pool_Options option0(0, ACE_MMAP_Memory_Pool_Options::NEVER_FIXED);
		for(int i=0; i < 3; i++) {
			// ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%C\n"), StoreName[i]));
			ACE_NEW_RETURN(shared[i], SHARED_ALLOC(StoreName[i], StoreName[i], &option0), -1);
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(shared%d) Mapped to base address %@\n"), i, shared[i]->base_addr()));
		}
		char buffer[100];
		int type=1;
		while(type) {
			type=GetMessageType(buffer);
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("Buffer: <%C>\n"), buffer));
			switch (type) {
			case 0:
				continue;
			case 1:
				index=0;
				break;
			case 2:
				index=0;
				break;
			case 3:
				index=1;
				break;
			case 4:
				index=1;
				break;
			default:
				index=2;
				break;
			}
			StoreMessages(shared[index], buffer);
		}
	}
	for(int i=0; i<3; i++) {
		shared[i]->sync();
		delete shared[i];
	}
	return EXIT_SUCCESS;
}
