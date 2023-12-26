include ../../../Makefile.mk

EXE_NAME:=main.$(SUFFIX_BIN)
EXE_SOURCES:=$(shell find . -type f -and -name "*.c")
EXE_OBJECTS:=$(addsuffix .o,$(basename $(EXE_SOURCES)))
CFLAGS:=-O2 -g3 -Wall -Werror -I../../include
LDFLAGS:=-Wall -Werror
CC:=gcc
ALL_DEPS:=Makefile

.PHONY: all
all: $(EXE_NAME)

$(EXE_NAME): $(EXE_OBJECTS) $(ALL_DEPS)
	$(info doing [$@])
	$(Q)$(CC) -o $(EXE_NAME) $(EXE_OBJECTS)

%.o: %.c
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean: $(ALL_DEPS)
	$(info doing [$@])
	$(Q)-rm -f *.o $(EXE_NAME)

.PHONY: show
show: $(EXE_OBJECTS) $(ALL_DEPS)
	$(info doing [$@])
	$(Q)echo main.o
	$(Q)nm main.o | grep mysym
	$(Q)echo file_with_weak_symbol.o
	$(Q)nm file_with_weak_symbol.o | grep mysym
