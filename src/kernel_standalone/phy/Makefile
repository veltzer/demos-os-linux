# this is a minimal makefile for demo purposes, keep it that way...
obj-m:=phy.o
phy-objs:=main.o
V?=0
KDIR?=/lib/modules/$(shell uname -r)/build
# in an embedded system:
#KDIR:=[where the system maker gave me kernel headers+makefile or the kernel itself which has a makefile]
ARCH:=x86_64
CROSS_COMPILE:=x86_64-linux-gnu-
# You could also use W=2 and W=3 for more warnings but most kernel headers
# fail those...
W?=1
.PHONY: modules
modules:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(CURDIR) V=$(V) W=$(W) modules
.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(CURDIR) V=$(V) clean
	rm -f *.stamp *.stamp.chp
.PHONY: insmod
insmod:
	@sudo insmod ./hello.ko
.PHONY: rmmod
rmmod:
	@sudo rmmod hello
.PHONY: dmesg
dmesg:
	@sudo dmesg | tail -10
