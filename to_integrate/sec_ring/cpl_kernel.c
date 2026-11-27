// cpl_kernel.c - print the two lower bits of CS (the Current Privilege Level)
// from kernel space. Kernel code runs in ring 0, so this should print 0.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static unsigned int read_cpl(unsigned short *cs_out)
{
	unsigned short cs;

	asm volatile ("mov %%cs, %0" : "=r" (cs));
	*cs_out = cs;
	return cs & 3;
}

static int __init cpl_init(void)
{
	unsigned short cs;
	unsigned int cpl = read_cpl(&cs);

	pr_info("cpl_kernel: load: CS = 0x%04hx, CPL (CS & 3) = %u\n", cs, cpl);
	return 0;
}

static void __exit cpl_exit(void)
{
	unsigned short cs;
	unsigned int cpl = read_cpl(&cs);

	pr_info("cpl_kernel: unload: CS = 0x%04hx, CPL (CS & 3) = %u\n", cs, cpl);
}

module_init(cpl_init);
module_exit(cpl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer <mark.veltzer@gmail.com>");
MODULE_DESCRIPTION("Show the two lower bits of CS (CPL) from kernel space");
