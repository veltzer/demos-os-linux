# first lets include the common makefile
include Makefile.mk
##############
# parameters #
##############
# directories
US_DIRS:=src/examples src/exercises src/tests
KERNEL_DIR:=src/kernel
KERNEL_SA_DIR:=src/kernel_standalone
US_INCLUDE:=src/include
# kernel variables
# version of kernel to build against
KVER:=$(shell uname -r)
# folder of the build folder for the kernel you build against
KDIR:=/lib/modules/$(KVER)/build
# fill in the vervosity level you want for the kernel module compilation process
# V:=1 will give you the command lines used...
V:=0
# warnings for kernel code (0,1,2,3,...). Better use 1 since kernel
# headers don't pass more than 1
W:=1
# extra flags to pass to the kernel module creation process...
# regular kernels do not have -Werror and we want it!
# The problem is that this makes the kernel build system scream at me (it fears I am changing
# the flags in some profound ways). This is what we have wrapper scripts for...
KCFLAGS:=-Werror
# optimization with debug info (for disassembly)
DEBUG:=0
OPT:=1
# do you want to show the commands executed ?
DO_MKDBG:=0
# the c++ compiler to be used
CXX:=g++
CC:=gcc
# do you want ccache support?
CCACHE:=1
# suffix for binary files
SUFFIX_BIN:=elf
# suffix for c++ object files
SUFFIX_OO:=oo
# checkpatch executable...
#SCRIPT_CHECKPATCH:=$(KDIR)/scripts/checkpatch.pl
SCRIPT_CHECKPATCH:=scripts/checkpatch.pl
# do tools?
DO_TOOLS:=1

# export all variables to sub-make processes...
# this could cause command line too long problems because all the make variables
# would be exported and they are pretty long (for instance the source files list...).
#export

########
# code #
########

# compilation flags
CXXFLAGS:=
CFLAGS:=
ifeq ($(DEBUG),1)
CXXFLAGS:=$(CXXFLAGS) -g3
CFLAGS:=$(CFLAGS) -g3
else
LDFLAGS:=$(LDFLAGS) -s
endif
ifeq ($(OPT),1)
CXXFLAGS:=$(CXXFLAGS) -O2
CFLAGS:=$(CFLAGS) -O2
endif
#WARN_FLAGS:=-Wall -Werror
#WARN_FLAGS:=-Wall -Werror -pedantic
WARN_FLAGS:=-Wall -Wextra -Werror -Wno-unused-parameter -Wno-clobbered -Wno-missing-field-initializers
CXXFLAGS:=$(CXXFLAGS) $(WARN_FLAGS) -I$(US_INCLUDE)
CFLAGS:=$(CFLAGS) $(WARN_FLAGS) -I$(US_INCLUDE)

# silent stuff
ifeq ($(DO_MKDBG),1)
Q:=
# we are not silent in this branch
else # DO_MKDBG
Q:=@
#.SILENT:
endif # DO_MKDBG

# sources from the git perspective
GIT_SOURCES:=$(shell git ls-files)
ALL_DEP:=$(TEMPLAR_ALL_DEP)
ALL:=$(TEMPLAR_ALL)
CLEAN:=
CLEAN_DIRS:=

ifeq ($(DO_TOOLS),1)
ALL_DEP+=out/tools.stamp
endif # DO_TOOLS

