# first lets include the common makefile
include Makefile.mk
##############
# parameters #
##############
# do kernel modules
DO_STP:=1
# check kernel modules
DO_CHP:=1
# check using cppcheck?
DO_CPPCHECK:=1
# check using cpplint?
DO_CPPLINT:=0
# should we depend on the Makefile itself?
DO_ALLDEP:=1
# do you want to show the commands executed ?
DO_MKDBG?=0
# do you want to check python scripts?
DO_PYLINT:=1
# do you want to depend on the wrapper script?
DO_DEP_WRAPPER:=1
# do you want to check bash syntax?
DO_CHECK_SYNTAX:=1
# do you want to run mdl on md files?
DO_MD_MDL:=1
# do spell check on all?
DO_MD_ASPELL:=1

#############
# variables #
#############
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
# fill in the verbosity level you want for the kernel module compilation process
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
# the c++ compiler to be used
CXX:=g++
# CXX:=clang++
CC:=gcc
# CC:=clang
# do you want ccache support?
CCACHE:=0
# suffix for binary files
SUFFIX_BIN:=elf
# suffix for c++ object files
SUFFIX_OO:=oo
# checkpatch executable...
SCRIPT_CHECKPATCH:=$(KDIR)/scripts/checkpatch.pl
# SCRIPT_CHECKPATCH:=scripts/checkpatch.pl --fix-inplace

# export all variables to sub-make processes...
# this could cause command line too long problems because all the make variables
# would be exported and they are pretty long (for instance the source files list...).
#export

MD_SRC:=$(shell find src/exercises -type f -and -name "*.md")
MD_BAS:=$(basename $(MD_SRC))
MD_MDL:=$(addprefix out/,$(addsuffix .mdl,$(MD_BAS)))
MD_ASPELL:=$(addprefix out/,$(addsuffix .aspell,$(MD_BAS)))

ifdef GITHUB_WORKFLOW
$(info in GITHUB_WORKFLOW)
DO_STP=0
DO_CHP=0
endif # GITHUB_WORKFLOW

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
endif # DEBUG

ifeq ($(OPT),1)
CXXFLAGS:=$(CXXFLAGS) -O2
CFLAGS:=$(CFLAGS) -O2
endif # OPT

# other interesting flags: -pedantic
WARN_FLAGS:=-Wall -Werror -Wextra -pedantic -Wno-unused-parameter -Wno-missing-field-initializers -Wno-variadic-macros
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
ALL:=
CLEAN:=
CLEAN_DIRS:=

# user space applications (c and c++)
CC_SRC:=$(shell find $(US_DIRS) $(KERNEL_DIR) -name "*.cc")
C_SRC:=$(shell find $(US_DIRS) $(KERNEL_DIR) -name "*.c" -and -not -name "mod_*.c")
CC_CPPCHECK:=$(addprefix out/, $(addsuffix .stamp.cppcheck, $(CC_SRC)))
C_CPPCHECK:=$(addprefix out/, $(addsuffix .stamp.cppcheck, $(C_SRC)))
CC_CPPLINT:=$(addprefix out/, $(addsuffix .stamp.cpplint, $(CC_SRC)))
ALL_C:=$(shell find . -name "*.c")
ALL_CC:=$(shell find . -name "*.cc")
ALL_H:=$(shell find . -name "*.h")
ALL_HH:=$(shell find . -name "*.hh")
ALL_US_C:=$(shell find $(US_DIRS) -name "*.c" -or -name "*.h") $(shell find src/include -name "*.h")
ALL_US_CC:=$(ALL_CC) $(ALL_HH)
ALL_US:=$(ALL_US_C) $(ALL_US_CC)
CC_PRE:=$(addsuffix .p,$(basename $(CC_SRC)))
C_PRE:=$(addsuffix .p,$(basename $(C_SRC)))
CC_DIS:=$(addsuffix .dis,$(basename $(CC_SRC)))
C_DIS:=$(addsuffix .dis,$(basename $(C_SRC)))
CC_OBJ:=$(addsuffix .$(SUFFIX_OO),$(basename $(CC_SRC)))
C_OBJ:=$(addsuffix .o,$(basename $(C_SRC)))
CC_EXE:=$(addsuffix .$(SUFFIX_BIN),$(basename $(CC_SRC)))
C_EXE:=$(addsuffix .$(SUFFIX_BIN),$(basename $(C_SRC)))
ALL_SH:=$(shell find src -name "*.sh")
ALL_STAMP:=$(addprefix out/, $(addsuffix .stamp, $(ALL_SH)))
ALL:=$(ALL) $(CC_EXE) $(C_EXE)
CLEAN:=$(CLEAN) $(CC_EXE) $(C_EXE) $(CC_OBJ) $(C_OBJ) $(CC_DIS) $(C_DIS) $(CC_PRE) $(C_PRE)

