include ../../../Makefile.mk

# variables
ALL:=memory_error.regular.$(SUFFIX_BIN) memory_error.fsanitize.$(SUFFIX_BIN)
ALL_DEPS:=Makefile
CXXFLAGS:=-O2 -g3 -Wall -Werror -I../../include

# rules
.PHONY: all
all: $(ALL)
	$(info doing [$@])
memory_error.regular.$(SUFFIX_BIN): memory_error.cc $(ALL_DEPS)
	$(info doing [$@])
	$(Q)g++ $(CXXFLAGS) -o $@ $<
memory_error.fsanitize.$(SUFFIX_BIN): memory_error.cc $(ALL_DEPS)
	$(info doing [$@])
	$(Q)g++ $(CXXFLAGS) -o $@ $< -fsanitize=address
.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL)