# user space applications (c and c++)
S_SRC:=$(shell find $(US_DIRS) $(KERNEL_DIR) -name "*.S")
CC_SRC:=$(shell find $(US_DIRS) $(KERNEL_DIR) -name "*.cc")
C_SRC:=$(shell find $(US_DIRS) $(KERNEL_DIR) -name "*.c" -and -not -name "mod_*.c")
ALL_C:=$(shell find . -name "*.c")
ALL_CC:=$(shell find . -name "*.cc")
ALL_H:=$(shell find . -name "*.h")
ALL_HH:=$(shell find . -name "*.hh")
ALL_US_C:=$(shell find $(US_DIRS) -name "*.c" -or -name "*.h") $(shell find src/include -name "*.h")
ALL_US_CC:=$(ALL_CC) $(ALL_HH)
ALL_US:=$(ALL_US_C) $(ALL_US_CC)
CC_ASX:=$(addsuffix .s,$(basename $(CC_SRC)))
C_ASX:=$(addsuffix .s,$(basename $(C_SRC)))
CC_PRE:=$(addsuffix .p,$(basename $(CC_SRC)))
C_PRE:=$(addsuffix .p,$(basename $(C_SRC)))
CC_DIS:=$(addsuffix .dis,$(basename $(CC_SRC)))
C_DIS:=$(addsuffix .dis,$(basename $(C_SRC)))
S_OBJ:=$(addsuffix .$(SUFFIX_O),$(basename $(S_SRC)))
CC_OBJ:=$(addsuffix .$(SUFFIX_OO),$(basename $(CC_SRC)))
C_OBJ:=$(addsuffix .o,$(basename $(C_SRC)))
S_EXE:=$(addsuffix .$(SUFFIX_BIN),$(basename $(S_SRC)))
CC_EXE:=$(addsuffix .$(SUFFIX_BIN),$(basename $(CC_SRC)))
C_EXE:=$(addsuffix .$(SUFFIX_BIN),$(basename $(C_SRC)))
ALL:=$(ALL) $(S_EXE) $(CC_EXE) $(C_EXE)
CLEAN:=$(CLEAN) $(CC_EXE) $(C_EXE) $(CC_OBJ) $(C_OBJ) $(CC_DIS) $(C_DIS) $(CC_ASX) $(C_ASX) $(CC_PRE) $(C_PRE)

# kernel modules
#MOD_SRC:=$(shell find $(KERNEL_DIR) -name "mod_*.c" -and -not -name "mod_*.mod.c")
MOD_SRC:=$(filter $(KERNEL_DIR)/%.c,$(GIT_SOURCES))
#MOD_SA_SRC:=$(shell find $(KERNEL_SA_DIR) -name "*.c")
MOD_SA_SRC:=$(filter $(KERNEL_SA_DIR)/%.c,$(GIT_SOURCES))
MOD_BAS:=$(basename $(MOD_SRC))
MOD_SA_BAS:=$(basename $(MOD_SA_SRC))
MOD_OBJ:=$(addsuffix .o,$(MOD_BAS))
MOD_CHP:=$(addsuffix .stamp,$(MOD_BAS) $(MOD_SA_BAS))
MOD_SR2:=$(addsuffix .mod.c,$(MOD_BAS))
MOD_OB2:=$(addsuffix .mod.o,$(MOD_BAS))
MOD_CM1:=$(addprefix $(KERNEL_DIR)/.,$(addsuffix .ko.cmd,$(notdir $(MOD_BAS))))
MOD_CM2:=$(addprefix $(KERNEL_DIR)/.,$(addsuffix .mod.o.cmd,$(notdir $(MOD_BAS))))
MOD_CM3:=$(addprefix $(KERNEL_DIR)/.,$(addsuffix .o.cmd,$(notdir $(MOD_BAS))))
MOD_MOD:=$(addsuffix .ko,$(MOD_BAS))
MOD_STP:=$(addsuffix .ko.stamp,$(MOD_BAS))
ALL:=$(ALL) $(MOD_STP) $(MOD_CHP)
CLEAN:=$(CLEAN) $(MOD_STP) $(MOD_MOD) $(MOD_SR2) $(MOD_OB2) $(KERNEL_DIR)/Module.symvers $(KERNEL_DIR)/modules.order $(MOD_CM1) $(MOD_CM2) $(MOD_CM3) $(MOD_OBJ)
CLEAN_DIRS:=$(CLEAN_DIRS) $(KERNEL_DIR)/.tmp_versions

