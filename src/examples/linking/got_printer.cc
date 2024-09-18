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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <link.h>
#include <elf.h>

// Function to get the name of a symbol at a given address
const char* get_symbol_name(void* addr) {
	Dl_info info;
	if (dladdr(addr, &info) && info.dli_sname) {
		return info.dli_sname;
	}
	return NULL;
}

// Callback function for dl_iterate_phdr
static int callback(struct dl_phdr_info *info, size_t, void *) {
	ElfW(Addr) addr = info->dlpi_addr;
	const ElfW(Phdr) *phdr = info->dlpi_phdr;
	int phnum = info->dlpi_phnum;

	for(int j = 0; j < phnum; j++) {
		if (phdr[j].p_type == PT_DYNAMIC) {
			ElfW(Dyn) *dyn = (ElfW(Dyn) *)(addr + phdr[j].p_vaddr);
			ElfW(Addr) *got = NULL;
			size_t got_size = 0;

			// Find GOT
			for(; dyn->d_tag != DT_NULL; ++dyn) {
				if (dyn->d_tag == DT_PLTGOT) {
					got = (ElfW(Addr) *)dyn->d_un.d_ptr;
				} else if (dyn->d_tag == DT_PLTRELSZ) {
					got_size = dyn->d_un.d_val / sizeof(ElfW(Addr));
				}
			}

			if (got && got_size) {
				printf("GOT for %s:\n", info->dlpi_name);
				for(size_t i = 0; i < got_size; ++i) {
					void *func_addr = (void*)got[i];
			if(func_addr==NULL)
				continue;
					const char *func_name = get_symbol_name(func_addr);
			if(func_name==NULL)
				continue;
					printf(" %p: %s\n", func_addr, func_name);
				}
				printf("\n");
			}
		}
	}
	return 0;
}

int main() {
	dl_iterate_phdr(callback, NULL);
	return 0;
}
