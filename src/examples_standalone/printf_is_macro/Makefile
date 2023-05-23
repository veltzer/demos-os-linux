include ../../../Makefile.mk

hello.$(SUFFIX_BIN): hello.c
	$(info doing [$@])
	$(Q)gcc -I../../include -Wall -Werror -O3 -s -o $@ $<

.PHONY: analyze
analyze:
	$(info doing [$@])
	$(Q)nm --dynamic --undefined-only --without-symbol-versions hello.$(SUFFIX_BIN) | grep puts

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f hello.$(SUFFIX_BIN) Makefile.stamp
