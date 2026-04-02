include ../../../Makefile.mk

# these flags actually turn out to be nothing on ubuntu 13.10
ACE_CFLAGS:=$(shell pkg-config --cflags ACE)
# these flags actually do something
ACE_LIBS:=$(shell pkg-config --libs ACE)
CXXFLAGS:=$(ACE_CFLAGS)
LD_FLAGS:=$(ACE_LIBS)
OBJECTS_regular:=main1.regular.o main2.regular.o
OBJECTS_compressed:=main1.compressed.o main2.compressed.o
OBJECTS_reduced:=main1.reduced.o main2.reduced.o
OBJECTS_debug:=main1.debug.o main2.debug.o
OBJECTS_split:=main1.split.o main2.split.o
OBJECTS_d4:=main1.d4.o main2.d4.o
OBJECTS_reddeb:=main1.reddeb.o main2.reddeb.o
OBJECTS_reddeb2:=main1.reddeb2.o main2.reddeb2.o
ALL:=main.regular.$(SUFFIX_BIN) main.compressed.$(SUFFIX_BIN) main.reduced.$(SUFFIX_BIN) main.split.$(SUFFIX_BIN) main.d4.$(SUFFIX_BIN) main.reddeb.$(SUFFIX_BIN) main.reddeb2.$(SUFFIX_BIN)
#REGULAR_FLAG:=-g3
REGULAR_FLAG:=-ggdb3

.PHONY: all
all: $(ALL)

main.regular.$(SUFFIX_BIN): $(OBJECTS_regular)
	$(info doing [$@])
	$(Q)g++ -o $@ $(OBJECTS_regular) $(LD_FLAGS)
main.compressed.$(SUFFIX_BIN): $(OBJECTS_compressed)
	$(info doing [$@])
	$(Q)g++ -o $@ $(OBJECTS_compressed) $(LD_FLAGS)
main.reduced.$(SUFFIX_BIN): $(OBJECTS_reduced) $(OBJECTS_debug)
	$(info doing [$@])
	#$(Q)g++ -o $@ $(OBJECTS_reduced) $(OBJECTS_debug) $(LD_FLAGS)
	$(Q)g++ -o $@ $(OBJECTS_reduced) $(LD_FLAGS)
main.split.$(SUFFIX_BIN): $(OBJECTS_split)
	$(info doing [$@])
	$(Q)g++ -o $@ $(OBJECTS_split) $(LD_FLAGS)
main.d4.$(SUFFIX_BIN): $(OBJECTS_d4)
	$(info doing [$@])
	$(Q)g++ -o $@ $(OBJECTS_d4) $(LD_FLAGS)
main.reddeb.$(SUFFIX_BIN): $(OBJECTS_reddeb)
	$(info doing [$@])
	$(Q)g++ -o $@ $(OBJECTS_reddeb) $(LD_FLAGS)
main.reddeb2.$(SUFFIX_BIN): $(OBJECTS_reddeb2)
	$(info doing [$@])
	$(Q)g++ -o $@ $(OBJECTS_reddeb2) $(LD_FLAGS)


# rules
%.regular.o: %.cc
	$(info doing [$@])
	$(Q)g++ $(REGULAR_FLAG) $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<
%.compressed.o: %.cc
	$(info doing [$@])
	$(Q)g++ -gz=zlib $(REGULAR_FLAG) $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<
%.reduced.o: %.cc
	$(info doing [$@])
	$(Q)g++ $(REGULAR_FLAG) $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<
	$(Q)objcopy --only-keep-debug $@ $(basename $<).debug.o
	$(Q)objcopy --strip-debug $@ $@
%.split.o: %.cc
	$(info doing [$@])
	$(Q)g++ -gsplit-dwarf $(REGULAR_FLAG) $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<
%.d4.o: %.cc
	$(info doing [$@])
	$(Q)g++ -gdwarf-4 $(REGULAR_FLAG) $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<
%.reddeb.o: %.cc
	$(info doing [$@])
	$(Q)g++ -femit-struct-debug-reduced -gdwarf-2 $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<
%.reddeb2.o: %.cc
	$(info doing [$@])
	$(Q)g++ -femit-struct-debug-baseonly -gdwarf-2 $(CXXFLAGS) $(ACE_FLAGS) -c -o $@ $<

.PHONY: debug
debug:
	$(info doing [$@])
	$(info ALL is $(ALL))
	$(info ACE_CFLAGS is $(ACE_CFLAGS))
	$(info ACE_LIBS is $(ACE_LIBS))
	$(info OBJECTS_regular is $(OBJECTS_regular))
	$(info OBJECTS_compressed is $(OBJECTS_compressed))
	$(info OBJECTS_reduced is $(OBJECTS_reduced))
	$(info OBJECTS_debug is $(OBJECTS_debug))
	$(info OBJECTS_split is $(OBJECTS_split))
	$(info OBJECTS_d4 is $(OBJECTS_d4))
	$(info OBJECTS_reddeb is $(OBJECTS_reddeb))
	$(info OBJECTS_reddeb2 is $(OBJECTS_reddeb2))

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(OBJECTS_regular) $(OBJECTS_compressed) $(OBJECTS_reduced) $(OBJECTS_debug) $(OBJECTS_split) $(OBJECTS_d4) $(OBJECTS_reddeb) $(OBJECTS_reddeb2) $(ALL) *.dwo
