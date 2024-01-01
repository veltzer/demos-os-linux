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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3)
#include <string.h>	// for strncpy(3), strcmp(3)
#include <err_utils.h>	// for CHECK_INT()

const int MAX_SIZE_OF_NAME=20;
const int MAX_SIZE_OF_PHONE=20;
const int MAX_ENTRIES_IN_PHONEBOOK=100;

typedef struct _phone_entry {
	char name[20];
	char phone[20];
	int used;
} phone_entry;

void phone_entry_init(phone_entry* pe) {
	pe->used=0;
}

int find_empty_cell(phone_entry* phonebook, const int size) {
	int i;
	for(i=0; i<size; i++) {
		if(phonebook[i].used==0) {
			return i;
		}
	}
	fprintf(stderr, "NO MORE ENTRIES IN PHONEBOOK, PANIC!\n");
	exit(EXIT_FAILURE);
}

int find_cell_by_name(phone_entry* phonebook, const int size, const char* name) {
	int i;
	for(i=0; i<size; i++) {
		if(strcmp(name, phonebook[i].name)==0) {
			return i;
		}
	}
	return -1;
}

void add_entry(phone_entry* phonebook, const int size) {
	int entry=find_empty_cell(phonebook, size);
	printf("give me the name for the new entry: ");
	CHECK_INT(scanf("%s", phonebook[entry].name), 1);
	printf("give me the phone for the new entry: ");
	CHECK_INT(scanf("%s", phonebook[entry].phone), 1);
	phonebook[entry].used=1;
}

void delete_entry(phone_entry* phonebook, const int size) {
	char name[256];
	printf("give me a name to delete: ");
	CHECK_INT(scanf("%s", name), 1);
	int entry=find_cell_by_name(phonebook, size, name);
	if(entry==-1) {
		fprintf(stderr, "you moron! give me a name in the book\n");
	} else {
		phonebook[entry].used=0;
	}
}

void find_entry(phone_entry* phonebook, const int size) {
	char name[256];
	printf("give me a name to find: ");
	CHECK_INT(scanf("%s", name), 1);
	int entry=find_cell_by_name(phonebook, size, name);
	if(entry==-1) {
		fprintf(stderr, "you moron! there is no one by that name\n");
	} else {
		printf("the phone for %s is %s\n", name, phonebook[entry].phone);
	}
}

void print_phonebook(phone_entry* phonebook, const int size) {
	int i;
	for(i=0; i<size; i++) {
		if(phonebook[i].used) {
			printf("name: %s, phone %s\n",
				phonebook[i].name,
				phonebook[i].phone
				);
		}
	}
}

static int over=0;

int show_menu() {
	int result;
	do {
		printf("here is your menu....\n");
		printf("1) add an entry (name and phone)\n");
		printf("2) delete an entry (by name)\n");
		printf("3) find an entry (by name)\n");
		printf("4) print the phonebook\n");
		printf("5) exit\n");
		CHECK_INT(scanf("%d", &result), 1);
	} while(result<1 || result>5);
	return result;
}

int main(int argc, char** argv, char** envp) {
	int i;
	phone_entry* phonebook;
	phonebook=malloc(sizeof(phone_entry)*MAX_ENTRIES_IN_PHONEBOOK);
	for(i=0; i<MAX_ENTRIES_IN_PHONEBOOK; i++) {
		phone_entry_init(phonebook+i);
		// phone_entry_init(&phonebook[i]);
	}
	while(!over) {
		int selection=show_menu();
		switch(selection) {
		case 1:
			add_entry(phonebook, MAX_ENTRIES_IN_PHONEBOOK);
			break;
		case 2:
			delete_entry(phonebook, MAX_ENTRIES_IN_PHONEBOOK);
			break;
		case 3:
			find_entry(phonebook, MAX_ENTRIES_IN_PHONEBOOK);
			break;
		case 4:
			print_phonebook(phonebook, MAX_ENTRIES_IN_PHONEBOOK);
			break;
		case 5:
			printf("terminating\n");
			over=1;
			break;
		}
	}
	// release the memory
	free(phonebook);
	return EXIT_SUCCESS;
}
