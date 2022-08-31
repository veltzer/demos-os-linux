# do you want to show the commands executed ?
# Since we are using ?= for assignment it means that you can just
# set this from the command line and avoid changing the makefile...
DO_MKDBG?=0
# suffix for c++ source files
SUFFIX_CC:=cc
# suffix for c++ header files
SUFFIX_HH:=hh
# suffix for binary files
SUFFIX_BIN:=elf
# suffix for C and assembly object files
SUFFIX_O:=o
# suffix for c++ object files
SUFFIX_OO:=oo
# suffix for dependency files
SUFFIX_DEP:=dep
# suffix for library files
SUFFIX_LIB:=so
# compile with intel compiler?
DO_INTEL:=0

########
# code #
########
# silent stuff
ifeq ($(DO_MKDBG),1)
Q:=
# we are not silent in this branch
else # DO_MKDBG
Q:=@
#.SILENT:
endif # DO_MKDBG

##################
# patterns rules #
##################
# replace the regular %.c->%.o rule with a silent one
%.o: %.c
	$(info doing [$@])
	$(Q)gcc $(CFLAGS) -c -o $@ $<
