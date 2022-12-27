# this is a minimal makefile for demo purposes, keep it that way...
obj-m:=hello.o
hello-objs:=main.o
V?=0
KDIR?=/lib/modules/$(shell uname -r)/build
# in an embedded system:
#KDIR:=[where the system maker gave me kernel headers+makefile or the kernel itself which has a makefile]
#ARCH:=arm
#CROSS_COMPILE:=[path+common prefix to where the toolchain is]
# You could also use W=2 and W=3 for more warnings but most kernel headers
# fail those...
W?=1
.PHONY: modules
modules:
	$(MAKE) -C $(KDIR) M=$(CURDIR) V=$(V) W=$(W) modules
.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) V=$(V) clean
	rm -f *.stamp
.PHONY: insmod
insmod:
	@sudo insmod ./hello.ko
.PHONY: rmmod
rmmod:
	@sudo rmmod hello
.PHONY: dmesg
dmesg:
	@sudo dmesg | tail -10
