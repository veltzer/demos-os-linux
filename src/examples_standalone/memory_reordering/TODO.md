# Memory Reordering Examples — Improvement TODO

## main.cpp (Dekker test)

1. **Missing error checking on pthread/sem calls** — `pthread_create`, `sem_init`, `sem_wait`, `sem_post` can all fail. None are checked. At minimum `pthread_create` should be checked.

2. **No cleanup** — the threads run `for(;;)` and are never joined or cancelled. The semaphores are never destroyed with `sem_destroy`. The program exits abruptly from `main` while threads are still alive.

3. **Thread pinning to separate cores** — The reordering rate depends heavily on whether the two threads run on different physical cores. Pinning with `pthread_setaffinity_np` to different cores would make the demo more reliable and reproducible.

4. **`x`, `y`, `r1`, `r2` could share a cache line** — Four `static int` variables are likely adjacent in memory. False sharing could affect results. Aligning them to separate cache lines with `__attribute__((aligned(64)))` would make the test cleaner.

5. **Portability of `mfence`** — The `mfence` is x86-only. A `#ifdef __x86_64__` guard (or use of `__sync_synchronize()`) would make the intent clearer and fail at compile time on other architectures rather than silently assembling the wrong thing.

## flag_data.cpp

6. **Same cleanup/error-checking issues as main.cpp** (points 1-2 above).

7. **`observed` is a global but only used in one thread and main** — Could be returned via the semaphore pattern like `r1`/`r2` in main.cpp, or at least made thread-local + copied. Minor clarity issue.

8. **The header comment says "store-load barrier" but the reordering in question is store-store** — The comment on line 7 says "the CPU may reorder the two stores so ready becomes visible before data". The barrier function is named `store_barrier` but uses `mfence` which is a full fence. The naming could be more precise (e.g., `store_store_barrier` or just `full_barrier`).

## Makefile

9. **No `.gitignore` for build artifacts** — The `.elf` files will show up in `git status`. A `.gitignore` with `*.elf` would keep the directory clean.

10. **Makefile does not support `clang++`** — The other examples in this project build with both gcc and clang. A `CXX` variable defaulting to `g++` would make it easy to switch.

## README.md

11. **README does not mention flag_data.cpp** — It only describes the Dekker test. The flag+data example and its "why you will not see reordering on x86" story are missing.