# kernel modules
MOD_SRC:=$(shell find $(KERNEL_DIR) -name "mod_*.c" -and -not -name "mod_*.mod.c")
#MOD_SA_SRC:=$(shell find $(KERNEL_SA_DIR) -name "*.c")
MOD_SA_SRC:=$(shell find $(KERNEL_SA_DIR) -name "*.c" -and -not -name "*.mod.c" -and -not -name "user_*.c")
MOD_BAS:=$(basename $(MOD_SRC))
MOD_SA_BAS:=$(basename $(MOD_SA_SRC))
MOD_OBJ:=$(addsuffix .o,$(MOD_BAS))
MOD_CHP:=$(addsuffix .stamp.chp,$(MOD_BAS) $(MOD_SA_BAS))
MOD_SR2:=$(addsuffix .mod.c,$(MOD_BAS))
MOD_OB2:=$(addsuffix .mod.o,$(MOD_BAS))
MOD_CM1:=$(addprefix $(KERNEL_DIR)/.,$(addsuffix .ko.cmd,$(notdir $(MOD_BAS))))
MOD_CM2:=$(addprefix $(KERNEL_DIR)/.,$(addsuffix .mod.o.cmd,$(notdir $(MOD_BAS))))
MOD_CM3:=$(addprefix $(KERNEL_DIR)/.,$(addsuffix .o.cmd,$(notdir $(MOD_BAS))))
MOD_MOD:=$(addsuffix .ko,$(MOD_BAS))
MOD_STP:=$(addsuffix .ko.stamp,$(MOD_BAS))
CLEAN:=$(CLEAN) $(MOD_STP) $(MOD_MOD) $(MOD_SR2) $(MOD_OB2) $(KERNEL_DIR)/Module.symvers $(KERNEL_DIR)/modules.order $(MOD_CM1) $(MOD_CM2) $(MOD_CM3) $(MOD_OBJ)
CLEAN_DIRS:=$(CLEAN_DIRS) $(KERNEL_DIR)/.tmp_versions
ALL_PY:=$(shell find . -type f -not -path "./.venv/*" -name "*.py")

# standlone
MK_SRC:=$(shell find src/examples_standalone src/kernel_standalone -name "Makefile")
MK_FLD:=$(dir $(MK_SRC))
MK_STP:=$(addsuffix .stamp,$(MK_SRC))

ifeq ($(DO_STP),1)
ALL:=$(ALL) $(MK_STP) $(MOD_STP)
endif # DO_STP

ifeq ($(DO_CHP),1)
ALL:=$(ALL) $(MOD_CHP)
endif # DO_CHP

ifeq ($(DO_PYLINT),1)
ALL:=$(ALL) out/pylint.stamp
endif # DO_PYLINT

ifeq ($(DO_CHECK_SYNTAX),1)
ALL:=$(ALL) $(ALL_STAMP)
endif # DO_CHECK_SYNTAX

