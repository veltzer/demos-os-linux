obj-m := cpl_kernel.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

.PHONY: all modules user clean

all: modules user

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

user: cpl_user

cpl_user: cpl_user.c
	$(CC) -Wall -Wextra -O2 -o $@ $<

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f cpl_user
