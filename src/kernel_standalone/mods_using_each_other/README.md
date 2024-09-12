This demo shows two kernel modules using each other.

How to see this running?
- insert mod1
- insert mod2 (order between modules is important since mod2 uses mod1)
- now do: watch lsmod and see the use count of mod1 go up and down

Things to note
- while you are in mod2's init function you are in use by a user space
	application (actually it's the insmod(1) program).
	That is why mod2's use count is 1.
- mod1's use count is 1 all the time since mod2 insmod.
	It does not go up to 1 on each API call rather it stays up all
	the time and you have to actually remove mod2 in order to rmmod
	mod1.
