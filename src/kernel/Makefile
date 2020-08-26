# this is a dummy makefile to appease the kernel build system
# don't put anything important here.
# put everything in the one big makefile above...

KDIR?=/lib/modules/$(shell uname -r)/build

.PHONY: modules
modules:
	$(MAKE) -C $(KDIR) M=$(CURDIR) V=$(V) modules
