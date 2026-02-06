include ../../../Makefile.mk

CXXFLAGS:=-g -rdynamic -Wall -Werror -I../../include
LDFLAGS:=-ldl
PROG:=run_func_from_dso.$(SUFFIX_BIN)

.PHONY: all
all: $(PROG)

$(PROG): run_func_from_dso.cc
	$(Q)$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	$(Q)-rm -f $(PROG)
