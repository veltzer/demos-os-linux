include ../../../Makefile.mk

CFLAGS_COMMON:=-I../../include -Wall -Werror -O2

ALL:=main.$(SUFFIX_BIN) volatile_demo.$(SUFFIX_BIN) libcallme.so

.PHONY: all
all: $(ALL)

callme.o: callme.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -fPIC -c -o $@ $<

libcallme.so: callme.o
	$(info doing [$@])
	$(Q)gcc -shared -o $@ $<

main.$(SUFFIX_BIN): main.c libcallme.so
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -o $@ main.c -L. -lcallme -Wl,-rpath,'$$ORIGIN'

volatile_demo.$(SUFFIX_BIN): volatile_demo.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -o $@ $<

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL) callme.o Makefile.stamp
