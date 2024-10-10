This example shows how to get error when you link with a library that does not satisfy your
undefined symbols.

Notes:
- when you link a library that does not link with all of it's required dependencies
you don't get any error from the linker.
- when you do it with the --no-undefined flag you do get an error.
- when you link with a library that does not satisfy it's dependencies you get an error
	even if you pass --no-undefined
- when you link with a library that does satisfy it's own dependencies but which uses some
	other lib that does not satisfy it's own dependencies then you get no error
	even if you pass --no-undefined

TODO:
- demo the use of:
	--allow-shlib-undefined
	--no-allow-shlib-undefined
