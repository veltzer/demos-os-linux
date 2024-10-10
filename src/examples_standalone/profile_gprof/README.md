This example shows how to profile using gprof.
What you should take notice of:
1. the makefile: see how to compile with gprof support.
	(-pg, allowed with -OXXX)
2. the code. If you exit with _exit then you won't get profile
	info. If you exit with exit or normally then you do
	get profile info. take heed!
