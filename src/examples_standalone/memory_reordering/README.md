# CPU Memory Reordering

This example demonstrates CPU store-load reordering using the classic
Preshing/Dekker-style test.

## The experiment

Two threads each write one variable then read the other's variable:

```txt
Thread 1:  x = 1;  r1 = y;
Thread 2:  y = 1;  r2 = x;
```

Under sequential consistency at least one thread must see the other's write,
so the outcome `r1 == 0 && r2 == 0` is impossible. Yet modern CPUs reorder
the store and the subsequent load within each thread, making it happen.

## Building and running

```bash
make run
```

This builds and runs two variants over 500 000 iterations each:

- `main_no_barriers.elf` — compiler barrier only, CPU free to reorder
- `main_with_barriers.elf` — `mfence` instruction prevents reordering

Typical output on x86-64:

```txt
=== Without barriers ===
Iterations : 500000
Reorderings: 964
Rate       : 0.1928%

=== With barriers ===
Iterations : 500000
Reorderings: 0
Rate       : 0.0000%
```

## Why it happens

x86-64 has a strong memory model (TSO — Total Store Order) but still allows
one specific reordering: a store followed by a load to a *different* address
can be reordered because the store sits in the store buffer while the load
goes to the cache directly. The `mfence` instruction drains the store buffer
before any subsequent load, closing the window.

On architectures with weaker memory models (ARM, POWER) the reordering rate
would be much higher and more barrier types would be needed.
