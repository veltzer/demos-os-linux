# note that you cannot use "." but rather need to use "$(M)" since when you
# run this makefile it changes directory!
# We can actually use $(M) when not calling the kernel makefile since
# it will evaluate to nothing which is ok for find but may be wrong
# for other utilities.

ifeq ($(M),)
FOLDER:=.
else
FOLDER:=$(M)
endif

KVER?=$(shell uname -r)
KDIR:=/lib/modules/$(KVER)/build
NAMES:=$(basename $(notdir $(shell find $(FOLDER) -name "mod_*.c")))
KOS:=$(addsuffix .ko,$(NAMES))
obj-m:=$(addprefix full_, $(addsuffix .o,$(NAMES)))

define template
 	full_$(1)-objs:=$(1).o
endef
$(foreach name,$(NAMES),$(eval $(call template,$(name))))

V?=0
W?=1

.PHONY: modules
modules:
	$(MAKE) -C $(KDIR) M=$(CURDIR) V=$(V) W=$(W) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) V=$(V) W=$(W) clean

.PHONY: debug
debug:
	$(info NAMES is $(NAMES))
	$(info KOS is $(KOS))
	$(info obj-m is $(obj-m))
	$(info full_mod_null-objs is $(full_mod_null-objs))
