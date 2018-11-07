include ../../../Makefile.mk

GPP:=g++-4.8

# variables
CC:=icpc
# this is no optimization
#CFLAGS:=-O0 -I/usr/include/i386-linux-gnu -I../../include
# in the Intel compiler -O, -O2 or no flag for optimization are the same so it always optimizes
CFLAGS:=-restrict -I../../include
CFLAGS0:=-O0 $(CFLAGS) -g3
CFLAGS1:=-O2 $(CFLAGS) -g3
CFLAGS2:=-O2 -no-simd $(CFLAGS) -g3
CFLAGS3:=-O2 -fargument-noalias $(CFLAGS) -g3
CFLAGS4:=-O2 -opt-multi-version-aggressive $(CFLAGS) -g3
# none of following flags makes the intel compiler actually show a difference between
# alias and non-alias...
CFLAGS5:=-no-simd -fargument-alias -no-opt-multi-version-aggressive -no-ansi-alias $(CFLAGS)

ALL:=
ifeq ($(DO_INTEL),1)
ALL+=main0.$(SUFFIX_BIN) main1.$(SUFFIX_BIN) main2.$(SUFFIX_BIN) main3.$(SUFFIX_BIN) main4.$(SUFFIX_BIN) main5.$(SUFFIX_BIN)
endif
ALL+=main6.$(SUFFIX_BIN) main7.$(SUFFIX_BIN)

# targets
.PHONY: all
all: $(ALL)

main0.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS0) main.cc -o main0.$(SUFFIX_BIN)
main1.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS1) main.cc -o main1.$(SUFFIX_BIN)
main2.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS2) main.cc -o main2.$(SUFFIX_BIN)
main3.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS3) main.cc -o main3.$(SUFFIX_BIN)
main4.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS4) main.cc -o main4.$(SUFFIX_BIN)
main5.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(CC) $(CFLAGS5) main.cc -o main5.$(SUFFIX_BIN)
main6.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(GPP) -mtune=native -O2 -I../../include main.cc -o main6.$(SUFFIX_BIN)
main7.$(SUFFIX_BIN): main.cc
	$(info doing [$@])
	$(Q)$(GPP) -I../../include main.cc -o main7.$(SUFFIX_BIN)

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)rm -f main0.$(SUFFIX_BIN) main1.$(SUFFIX_BIN) main2.$(SUFFIX_BIN) main3.$(SUFFIX_BIN) main4.$(SUFFIX_BIN) main5.$(SUFFIX_BIN) main6.$(SUFFIX_BIN) main7.$(SUFFIX_BIN) main.dis

.PHONY: dis
dis:
	$(info doing [$@])
	$(Q)objdump main1.$(SUFFIX_BIN) --disassemble > main.dis