# standlone
MK_SRC:=$(shell find src/examples_standalone src/kernel_standalone -name "Makefile")
MK_FLD:=$(dir $(MK_SRC))
MK_STP:=$(addsuffix .stamp,$(MK_SRC))
ALL:=$(ALL) $(MK_STP)

#########
# rules #
#########

# generic section
# do not touch this recipe
all: $(ALL)

out/tools.stamp: package.json config/deps.py
	$(info doing [$@])
	# $(Q)templar install_deps
	$(Q)make_helper touch-mkdir $@

.PHONY: clean_standalone
clean_standalone:
	$(info doing [$@])
	$(Q)for x in $(MK_FLD); do $(MAKE) -C "$$x" clean Q=$(Q); if [ ! $$? -eq 0 ]; then exit $$?; fi; done
	$(Q)rm -f $(MK_STP)
.PHONY: clean_soft
clean_soft: clean_standalone
	$(info doing [$@])
	$(Q)-rm -f $(CLEAN)
	$(Q)-rm -rf $(CLEAN_DIRS)

# -x: remove everything not known to git (not only ignore rules).
# -X: remove files in .gitignore but not everything unknown to git
# -d: remove directories also.
# -f: force.
# hard clean (may remove manually created files not yet added to the git index):
GIT_CLEAN_FLAGS:=-xdf
# soft clean (only removes stuff mentionaed in .gitignore files)
#GIT_CLEAN_FLAGS:=-Xdf
.PHONY: clean_hard
clean_hard:
	$(info doing [$@])
	$(Q)git clean $(GIT_CLEAN_FLAGS) > /dev/null
.PHONY: clean_hard_test
clean_hard_test:
	$(info doing [$@])
	$(Q)git clean $(GIT_CLEAN_FLAGS) --dry-run
.PHONY: build_standalone
build_standalone:
	$(Q)for x in $(MK_FLD); do $(MAKE) -C "$$x" Q=$(Q); if [ ! $$? -eq 0 ]; then exit $$?; fi; done

# the reason that tar and gzip were selected and not zip is that the build system
# for the native demos requires scripts with permissions and stuff. This may be different
# for other languages where the permission stuff is not that important (e.g. php).
.PHONY: archive_src
archive_src:
	$(info doing [$@])
	$(Q)git archive --format=tar --prefix=archive_src/ HEAD src Makefile scripts | gzip > /tmp/archive_src.tar.gz
.PHONY: archive_ace
archive_ace:
	$(info doing [$@])
	$(Q)git archive --format=tar --prefix=archive_ace/ HEAD src/user_space/ace/examples | gzip > /tmp/ace.tar.gz

.PHONY: git_maintain
git_maintain:
	$(info doing [$@])
	$(Q)git gc

# general rules...

# how to create regular executables...
$(CC_OBJ): %.$(SUFFIX_OO): %.cc $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) $(CCACHE) 0 $< $@ $(CXX) -c $(CXXFLAGS) -o $@ $<
$(C_OBJ): %.o: %.c $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) $(CCACHE) 0 $< $@ $(CC) -c $(CFLAGS) -o $@ $<
$(S_OBJ): %.o: %.S $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) $(CCACHE) 0 $< $@ $(CC) -c -o $@ $<
$(CC_EXE): %.$(SUFFIX_BIN): %.$(SUFFIX_OO) $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 1 $(addsuffix .cc,$(basename $<)) $@ $(CXX) $(CXXFLAGS) -o $@ $<
$(C_EXE): %.$(SUFFIX_BIN): %.o $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 1 $(addsuffix .c,$(basename $<)) $@ $(CC) $(CFLAGS) -o $@ $<
$(S_EXE): %.$(SUFFIX_BIN): %.o $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 1 $(addsuffix .S,$(basename $<)) $@ $(CC) -o $@ $<
$(CC_ASX): %.s: %.cc $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 0 $< $@ $(CXX) $(CXXFLAGS) -S -o $@ $<
$(C_ASX): %.s: %.cc $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 0 $< $@ $(CC) $(CFLAGS) -S -o $@ $<
$(CC_PRE): %.p: %.cc $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 0 $< $@ $(CXX) $(CXXFLAGS) -E -o $@ $<
$(C_PRE): %.p: %.c $(ALL_DEP) scripts/wrapper_compile.py
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 0 $< $@ $(CC) $(CFLAGS) -E -o $@ $<
$(CC_DIS) $(C_DIS): %.dis: %.$(SUFFIX_BIN) $(ALL_DEP)
	$(info doing [$@])
	$(Q)objdump --disassemble --source --demangle $< > $@
