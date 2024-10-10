# GCC how to prevent warnings on system includes

This example shows that third party headers in project should be included
using the `-isystem [folder]` flags of the compiler. This makes the compiler
shut up about warning having to do with the third parties and enables one
to increase the warning level for ones own code.
