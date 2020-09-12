include ../../../Makefile.mk

# variables
CXX:=icc
#CXXFLAGS:=-I/usr/include/i386-linux-gnu
CXXFLAGS:=-Wall -Werror -O2 -I../../include
PROF_GEN=-prof-gen
PROF_USE=-prof-use

ifeq ($(DO_INTEL),1)
	ALL:=main_stage2.$(SUFFIX_BIN)
else
	ALL:=
endif

# targets
.PHONY: all
all: $(ALL)

main_stage1.o: main.cc
	$(info doing [$@])
	$(Q)$(CXX) $(CXXFLAGS) $(PROF_GEN) -c -o $@ $<
main_stage1.$(SUFFIX_BIN): main_stage1.o
	$(info doing [$@])
	$(Q)$(CXX) -o $@ $<
main_run.stamp: main_stage1.$(SUFFIX_BIN)
	$(info doing [$@])
	$(Q)rm -f *.dpi *.dyn *.lock
	$(Q)pymakehelper only_print_on_error main_stage1.$(SUFFIX_BIN)
	$(Q)touch main_run.stamp
main_stage2.o: main.cc main_run.stamp
	$(info doing [$@])
	$(Q)$(CXX) $(CXXFLAGS) $(PROF_USE) -c -o $@ $<
main_stage2.$(SUFFIX_BIN): main_stage2.o
	$(info doing [$@])
	$(Q)$(CXX) -o $@ $<

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f *.dyn *.lock *.dpi main_stage?.o main_stage?.$(SUFFIX_BIN) *.stamp
