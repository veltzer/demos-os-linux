# Default constructor

This example shows that regular C++ (even without templates) compiles a default constructor
into EVERY compilation unit that uses classes of that type.
As you can see the constructor of A has the ` W ` sign in the object file which means "it's OK if there are many like it...".
Notice that the object `B` which seems to need a constructor of it's own doesn't seem to get one. This is probably due to the fact that it has no parents and so no-one to call...

This
* makes the object files bigger.
* requires the `W` flag from the linker to prevent multiple declarations of the same constructor
to create link errors.

do `make show` to see this.