#	$(Q)objdump --demangle --disassemble --no-show-raw-insn --section=.text $< > $@
#	$(Q)objdump --demangle --source --disassemble --no-show-raw-insn --section=.text $< > $@

# rule about how to check kernel source files
$(MOD_CHP): %.stamp: %.c $(ALL_DEP)
	$(info doing [$@])
	$(Q)make_helper wrapper-silent $(SCRIPT_CHECKPATCH) --file $<
	$(Q)touch $@
# rule about how to create .ko files...
$(MOD_STP): %.ko.stamp: %.c $(ALL_DEP) scripts/wrapper_make.pl
	$(info doing [$@])
	$(Q)+scripts/wrapper_make.pl -C $(KDIR) V=$(V) W=$(W) M=$(abspath $(dir $<)) modules obj-m=$(addsuffix .o,$(notdir $(basename $<)))
	$(Q)touch $@

# rules about makefiles
$(MK_STP): %.stamp: % $(ALL_DEP)
	$(info doing [$@])
	$(Q)$(MAKE) -C $(dir $<) Q=$(Q)
	$(Q)touch $@

.PHONY: debug_me
debug_me:
	$(info MOD_MOD is $(MOD_MOD))
	$(info CC_SRC is $(CC_SRC))
	$(info CC_DIS is $(CC_DIS))
	$(info CC_EXE is $(CC_EXE))
	$(info C_SRC is $(C_SRC))
	$(info C_DIS is $(C_DIS))
	$(info C_EXE is $(C_EXE))
	$(info S_OBJ is $(S_OBJ))
	$(info S_EXE is $(S_EXE))
	$(info MOD_SRC is $(MOD_SRC))
	$(info MOD_SA_SRC is $(MOD_SA_SRC))
	$(info ALL is $(ALL))
	$(info KDIR is $(KDIR))
	$(info V is $(V))
	$(info KCFLAGS is $(KCFLAGS))
	$(info CLEAN is $(CLEAN))
	$(info CLEAN_DIRS is $(CLEAN_DIRS))
	$(info GIT_SOURCES is $(GIT_SOURCES))
	$(info ALL_DEP is $(ALL_DEP))
	$(info CXX is $(CXX))
	$(info CXXFLAGS is $(CXXFLAGS))
	$(info CC is $(CC))
	$(info CFLAGS is $(CFLAGS))
	$(info MK_SRC is $(MK_SRC))
	$(info MK_FLD is $(MK_FLD))
	$(info MK_STP is $(MK_STP))
	$(info ALL_H is $(ALL_H))
	$(info ALL_HH is $(ALL_HH))
	$(info ALL_US_C is $(ALL_US_C))
	$(info ALL_US_CC is $(ALL_US_CC))
	$(info ALL_US is $(ALL_US))

.PHONY: todo
todo:
	-@grep TODO $(CC_SRC) $(C_SRC)

# various checks

