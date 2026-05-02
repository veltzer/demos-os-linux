include ../../../Makefile.mk

CFLAGS_COMMON:=-I../../include -Wall -Werror --std=c99 -g3

ALL:=restrict_opt.$(SUFFIX_BIN) restrict_dbg.$(SUFFIX_BIN)

.PHONY: all
all: $(ALL)

res_opt.o: res.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -O3 -c -o $@ $<

res_dbg.o: res.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -O0 -c -o $@ $<

restrict_opt.$(SUFFIX_BIN): restrict.c res_opt.o
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -O3 -o $@ restrict.c res_opt.o

restrict_dbg.$(SUFFIX_BIN): restrict.c res_dbg.o
	$(info doing [$@])
	$(Q)gcc $(CFLAGS_COMMON) -O0 -o $@ restrict.c res_dbg.o

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL) *.o Makefile.stamp
