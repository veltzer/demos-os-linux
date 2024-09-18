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

    for (int j = 0; j < phnum; j++) {
        if (phdr[j].p_type == PT_DYNAMIC) {
            ElfW(Dyn) *dyn = (ElfW(Dyn) *)(addr + phdr[j].p_vaddr);
            ElfW(Addr) *got = NULL;
            size_t got_size = 0;

            // Find GOT
            for (; dyn->d_tag != DT_NULL; ++dyn) {
                if (dyn->d_tag == DT_PLTGOT) {
                    got = (ElfW(Addr) *)dyn->d_un.d_ptr;
                } else if (dyn->d_tag == DT_PLTRELSZ) {
                    got_size = dyn->d_un.d_val / sizeof(ElfW(Addr));
                }
            }

            if (got && got_size) {
                printf("GOT for %s:\n", info->dlpi_name);
                for (size_t i = 0; i < got_size; ++i) {
                    void *func_addr = (void*)got[i];
		    if(func_addr==NULL)
			    continue;
                    const char *func_name = get_symbol_name(func_addr);
		    if(func_name==NULL)
			    continue;
                    printf("  %p: %s\n", func_addr, func_name);
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