.PHONY: check_ws
check_ws:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -l "\ \ " -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)make_helper wrapper-ok git grep -l " $$" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)make_helper wrapper-ok git grep -l "\s$$" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)make_helper wrapper-ok git grep -l "$$$$" -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_main
check_main:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -e " main(" --and --not -e argc -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)make_helper wrapper-ok git grep -e "ACE_TMAIN" --and --not -e argc -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_ace_include
check_ace_include:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -l "include\"ace" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)make_helper wrapper-ok git grep -l "include \"ace" -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_include
check_include:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -l "#include[^ ]" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)make_helper wrapper-ok git grep -l "#include  " -- '*.h' '*.hh' '*.c' '*.cc'
# enable this when you have the balls...
#$(Q)make_helper wrapper-ok git grep -l -e "#include" --and --not -e "\/\/ for" --and --not -e "firstinclude" -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_license
check_license:
	$(info doing [$@])
	$(Q)scripts/check_license.py
#	$(Q)make_helper wrapper-ok git grep -L "Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>" -- '*.c' '*.cc' '*.h' '*.hh' '*.S'
.PHONY: check_exit
check_exit:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -l "exit(1)" -- '*.c' '*.cc' '*.h' '*.hh'
# " =" cannot be checked because of void foo(void* =0) and that is the reason for the next
.PHONY: check_pgrep
check_pgrep:
	$(info doing [$@])
	$(Q)wrapper-ok git grep -e $$"$$$$$$" --or -e "= " --or -e "[^\*] =" --or -e "^ " --or -e $$'\t ' --or -e $$" \t" --or -e "\ \ " --or -e $$"\t$$" --or -e " $$" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_firstinclude
check_firstinclude:
	$(info doing [$@])
	$(Q)git grep -L -e '^#include <firstinclude.h>$$' -- '*.c' '*.cc' '*.h' '*.hh' | grep -v kernel_standalone | grep -v mod_ | grep -v examples_standalone | grep -v firstinclude | grep -v shared.h | make_helper wrapper-ok grep -v kernel_helper.h
.PHONY: check_check
check_check:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -e 'CHECK_' --and -e '=' --and --not -e '=CHECK_' --and --not -e ')CHECK_' --and --not -e ',CHECK_' --and --not -e 'CHECK_ASSERT' --and --not -e PTHREAD_ERROR --and --not -e ', CHECK_' --and --not -e ERRORCHECK_
.PHONY: check_perror
check_perror:
	$(info doing [$@])
	$(Q)git grep 'perror' -- '*.c' '*.cc' '*.h' '*.hh' | grep -v assert_perror | grep -v perror.cc | make_helper wrapper-ok grep -v err_utils.h
#--and --not -e "assert_perror" --and --not -e "perror.cc" --and --not -e "us_helper.h" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_fixme
check_fixme:
	$(info doing [$@])
	$(Q)make_helper wrapper-noerr git grep FIXME -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_while1
check_while1:
	$(info doing [$@])
	$(Q)make_helper wrapper-noerr git grep "while\(1\)" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_usage
check_usage:
	$(info doing [$@])
	$(Q)make_helper wrapper-noerr git grep -e \\\"usage --and -e stderr -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_pthread
check_pthread:
	$(info doing [$@])
	$(Q)make_helper wrapper-noerr git grep -l 'CHECK_ZERO(pthread' -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_usage_2
check_usage_2:
	$(info doing [$@])
	$(Q)make_helper wrapper-noerr git grep -l "Usage" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_gitignore
check_gitignore:
	$(info doing [$@])
	$(Q)find . -mindepth 2 -and -name ".gitignore"
.PHONY: check_exitzero
check_exitzero:
	$(info doing [$@])
	$(Q)make_helper wrapper-noerr git grep -l 'exit\(0\)' -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_no_symlinks
check_no_symlinks:
	$(info doing [$@])
	$(Q)scripts/git_check_no_symlinks.py
.PHONY: check_check_header
check_check_header:
	$(info doing [$@])
	$(Q)git grep include -- '*.c' '*.cc' '*.h' '*.hh' | grep us_helper | make_helper wrapper-ok grep CHECK