ifeq ($(DO_DEP_WRAPPER),1)
DEP_WRAPPER:=scripts/wrapper_compile.py
else
DEP_WRAPPER:=
endif # DO_DEP_WRAPPER

ifeq ($(DO_MD_MDL),1)
ALL+=$(MD_MDL)
endif # DO_MD_MDL

ifeq ($(DO_MD_ASPELL),1)
ALL+=$(MD_ASPELL)
endif # DO_MD_ASPELL

ifeq ($(DO_CPPCHECK),1)
ALL+=$(CC_CPPCHECK)
ALL+=$(C_CPPCHECK)
endif # DO_CPPCHECK

ifeq ($(DO_CPPLINT),1)
ALL+=$(CC_CPPLINT)
endif # DO_CPPLINT

#########
# rules #
#########

# generic section
# do not touch this recipe
all: $(ALL)
	@true

.PHONY: clean_standalone
clean_standalone:
	$(info doing [$@] from [$<])
	$(Q)for x in $(MK_FLD); do $(MAKE) -C "$$x" clean Q=$(Q); if [ ! $$? -eq 0 ]; then exit $$?; fi; done
	$(Q)rm -f $(MK_STP)
.PHONY: clean_soft
clean_soft: clean_standalone
	$(info doing [$@] from [$<])
	$(Q)-rm -f $(CLEAN)
	$(Q)-rm -rf $(CLEAN_DIRS)

.PHONY: clean
clean:
	$(info doing [$@] from [$<])
	$(Q)rm -f $(ALL)
.PHONY: clean_hard
clean_hard:
	$(info doing [$@] from [$<])
	$(Q)git clean -qffxd
.PHONY: build_standalone
build_standalone:
	$(Q)for x in $(MK_FLD); do $(MAKE) -C "$$x" Q=$(Q); if [ ! $$? -eq 0 ]; then exit $$?; fi; done

# the reason that tar and gzip were selected and not zip is that the build system
# for the native demos requires scripts with permissions and stuff. This may be different
# for other languages where the permission stuff is not that important (e.g. php).
.PHONY: archive_src
archive_src:
	$(info doing [$@] from [$<])
	$(Q)git archive --format=tar --prefix=archive_src/ HEAD src Makefile scripts | gzip > /tmp/archive_src.tar.gz
.PHONY: archive_ace
archive_ace:
	$(info doing [$@] from [$<])
	$(Q)git archive --format=tar --prefix=archive_ace/ HEAD src/user_space/ace/examples | gzip > /tmp/ace.tar.gz

.PHONY: git_maintain
git_maintain:
	$(info doing [$@] from [$<])
	$(Q)git gc

.PHONY: debug
debug:
	$(info MOD_MOD is $(MOD_MOD))
	$(info MOD_STP is $(MOD_STP))
	$(info MOD_CHP is $(MOD_CHP))
	$(info CC_SRC is $(CC_SRC))
	$(info CC_DIS is $(CC_DIS))
	$(info CC_EXE is $(CC_EXE))
	$(info C_SRC is $(C_SRC))
	$(info C_DIS is $(C_DIS))
	$(info C_EXE is $(C_EXE))
	$(info MOD_SRC is $(MOD_SRC))
	$(info MOD_SA_SRC is $(MOD_SA_SRC))
	$(info ALL is $(ALL))
	$(info KDIR is $(KDIR))
	$(info V is $(V))
	$(info KCFLAGS is $(KCFLAGS))
	$(info CLEAN is $(CLEAN))
	$(info CLEAN_DIRS is $(CLEAN_DIRS))
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
	$(info ALL_PY is $(ALL_PY))
	$(info ALL_SH is $(ALL_SH))
	$(info MD_SRC is $(MD_SRC))
	$(info MD_BAS is $(MD_BAS))
	$(info MD_ASPELL is $(MD_ASPELL))
	$(info MD_MDL is $(MD_MDL))

.PHONY: todo
todo:
	-@grep TODO $(CC_SRC) $(C_SRC)

