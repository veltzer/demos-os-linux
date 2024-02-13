include ../../../Makefile.mk

ALL:=optimizers-g.out optimizers-s.out optimizers-fast.out optimizers-0.out optimizers-1.out optimizers-2.out optimizers-3.out optimizers-4.out optimizers-O.out warnings.out

.PHONY: all
all: $(ALL)
	$(info doing [$@])

optimizers-0.out:
	$(info doing [$@])
	$(Q)gcc -Q -O0 --help=optimizers > $@
optimizers-1.out:
	$(info doing [$@])
	$(Q)gcc -Q -O1 --help=optimizers > $@
optimizers-2.out:
	$(info doing [$@])
	$(Q)gcc -Q -O2 --help=optimizers > $@
optimizers-3.out:
	$(info doing [$@])
	$(Q)gcc -Q -O3 --help=optimizers > $@
optimizers-4.out:
	$(info doing [$@])
	$(Q)gcc -Q -O4 --help=optimizers > $@
optimizers-O.out:
	$(info doing [$@])
	$(Q)gcc -Q -O --help=optimizers > $@
optimizers-fast.out:
	$(info doing [$@])
	$(Q)gcc -Q -Ofast --help=optimizers > $@
optimizers-s.out:
	$(info doing [$@])
	$(Q)gcc -Q -Os --help=optimizers > $@
optimizers-g.out:
	$(info doing [$@])
	$(Q)gcc -Q -Og --help=optimizers > $@
warnings.out:
	$(info doing [$@])
	$(Q)gcc -Q --help=warnings > $@

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f $(ALL)

.PHONY: count
count:
	$(info doing [$@])
	$(Q)for x in *.out; do echo $$x; grep "enabled" $$x | wc -l;done

.PHONY: debug
debug:
	$(info doing [$@])
	$(info ALL is $(ALL))
