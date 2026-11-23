include ../../../Makefile.mk

CFLAGS:=-Wall -Werror -I../../include
SHARED_FLAGS:=-shared -fPIC

.PHONY: all
all: main.$(SUFFIX_BIN)
	$(info doing [$@])

main.$(SUFFIX_BIN): main.c libfoo.so libbar.so
	$(info doing [$@])
	$(Q)gcc $(CFLAGS) -o $@ main.c -L. -lbar -lfoo
libfoo.so: foo.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS) $(SHARED_FLAGS) -o libfoo.so foo.c -Xlinker -init -Xlinker foo_lib_init -Xlinker -fini -Xlinker foo_lib_fini -Xlinker --build-id=0xaabbcc
libbar.so: bar.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS) $(SHARED_FLAGS) -o libbar.so bar.c -Xlinker -init -Xlinker bar_lib_init -Xlinker -fini -Xlinker bar_lib_fini -Xlinker --build-id=0xaabbcc

.PHONY: run
run: main.$(SUFFIX_BIN)
	$(info doing [$@])
	$(Q)LD_LIBRARY_PATH=. ./main.$(SUFFIX_BIN)

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f main.$(SUFFIX_BIN) libfoo.so libbar.so
.PHONY: dump
dump:
	$(info doing [$@])
	$(Q)objdump -t libfoo.so
.PHONY: dump_private
dump_private:
	$(info doing [$@])
	$(Q)objdump -p libfoo.so
.PHONY: dump_all
dump_all:
	$(info doing [$@])
	$(Q)objdump -s libfoo.so
