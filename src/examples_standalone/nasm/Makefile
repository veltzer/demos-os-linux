include ../../../Makefile.mk

SYSTEM_ARCH:=$(shell arch)
#ARCH:=elf32
ARCH:=elf64
SOURCES=$(shell find . -name "*.asm")
OBJECTS=$(addsuffix .o,$(basename $(SOURCES)))
BINARIES=$(addsuffix .elf,$(basename $(SOURCES)))
ALL:=$(BINARIES)

.PHONY: all
all: $(ALL)
	$(info doing [$@])

# rules
$(BINARIES): %.elf: %.o
	$(info doing [$@])
	$(Q)ld -o $@ $<

$(OBJECTS): %.o: %.asm
	$(info doing [$@])
	$(Q)nasm -f $(ARCH) -o $@ $<

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL) hello.o

.PHONY: debug
debug:
	$(info SOURCES is $(SOURCES))
	$(info OBJECTS is $(OBJECTS))
	$(info BINARIES is $(BINARIES))
	$(info SYSTEM_ARCH is $(SYSTEM_ARCH))
	$(info ARCH is $(ARCH))
	$(info ALL is $(ALL))
