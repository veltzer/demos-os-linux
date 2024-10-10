This demostration show that:
- any function in a shared object that you want called before an application starts
	to use your shared object can be labeled with __attribute__(constructor).
- the same goes for finishing (__attribute__(destructor)).
- you can attach priorities to constructors and destructors.
	0-100 - reserved for the system (the compiler, the standard libraries etc...).
	100-... - is yours. Lower numbers are more important and will be called sooner
	for constructors and later for destructors.
- no need to compile the shared object in any special way.
- the hooks will be called once.
- what happens when you fork(2) ? (the multi-processing case)
	constructors will be called once - this is logical since they are called before
	main() and so are over before you fork...
	destructors are going to be called once per child ending, unless the child
	end abruptly (not through exit(3) but rather by _exit(2), _Exit(2) or abnormal
	termination using singal).
- what happens when you clone(2) ? (the multi-threading case)
	Nothing. This example demostrates that cloning does not call either constructors
	or destructors.
- what happens when you have more than one library doing such stuff ? where does the order
of initialization derive from ?
	linking order matters. Libraries further back the link line are more important
	and will be called first on construction and later on destruction.
	Notice that if library A has a function with priority p1 and library B
	has a function with priority p2 and A appears later on the linking line
	then A's functions are going to be called first on construction and later
	on destruction even though p2<p1 (B's function are "more important").
	If you want to avoid all of these issues then have one entry point into your
	system and handle order yourself...
