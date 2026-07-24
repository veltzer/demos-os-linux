include ../../../Makefile.mk

# Build foo.c for both intel (x86-64) and ARM (aarch64).
# Requires the aarch64-linux-gnu-gcc cross compiler.

ALL:=foo.intel.elf foo.arm.elf

.PHONY: all
all: $(ALL)

foo.intel.elf: foo.c
	$(info doing [$@])
	$(Q)gcc -I../../include -Wall -Werror -O3 -g -o $@ $<

foo.arm.elf: foo.c
	$(info doing [$@])
	$(Q)aarch64-linux-gnu-gcc -I../../include -Wall -Werror -O3 -g -o $@ $<

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL) Makefile.stamp
