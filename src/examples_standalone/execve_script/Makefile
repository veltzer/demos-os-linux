include ../../../Makefile.mk

run_it.$(SUFFIX_BIN): run_it.c
	$(info doing [$@])
	$(Q)gcc -I../../include -Wall -Werror -O3 -s -o $@ $<

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f run_it.$(SUFFIX_BIN) Makefile.stamp
