To use dmalloc first use dmalloc(1) on the command line, set what debugging options
you need and only then run your software. dmalloc(1) will define the environment variables
that will instruct the dmalloc library how to operate.

efence
======
does not supply h files so has no rich API.
it does not detect unfreed memory (but this could easily be detected by other malloc debuggers or very simply accounting code at the user level).
it has high overhead (allocating 2 pages one of which is physical per memory allocation - so you better write high volume slabs if you want to use it...).
