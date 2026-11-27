// cpl_user.c - print the two lower bits of CS (the Current Privilege Level)
// from user space. On x86 user code runs in ring 3, so this should print 3.
#include <stdio.h>

int main(void)
{
	unsigned short cs;

	__asm__ volatile ("mov %%cs, %0" : "=r" (cs));
	printf("CS = 0x%04hx, CPL (CS & 3) = %u\n", cs, cs & 3);
	return 0;
}