.PHONY: check_veltzer_https
check_veltzer_https:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep "http:\/\/veltzer.net"
.PHONY: check_all
check_all: check_ws check_main check_ace_include check_include check_license check_exit check_firstinclude check_perror check_check kernel_check check_fixme check_while1 check_usage check_pthread check_usage_2 check_gitignore check_exitzero check_check_header check_veltzer_https check_for check_semisemi

.PHONY: check_semisemi
check_semisemi:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep ";;" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_for
check_for:
	$(info doing [$@])
	$(Q)git grep "for (" -- '*.h' '*.hh' '*.c' '*.cc' | make_helper wrapper-ok grep -v kernel
.PHONY: check_dots
check_dots:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -l " : " -- '*.h' '*.hh' '*.c' '*.cc'
# checks that dont pass
.PHONY: check_syn
check_syn:
	$(info doing [$@])
	$(Q)make_helper wrapper-ok git grep -l "while (" -- '*.c' '*.h' '*.cc' '*.hh'
	$(Q)make_helper wrapper-ok git grep -l "for (" -- '*.c' '*.h' '*.cc' '*.hh'
	$(Q)make_helper wrapper-ok git grep -l "if (" -- '*.c' '*.h' '*.cc' '*.hh'
	$(Q)make_helper wrapper-ok git grep -l "switch (" -- '*.c' '*.h' '*.cc' '*.hh'

.PHONY: check_files
check_files:
	-find . -mindepth 2 -type f -and -not -name "*.cc" -and -not -name "*.h" -and -not -name "*.h" -and -not -name "*.txt" -and -not -name "*.conf" -and -not -name "*.ini" -and -not -name "*.sample" -and -not -name "*.data" -and -not -name "*.doc" -and -not -name "*.bash" -and -not -name "*.c"
.PHONY: check_tests_for_drivers
check_tests_for_drivers:
	cd $(KERNEL_DIR);for x in test_*.cc; do y=`echo $$x | cut -f 2- -d _`;z=mod_`basename $$y .cc`.c; if [ ! -f $$z ]; then echo "missing $$z"; fi ; done
	cd $(KERNEL_DIR);for x in mod_*.c; do y=`echo $$x | cut -f 2- -d _`;z=test_`basename $$y .c`.cc; if [ ! -f $$z ]; then echo "missing $$z"; fi ; done

# various file finds...

PROJECTS_EXPR:=-name ".project" -or -name ".cproject" -or -wholename "./nbproject/*"
SOURCE_EXPR:=-name "*.cc" -or -name "*.hh" -or -name "*.h" -or -name "*.c" -or -name "Makefile" -or -name "*.txt" -or -name "*.sed" -or -name "*.patch" -or -name "*.mk" -or -name "*.cfg" -or -name "*.sh" -or -name "*.cfg" -or -name "*.html" -or -name "*.css" -or -name "*.js" -or -name "*.ajax" -or -name "*.php" -or -name "*.gdb" -or -name ".gitignore" -or -name "*.pl" -or $(PROJECTS_EXPR) -or -name "*.gif" -or -name "*.png" -or -name "*.xml" -or -name "*.sxw" -or -name "*.sxg" -or -wholename "*/.settings/*" -or -name "*.doc" -or -name "*.pdf" -or -name "*.jar" -or -name ".classpath" -or -name "*.sqlite" -or -name "*.py"
TARGET_EXPR:=-name "*.$(SUFFIX_BIN)" -or -name "*.d" -or -name "*.o" -or -name "*.so" -or -name "*.o.cmd" -or -name "*.ko" -or -name "*.ko.cmd" -or -wholename "*/.tmp_versions/*" -or -name "Module.symvers" -or -name "modules.order" -or -name "*.class" -or -name "*.stamp" -or -name "*.dis"
GIT_SOURCE_EXPR:=-type f $(addprefix -or -path ./,$(GIT_SOURCES))

.PHONY: find_not_source
find_not_source:
	-@find -type f -not -path "./.git/*" -and -not \( $(SOURCE_EXPR) \)
