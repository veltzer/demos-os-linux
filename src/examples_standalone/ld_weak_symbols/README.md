This example shows that:

- if file1.c has "int x" (uninitialized and so weak) and file2.c has "int x=56" (initialized
	and therefore strong) symbols, they will merge in the link phase.
	This means the two files will share the x symbol.

- if two files do not concur about the type of a symbol, and one of them has the symbol as
	weak (possibly both) then all hell can break loose.

- Mind you - this whole thing only applies to C. in C++ the linker is more strict.

Update 2020:
- the above is wrong since the C++ linker is not much stricter.

References:
- https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
