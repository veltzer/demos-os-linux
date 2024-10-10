This demo shows all the required steps needed in order to make a dynamic library:
- write a bunch of c/c++ files with headers interface files.
- compile them all, prefereably with -O2 and -Wall -Werror.
- link them with -shared -fpic (-fpic not strictly needed on Intel).
- library name should be lib[libname].so
- compile an application.
- link the application with -L[path to library] -l[libname].
- -L in previous link is not needed if the library is installed in standard paths.
- run the application with LD_LIBRARY_PATH pointing to where the library is.
- LD_LIBRARY_PATH in the previous statement is not needed if the library is installed in
	standard paths.
- have a look at my Makefile to see how to write a makefile for such a library.

How to see what you've created:
- make show_results
- make show_results_verbose

This also demostrates the fact that shared libraries carry their own depenency information with them. You can see this by linking a library when you create your library and then seeing this information in the output of ldd.
