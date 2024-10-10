# Compiling files from stdin

This example shows how to use g++ to compile files from standard input instead of from
a file.

## NOTES
* you have to tell g++ the language you are compiling as g++ cannot automagically
deduce the source language from the file extension of the file it is compiling like
it normally does.
* your errors will have the prefix <stdin> assigned to them instead of the file name.
This can be confusing for developers. You can replace the errors <stdin> with the name
of the file. Or maybe there is a features of gcc that you can use in order to emit
the right file name?
