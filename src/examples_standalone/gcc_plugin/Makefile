include ../../../Makefile.mk

VER:=$(shell gcc --version | head -1 | cut -f 4 -d " " | cut -c1-3)
VER:=9
DO_PLUGIN:=0
DO_MAIN:=0
ALL:=

ifeq ($(DO_MAIN),1)
ALL:=$(ALL) main.elf
endif # DO_MAIN

ifeq ($(DO_PLUGIN),1)
ALL:=$(ALL) plugin.so
endif # DO_PLUGIN

.PHONY: all
all: $(ALL)

TARGET_GCC:=gcc-$(VER)
HOST_GCC:=g++-$(VER)

GCCPLUGINS_DIR:=$(shell $(TARGET_GCC) -print-file-name=plugin)

CFLAGS:=-I$(GCCPLUGINS_DIR)/include -I../../include -O2 -Wall -Werror
CXXFLAGS:=-I$(GCCPLUGINS_DIR)/include -I../../include -O2 -Wall -Werror -fPIC -fno-rtti -DVER=\"$(VER)\"

main.elf: main.c plugin.so
	$(info doing [$@])
	$(Q)LD_LIBRARY_PATH=. gcc-$(VER) -I../../include -fplugin=plugin.so $^ -o $@

plugin.so: plugin.c
	$(info doing [$@])
	$(Q)$(HOST_GCC) -shared $(CXXFLAGS) $^ -o $@ -laspell

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f *.o *.so main.elf Makefile.stamp

.PHONY: debug
debug:
	$(info VER is $(VER))
	$(info GCCPLUGINS_DIR is $(GCCPLUGINS_DIR))
