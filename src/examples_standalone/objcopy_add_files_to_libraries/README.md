This demo is about relinking an already existing so to be exactly as before
but with added object files within.

This is done via objcopy(1).

In general this is done so:

objcopy --add-section [nameofsection]=[nameofile] [infile] [outfile]

If you want to see the strings in the extra files you need to use 'strings -a'
and not just 'strings' since the extra object file is not in a standard section.

You can use ldd(1) to see that indeed the resulting library has the same attributes
(dependencies for instance) as the original one.
