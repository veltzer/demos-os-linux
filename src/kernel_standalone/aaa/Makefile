HAVE_PWD=$(shell if test -f $(PWD)/../Makefile.template; then echo 1; else echo 0; fi)
HAVE_CURDIR=$(shell if test -f $(CURDIR)/../Makefile.template; then echo 1; else echo 0; fi)
HAVE_ORIG=$(shell if test -f $(ORIG)/../Makefile.template; then echo 1; else echo 0; fi)
ifeq ($(HAVE_ORIG),1)
include $(ORIG)/../Makefile.template
else
ifeq ($(HAVE_PWD),1)
include $(PWD)/../Makefile.template
else
ifeq ($(HAVE_CURDIR),1)
include $(CURDIR)/../Makefile.template
endif
endif
endif
