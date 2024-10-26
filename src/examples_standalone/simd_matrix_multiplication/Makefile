include ../../../Makefile.mk

simd.$(SUFFIX_BIN): simd.c
	$(info doing [$@])
	$(Q)gcc -mavx -march=native -O3 $< -o $@ -lm

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f simd.$(SUFFIX_BIN) Makefile.stamp
