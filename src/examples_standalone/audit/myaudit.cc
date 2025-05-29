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
#include <link.h>
#include <stdio.h>

/*
 * This is a demo of how to use the auditing API of the GNU dynamic linker
 * See "man rtld-audit" for more details
 * Most of the code here was stolen and fixed from that manual entry...
 */

unsigned int la_version(unsigned int version) {
	printf("la_version(): %d\n", version);
	return version;
}

char* la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag) {
	printf("la_objsearch(): name=%s; cookie=%p; ", name, cookie);
	printf("flag=%s\n",
		(flag==LA_SER_ORIG) ? "LA_SER_ORIG" :
		(flag==LA_SER_LIBPATH) ? "LA_SER_LIBPATH" :
		(flag==LA_SER_RUNPATH) ? "LA_SER_RUNPATH" :
		(flag==LA_SER_DEFAULT) ? "LA_SER_DEFAULT" :
		(flag==LA_SER_CONFIG) ? "LA_SER_CONFIG" :
		(flag==LA_SER_SECURE) ? "LA_SER_SECURE" :
		"???");
	return (char*)name;
}

void la_activity (uintptr_t *cookie, unsigned int flag) {
	printf("la_activity(): cookie=%p; flag=%s\n", cookie,
		(flag==LA_ACT_CONSISTENT) ? "LA_ACT_CONSISTENT" :
		(flag==LA_ACT_ADD) ? "LA_ACT_ADD" :
		(flag==LA_ACT_DELETE) ? "LA_ACT_DELETE" :
		"???");
}

unsigned int la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) {
	printf("la_objopen(): loading \"%s\"; lmid=%s; cookie=%p\n",
		map->l_name,
		(lmid==LM_ID_BASE) ? "LM_ID_BASE" :
		(lmid==LM_ID_NEWLM) ? "LM_ID_NEWLM" :
		"???",
		cookie);
	return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}

unsigned int la_objclose (uintptr_t *cookie) {
	printf("la_objclose(): %p\n", cookie);
	return 0;
}

void la_preinit(uintptr_t *cookie) {
	printf("la_preinit(): %p\n", cookie);
}

#if __i386__
uintptr_t la_symbind32(Elf32_Sym *sym, unsigned int ndx, uintptr_t *refcook, uintptr_t *defcook, unsigned int *flags, const char *symname) {
	printf("la_symbind32(): symname=%s; sym->st_value=%x\n",
		symname, sym->st_value);
	printf("ndx=%d; flags=0x%x", ndx, *flags);
	printf("; refcook=%p; defcook=%p\n", refcook, defcook);
	return sym->st_value;
}
Elf32_Addr la_i86_gnu_pltenter(Elf32_Sym *sym, unsigned int ndx, uintptr_t *refcook, uintptr_t *defcook, La_i86_regs *regs, unsigned int *flags, const char *symname, long int *framesizep) {
	printf("la_i86_gnu_pltenter(): %s (%x)\n", symname, sym->st_value);
	return sym->st_value;
}
#endif // __i386__

#if __x86_64__
uintptr_t la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook, uintptr_t *defcook, unsigned int *flags, const char *symname) {
	printf("la_symbind64(): symname=%s; sym->st_value=%zx\n", symname, sym->st_value);
	printf("ndx=%d; flags=0x%x; ", ndx, *flags);
	printf("refcook=%p; defcook=%p\n", refcook, defcook);
	return sym->st_value;
}
#endif // __x86_64__