# various checks

.PHONY: check_ws
check_ws:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep -l "\ \ " -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)pymakehelper no_err git grep -l " $$" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)pymakehelper no_err git grep -l "\s$$" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)pymakehelper no_err git grep -l "$$$$" -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_main
check_main:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep -e " main(" --and --not -e argc -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)pymakehelper no_err git grep -e "ACE_TMAIN" --and --not -e argc -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_ace_include
check_ace_include:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep -l "include\"ace" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)pymakehelper no_err git grep -l "include \"ace" -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_include
check_include:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep -l "#include[^ ]" -- '*.h' '*.hh' '*.c' '*.cc'
	$(Q)pymakehelper no_err git grep -l "#include  " -- '*.h' '*.hh' '*.c' '*.cc'
# enable this when you have the balls...
#$(Q)pymakehelper no_err git grep -l -e "#include" --and --not -e "\/\/ for" --and --not -e "firstinclude" -- '*.h' '*.hh' '*.c' '*.cc'
.PHONY: check_license
check_license:
	$(info doing [$@] from [$<])
	$(Q)scripts/check_license.py
#	$(Q)pymakehelper no_err git grep -L "Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>" -- '*.c' '*.cc' '*.h' '*.hh' '*.S'
.PHONY: check_exit
check_exit:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep -l "exit(1)" -- '*.c' '*.cc' '*.h' '*.hh'
# " =" cannot be checked because of void foo(void* =0) and that is the reason for the next
.PHONY: check_pgrep
check_pgrep:
	$(info doing [$@] from [$<])
	$(Q)wrapper-ok git grep -e $$"$$$$$$" --or -e "= " --or -e "[^\*] =" --or -e "^ " --or -e $$'\t ' --or -e $$" \t" --or -e "\ \ " --or -e $$"\t$$" --or -e " $$" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_firstinclude
check_firstinclude:
	$(info doing [$@] from [$<])
	$(Q)git grep -L -e '^#include <firstinclude.h>$$' -- '*.c' '*.cc' '*.h' '*.hh' | grep -v kernel_standalone | grep -v mod_ | grep -v examples_standalone | grep -v firstinclude | grep -v shared.h | pymakehelper no_err grep -v kernel_helper.h
.PHONY: check_check
check_check:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep -e 'CHECK_' --and -e '=' --and --not -e '=CHECK_' --and --not -e 'CHECK_' --and --not -e ',CHECK_' --and --not -e 'CHECK_ASSERT' --and --not -e PTHREAD_ERROR --and --not -e ', CHECK_' --and --not -e ERRORCHECK_
.PHONY: check_perror
check_perror:
	$(info doing [$@] from [$<])
	$(Q)git grep 'perror' -- '*.c' '*.cc' '*.h' '*.hh' | grep -v assert_perror | grep -v perror.cc | pymakehelper no_err grep -v err_utils.h
#--and --not -e "assert_perror" --and --not -e "perror.cc" --and --not -e "us_helper.h" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_fixme
check_fixme:
	$(info doing [$@] from [$<])
	$(Q)pymakehelper no_err git grep FIXME -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_while1
check_while1:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep "while\(1\)" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_usage
check_usage:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep -e \\\"usage --and -e stderr -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_pthread
check_pthread:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep -l 'CHECK_ZERO(pthread' -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_usage_2
check_usage_2:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep -l "Usage" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_gitignore
check_gitignore:
	$(info doing [$@])
	$(Q)find . -mindepth 2 -and -name ".gitignore"
.PHONY: check_exitzero
check_exitzero:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep -l 'exit\(0\)' -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_no_symlinks
check_no_symlinks:
	$(info doing [$@])
	$(Q)scripts/git_check_no_symlinks.py
.PHONY: check_check_header
check_check_header:
	$(info doing [$@])
	$(Q)git grep include -- '*.c' '*.cc' '*.h' '*.hh' | grep us_helper | pymakehelper no_err grep CHECK
