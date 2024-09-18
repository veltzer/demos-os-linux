# DSO vs static linking speed

The cost of calling a function from a Dynamic Shared Object (DSO) compared to static linking is an important consideration in software design and performance optimization. Let's break this down:

Performance Difference:

Calling a function from a DSO is generally slower than calling a statically linked function. The performance difference can vary, but it's typically in the range of a few nanoseconds to tens of nanoseconds per call on modern systems.

Reasons for the Overhead:
a) Indirection: The main source of overhead comes from the indirection through the Global Offset Table (GOT). When calling a DSO function, the program first looks up the function's address in the GOT, then jumps to that address.
b) Potential cache misses: If the GOT entry isn't in the CPU cache, there's an additional delay to fetch it from memory.
c) Potential page faults: If the DSO isn't loaded into memory, the first call can incur a significant delay due to page faults.

To quantify the difference I wrote this example.

The results will vary depending on your system, but you might see output like this:

```txt
CopyTime for 1000000000 DSO function calls: 1234567890 ns
Time for 1000000000 static function calls: 1000000000 ns
Difference per call: 0.23 ns
```

This benchmark demonstrates that:

The difference per call is typically small (often less than a nanosecond on modern systems).
Over a large number of calls, this small difference can add up.
Considerations:
a) First call: The first call to a DSO function might be significantly slower due to lazy binding and potential page faults.
b) Inlining: Static functions can be inlined by the compiler, potentially making them much faster.
c) Cache effects: Repeated calls to the same function may have different performance characteristics due to caching.
Trade-offs:

While DSO calls are slightly slower, they offer benefits like:

Reduced memory usage (shared libraries are loaded once for multiple processes)
Ability to update libraries without recompiling the main program
Potential for better code organization and modularity

In most cases, the slight performance overhead of DSO calls is outweighed by these benefits, unless you're dealing with extremely performance-critical code that makes a very large number of function calls.
