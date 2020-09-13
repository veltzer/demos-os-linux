include ../../../Makefile.mk

ACXX := ag++
SOURCES := $(wildcard *.$(SUFFIX_CC))
PROG = hello.$(SUFFIX_BIN)
OBJECTS := $(addsuffix .$(SUFFIX_OO),$(basename $(SOURCES)))
DEPS    := $(addsuffix .$(SUFFIX_DEP),$(basename $(SOURCES)))
HEADERS := $(wildcard *.$(SUFFIx_HH))
ASPECTS := $(wildcard *.ah)
CXXFLAGS := -Wall -Werror -O3

.PHONY: all
all: $(PROG)

$(PROG): $(OBJECTS)
	$(info doing [$@])
	$(Q)$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)rm -rf *.$(SUFFIX_DEP) *.$(SUFFIX_OO) $(PROG)

# rules
$(OBJECTS): %.$(SUFFIX_OO) : %.$(SUFFIX_CC)
	$(info doing [$@])
	$(Q)pymakehelper only_print_on_error $(ACXX) $(CXXFLAGS) -c -o $@ $<

$(DEPS): %.$(SUFFIX_DEP) : %.$(SUFFIX_CC)
	$(info doing [$@])
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -x c++ -MM $(INCLUDE_LIST) $< | sed -e "s/$*\.o/$(JUNK)\/& $(JUNK)\/$*.d/g" > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: debug
debug:
	$(info SOURCES is $(SOURCES))
	$(info OBJECTS is $(OBJECTS))
	$(info HEADERS is $(HEADERS))
	$(info ASPECTS is $(ASPECTS))
	$(info DEPS is $(DEPS))
	$(info PROG is $(PROG))
	$(info CXXFLAGS is $(CXXFLAGS))