.PHONY: check_all
check_all: check_ws check_main check_ace_include check_include check_license check_exit check_firstinclude check_perror check_check check_fixme check_while1 check_usage check_pthread check_usage_2 check_exitzero check_check_header check_for check_semisemi
	$(info doing [check_have_solutions])
	$(Q)scripts/check_have_solutions.py

.PHONY: check_semisemi
check_semisemi:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep ";;" -- '*.c' '*.cc' '*.h' '*.hh'
.PHONY: check_for
check_for:
	$(info doing [$@])
	$(Q)git grep "for (" -- '*.h' '*.hh' '*.c' '*.cc' | pymakehelper no_err grep -v kernel
.PHONY: check_dots
check_dots:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep -l " : " -- '*.h' '*.hh' '*.c' '*.cc'
# checks that dont pass
.PHONY: check_syn
check_syn:
	$(info doing [$@])
	$(Q)pymakehelper no_err git grep -l "while (" -- '*.c' '*.h' '*.cc' '*.hh'
	$(Q)pymakehelper no_err git grep -l "for (" -- '*.c' '*.h' '*.cc' '*.hh'
	$(Q)pymakehelper no_err git grep -l "if (" -- '*.c' '*.h' '*.cc' '*.hh'
	$(Q)pymakehelper no_err git grep -l "switch (" -- '*.c' '*.h' '*.cc' '*.hh'

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

.PHONY: find_not_source
find_not_source:
	-@find -type f -not -path "./.git/*" -and -not \( $(SOURCE_EXPR) \)
.PHONY: find_not_target
find_not_target:
	-@find -type f -not -path "./.git/*" -and -not \( $(TARGET_EXPR) \)
.PHONY: find_not_source_target
find_not_source_target:
	-@find -type f -not -path "./.git/*" -and -not \( $(SOURCE_EXPR) \) -and -not \( $(TARGET_EXPR) \)
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
	$(info doing [$@])
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
format_uncrustify:
	$(info doing [$@])
	$(Q)uncrustify -c support/uncrustify.cfg --no-backup -l C $(ALL_US_C)
	$(Q)uncrustify -c support/uncrustify.cfg --no-backup -l CPP $(ALL_US_CC)
.PHONY: format_astyle
format_astyle:
	$(error disabled - use format_uncrustify instead)
	$(info doing [$@])
	$(Q)astyle --verbose --suffix=none --formatted --preserve-date --options=support/astyle.cfg $(ALL_US)
.PHONY: format_indent
format_indent:
	$(error disabled - use format_uncrustify instead)
	$(info doing [$@])
	$(Q)indent $(ALL_US)

.PHONY: pylint
pylint: out/pylint.stamp
out/pylint.stamp: $(ALL_PY)
	$(Q)pylint --reports=n --score=n $(ALL_PY)
	$(Q)pymakehelper touch_mkdir $@

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

#################
# pattern rules #
#################
$(CC_OBJ): %.$(SUFFIX_OO): %.cc $(DEP_WRAPPER)
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) $(CCACHE) 0 $< $@ $(CXX) -c $(CXXFLAGS) -o $@ $<
$(C_OBJ): %.o: %.c $(DEP_WRAPPER)
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) $(CCACHE) 0 $< $@ $(CC) -c $(CFLAGS) -o $@ $<
$(CC_EXE): %.$(SUFFIX_BIN): %.$(SUFFIX_OO) $(DEP_WRAPPER)
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 1 $(addsuffix .cc,$(basename $<)) $@ $(CXX) $(CXXFLAGS) -o $@ $<
$(C_EXE): %.$(SUFFIX_BIN): %.o $(DEP_WRAPPER)
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 1 $(addsuffix .c,$(basename $<)) $@ $(CC) $(CFLAGS) -o $@ $<
$(CC_PRE): %.p: %.cc $(DEP_WRAPPER)
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 0 $< $@ $(CXX) $(CXXFLAGS) -E -o $@ $<
$(C_PRE): %.p: %.c $(DEP_WRAPPER)
	$(info doing [$@])
	$(Q)scripts/wrapper_compile.py $(DO_MKDBG) 0 0 $< $@ $(CC) $(CFLAGS) -E -o $@ $<
