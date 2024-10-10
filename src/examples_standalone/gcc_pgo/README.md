# GCC pgo optimizations

This is an example of how to build a piece of software using the `-fprofile-arcs` feature.
This feature means that you profile your branch instructions and then feed back
this information into another build phase in order to plant hinted branch
instructions to the CPU.

It seems that using `-fprofile-arcs` and the `-fbranch-probabilities` as command line
arcs does not work. Instead we use -fprofile-generate and then `-fprofile-use` where
`-fprofile-generate` implies `-fprofile-arcs` and `-fprofile-use` implies
`-fbranch-probabilities` (at least that is what the documentation says).
