include ../../../Makefile.mk

# variables
ALL:=heap_bug.regular.$(SUFFIX_BIN) heap_bug.asan.$(SUFFIX_BIN)
ALL_DEPS:=Makefile
CFLAGS:=-O1 -g3 -Wall -Werror -fno-omit-frame-pointer -I../../include

# rules
.PHONY: all
all: $(ALL)
	$(info doing [$@])
heap_bug.regular.$(SUFFIX_BIN): heap_bug.c $(ALL_DEPS)
	$(info doing [$@])
	$(Q)gcc $(CFLAGS) -o $@ $<
heap_bug.asan.$(SUFFIX_BIN): heap_bug.c $(ALL_DEPS)
	$(info doing [$@])
	$(Q)gcc $(CFLAGS) -o $@ $< -fsanitize=address
.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL)