$(CC_DIS) $(C_DIS): %.dis: %.$(SUFFIX_BIN)
	$(info doing [$@])
	$(Q)objdump --disassemble --source --demangle $< > $@
#	$(Q)objdump --demangle --disassemble --no-show-raw-insn --section=.text $< > $@
#	$(Q)objdump --demangle --source --disassemble --no-show-raw-insn --section=.text $< > $@
$(CC_CPPCHECK): out/%.cc.stamp.cppcheck: %.cc
	$(info doing [$@])
	$(Q)cppcheck --inline-suppr --error-exitcode=1 --quiet -i $(US_INCLUDE) $<
	$(Q)pymakehelper touch_mkdir $@
$(C_CPPCHECK): out/%.c.stamp.cppcheck: %.c
	$(info doing [$@])
	$(Q)cppcheck --inline-suppr --error-exitcode=1 --quiet -i $(US_INCLUDE) $<
	$(Q)pymakehelper touch_mkdir $@
$(CC_CPPLINT): out/%.cc.stamp.cpplint: %.cc
	$(info doing [$@])
	$(Q)cpplint $<
	$(Q)pymakehelper touch_mkdir $@

# rule about how to check kernel source files
$(MOD_CHP): %.stamp.chp: %.c
	$(info doing [$@])
	$(Q)pymakehelper only_print_on_error $(SCRIPT_CHECKPATCH) --no-tree --file $<
	$(Q)pymakehelper touch_mkdir $@
# rule about how to create .ko files...
$(MOD_STP): %.ko.stamp: %.c
	$(info doing [$@])
	$(Q)sed 's/MODNAME/$(notdir $(basename $<))/g' src/kernel/Makefile.tmpl > src/kernel/Makefile
	$(Q)pymakehelper only_print_on_error make -C src/kernel V=$(V) W=$(W) modules
	$(Q)pymakehelper touch_mkdir $@
# shell checking rules
$(ALL_STAMP): out/%.stamp: % .shellcheckrc
	$(info doing [$@])
	$(Q)shellcheck --severity=error --shell=bash --external-sources --source-path="$$HOME" $<
	$(Q)pymakehelper touch_mkdir $@

# rules about makefiles
$(MK_STP): %.stamp: %
	$(info doing [$@])
	$(Q)$(MAKE) -C $(dir $<) Q=$(Q)
	$(Q)pymakehelper touch_mkdir $@
$(MD_MDL): out/%.mdl: %.md .mdlrc .mdl.style.rb
	$(info doing [$@])
	$(Q)GEM_HOME=gems gems/bin/mdl $<
	$(Q)mkdir -p $(dir $@)
	$(Q)touch $@
$(MD_ASPELL): out/%.aspell: %.md .aspell.conf .aspell.en.prepl .aspell.en.pws
	$(info doing [$@])
	$(Q)aspell --conf-dir=. --conf=.aspell.conf list < $< | pymakehelper error_on_print sort -u
	$(Q)pymakehelper touch_mkdir $@


###################
# gathering rules #
###################
.PHONY: all_cc_cppcheck
all_cc_cppcheck: $(CC_CPPCHECK)
.PHONY: all_c_cppcheck
all_c_cppcheck: $(C_CPPCHECK)
.PHONY: all_cc_cpplint
all_cc_cpplint: $(CC_CPPLINT)

############
# all deps #
############

ifeq ($(DO_ALLDEP),1)
.EXTRA_PREREQS+=$(foreach mk, ${MAKEFILE_LIST},$(abspath ${mk}))
endif # DO_ALLDEP
