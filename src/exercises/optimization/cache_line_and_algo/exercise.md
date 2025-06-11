# Cache lines and optimiations

Allocate a big array of integers of size 100MB with random data.

You need to process this array two times:
1. add 1 to all elements of the array 
2. multiply each element that has an odd number before it, by 2.

Do the processing twice:
1. without regards to cache lines (do the first processing on the entire array and then the second).
2. with regards to cache lines.

Measure how long each one takes.
