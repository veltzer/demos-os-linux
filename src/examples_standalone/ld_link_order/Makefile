include ../../../Makefile.mk

ALL:=main.$(SUFFIX_BIN) main_awithb.$(SUFFIX_BIN) main_awithbrpath.$(SUFFIX_BIN) liba.so libb.so libawithb.so libawithbrpath.so main.o a.o b.o
CFLAGS:=-O2 -Wall -Werror -I../../include

.PHONY: all
all: $(ALL)
	$(info doing [$@])
a.o: a.c
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS) -c -fPIC -o $@ $<
b.o: b.c
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS) -c -fPIC -o $@ $<
main.o: main.c
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
liba.so: a.o
	$(info doing [$@])
	$(Q)$(CC) -shared -o $@ $<
libb.so: b.o
	$(info doing [$@])
	$(Q)$(CC) -shared -o $@ $<
libawithb.so: a.o libb.so
	$(info doing [$@])
	$(Q)$(CC) -shared -o $@ $< -L. -lb
libawithbrpath.so: a.o libb.so
	$(info doing [$@])
	$(Q)$(CC) -shared -o $@ $< -Wl,-rpath=$(CURDIR) -L. -lb
main.$(SUFFIX_BIN): main.o liba.so libb.so
	$(info doing [$@])
	$(Q)$(CC) -o $@ $< -L. -la -lb
# the above works but specifying the correct link order
# this does not work as the link order is wrong
# $(Q)$(CC) -o $@ $< -L. -lb -la
# this works but is not precise (you should really know your dependencies)
# $(Q)$(CC) -o $@ $< -L. -Wl,--start-group -lb -la -Wl,--end-group
main_awithb.$(SUFFIX_BIN): main.o libawithb.so libb.so
	$(info doing [$@])
	$(Q)$(CC) -o $@ $< -L. -lawithb -Wl,-rpath-link=.
# the above is the best solution since -rpath-link does not get hardcoded into the product (be it executable or library)
# this works well but is command line ugly (environment variable)
# $(Q)LD_LIBRARY_PATH=. $(CC) -o $@ $< -L. -lawithb
# this works but specifying 'libb.so' is superfluous
# $(Q)$(CC) -o $@ $< -L. -lawithb -L. -lb
# this does not work as it cannot find 'libb.so'
# $(Q)$(CC) -o $@ $< -L. -lawithb
main_awithbrpath.$(SUFFIX_BIN): main.o libawithbrpath.so libb.so
	$(info doing [$@])
	$(Q)$(CC) -o $@ $< -L. -lawithbrpath
.PHONY: clean
clean: $(ALL_DEPS)
	$(info doing [$@])
	$(Q)-rm -f $(ALL)
.PHONY: ldd
ldd: $(ALL)
	$(info doing [$@])
	echo liba.so
	$(Q)ldd liba.so
	echo libb.so
	$(Q)ldd libb.so
	echo libawithb.so
	$(Q)ldd libawithb.so
	echo libawithbrpath.so
	$(Q)ldd libawithbrpath.so
	echo main.elf
	$(Q)ldd main.elf
	echo main_awithb.elf
	$(Q)ldd main_awithb.elf
	echo main_awithbrpath.elf
	$(Q)ldd main_awithbrpath.elf
.PHONY: run_main
run_main:
	$(info doing [$@])
	$(Q)LD_LIBRARY_PATH=. ./main.$(SUFFIX_BIN)
.PHONY: run_main_awithb
run_main_awithb:
	$(info doing [$@])
	$(Q)LD_LIBRARY_PATH=. ./main_awithb.$(SUFFIX_BIN)