.PHONY: find_not_target
find_not_target:
	-@find -type f -not -path "./.git/*" -and -not \( $(TARGET_EXPR) \)
.PHONY: find_not_source_target
find_not_source_target:
	-@find -type f -not -path "./.git/*" -and -not \( $(SOURCE_EXPR) \) -and -not \( $(TARGET_EXPR) \)
.PHONY: find_not_git_target
find_not_git_target:
	-@find -type f -and -not \( $(GIT_SOURCE_EXPR) \) -and -not \( $(TARGET_EXPR) \)
.PHONY: find_not_git_source
find_not_git_source:
	-@find -type f -and -not \( $(GIT_SOURCE_EXPR) \)
.PHONY: find_exercises
find_exercises:
	-@find -type f -name "exercise.txt"

# kernel section

.PHONY: kernel_clean
kernel_clean:
	$(info doing [$@])
	$(Q)-rm -rf $(KERNEL_DIR)/.tmp_versions
	$(Q)-rm -f $(KERNEL_DIR)/Module.symvers $(KERNEL_DIR)/modules.order $(KERNEL_DIR)/mod_*.ko $(KERNEL_DIR)/mod_*.o $(KERNEL_DIR)/*.mod.c $(KERNEL_DIR)/.??* $(KERNEL_DIR)/*.stamp
.PHONY: kernel_check
kernel_check: $(MOD_CHP)
.PHONY: kernel_build
kernel_build: $(MOD_MOD)
.PHONY: kernel_help
kernel_help:
	$(MAKE) -C $(KDIR) help
.PHONY: kernel_tail
kernel_tail:
	@sudo tail /var/log/kern.log
.PHONY: kernel_tailf
kernel_tailf:
	@sudo tail -f /var/log/kern.log
.PHONY: kernel_syslog_tail
kernel_syslog_tail:
	@sudo tail /var/log/kern.log
.PHONY: kernel_syslog_tailf
kernel_syslog_tailf:
	@sudo tail -f /var/log/kern.log
.PHONY: kernel_dmesg
kernel_dmesg:
	@sudo dmesg
.PHONY: kernel_dmesg_clean
kernel_dmesg_clean:
	@sudo dmesg -c > /dev/null
.PHONY: kernel_halt
kernel_halt:
	@sudo halt
.PHONY: kernel_reboot
kernel_reboot:
	@sudo reboot
.PHONY: kernel_makeeasy
kernel_makeeasy:
	@sudo echo "%sudo ALL=NOPASSWD: ALL" >> /etc/sudoers

# code formatting

# This is what I use
.PHONY: format_uncrustify
format_uncrustify: $(ALL_DEP)
	$(info doing [$@])
	$(Q)uncrustify -c support/uncrustify.cfg --no-backup -l C $(ALL_US_C)
	$(Q)uncrustify -c support/uncrustify.cfg --no-backup -l CPP $(ALL_US_CC)
.PHONY: format_astyle
format_astyle: $(ALL_DEP)
	$(error disabled - use format_uncrustify instead)
	$(info doing [$@])
	$(Q)astyle --verbose --suffix=none --formatted --preserve-date --options=support/astyle.cfg $(ALL_US)
.PHONY: format_indent
format_indent: $(ALL_DEP)
	$(error disabled - use format_uncrustify instead)
	$(info doing [$@])
	$(Q)indent $(ALL_US)

# code measurements

.PHONY: sloccount
sloccount:
	$(info doing [$@])
	$(Q)sloccount .
.PHONY: count_files
count_files:
	$(info doing [$@])
	$(Q)echo number of C++ or C files not including headers: `find . -name "*.cc" -or -name "*.c" | wc -l`
	$(Q)echo number of C++ or C headers: `find . -name "*.hh" -or -name "*.h" | wc -l`
.PHONY: cloc
cloc:
	$(info doing [$@])
	$(Q)cloc .
