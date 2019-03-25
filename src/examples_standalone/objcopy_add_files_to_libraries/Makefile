include ../../../Makefile.mk

LIB:=/usr/lib/x86_64-linux-gnu/libmpi.so.40
ALL:=libmpi.so

.PHONY: all
all: $(ALL)
	$(info doing [$@])

libmpi.so: extra.o $(LIB)
	$(info doing [$@])
	$(Q)objcopy --add-section foobar=extra.o $(LIB) libmpi.so

extra.o: extra.cc
	$(info doing [$@])
	$(Q)g++ -O2 -c -o extra.o extra.cc -I../../include

.PHONY: obj
obj: extra.o
	$(info doing [$@])

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f extra.o libmpi.so

.PHONY: strings
strings:
	$(info doing [$@])
	$(Q)strings -a libmpi.so  | grep "this is the content"

.PHONY: ldd
ldd:
	$(info doing [$@])
	$(Q)ldd $(LIB)
