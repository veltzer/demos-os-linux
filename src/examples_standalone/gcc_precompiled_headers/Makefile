include ../../../Makefile.mk

# these flags actually turn out to be nothing on ubuntu 13.10 (mark)
ACE_FLAGS:=$(shell pkg-config --cflags ACE)
#CFLAGS:=-H -Winvalid-pch
CFLAGS:=-Winvalid-pch
SOURCES:=$(shell find . -name "*.cc")
OBJECTS:=$(shell ./objects.sh)
OBJECTS_PRECOMP:=$(shell ./objects_precomp.sh)
OBJECTS_NOPRECOMP:=$(shell ./objects_noprecomp.sh)

.PHONY: all
all: pch/myace.hh.gch $(OBJECTS)

pch/myace.hh.gch: include/myace.hh
	$(info doing [$@])
	$(Q)mkdir -p $(dir $@)
	$(Q)g++ -Iinclude $(CFLAGS) $(ACE_FLAGS) -o $@ $<

# rules
%.precompiled.o:  main.cc pch/myace.hh.gch
	$(info doing [$@])
	$(Q)g++ -Ipch $(CFLAGS) $(ACE_FLAGS) -c -o $@ $<
%.noprecomp.o: main.cc
	$(info doing [$@])
	$(Q)g++ -Iinclude $(CFLAGS) $(ACE_FLAGS) -c -o $@ $<

.PHONY: debug
debug:
	$(info ACE_FLAGS is $(ACE_FLAGS))
	$(info SOURCES is $(SOURCES))
	$(info OBJECTS is $(OBJECTS))
	$(info OBJECTS_PRECOMP is $(OBJECTS_PRECOMP))
	$(info OBJECTS_NOPRECOMP is $(OBJECTS_NOPRECOMP))

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f pch/myace.hh.gch *.o

.PHONY: time
time: clean
	time make $(OBJECTS_NOPRECOMP)
	make pch/myace.hh.gch
	time make $(OBJECTS_PRECOMP)
