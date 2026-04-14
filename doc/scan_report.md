# Source Scan Report — Outstanding Items

Result of a full read-through of all 1203 C/C++ source files under `src/`.
Most bugs found during the scan have been fixed. This file lists what was
**intentionally left alone** and what remains as lower-priority cleanup.

## Stub / TBD files

These compile but contain no real implementation. Either fill them in or
remove them from the build.

- `src/examples/design_patterns/behavioral/` — `interpreter.cc`,
  `iterator.cc`, `mediator.cc`, `memento.cc`, `observer.cc`, `strategy.cc`,
  `template_method.cc`
- `src/examples/design_patterns/contention/` — `atomics.cc`, `cow.cc`,
  `double_checked_locking.cc`, `lock_free.cc`, `rcu.cc`, `readers_writer.cc`
- `src/examples/design_patterns/creational/` — `builder.cc`, `factory.cc`
- `src/examples/design_patterns/structural/` — `facade.cc`, `flyweight.cc`
- `src/examples/design_patterns/multi_threaded/` — **all 13 files** are `// TBD`
  stubs (active_object, async_method, disruptor, futures_promises,
  half_sync_half_async, message_queue, monitor_object, proactor,
  producer_consumer, reactor, spin_lock, thread_pool, thread_safe_interface)
- `src/exercises/cpp/day_of_week_strongly_typed_primitive/solution.cc`
- `src/exercises/cpp/rule_of_three/solution.cc`
- `src/exercises/cpp/basic_oo/circle2.cc` — empty `main()`
- `src/examples/real_time/sigqueue.cc`, `threads.cc`
- `src/examples/namespaces/unshare_simple.cc` — never calls `unshare`

## Known non-working demos (documented as such)

- `src/examples/kernel_interaction/get_driver_version.cc` — dlopen approach
  documented as broken in-file.
- `src/examples/netlink/udev.cc` — socket never bound; in-file comment
  confirms it does not work.

## Intentional UB / teaching demos (leave as-is)

These deliberately demonstrate undefined behavior or compiler pitfalls; the
UB is the point of the demo.

- `src/examples/c/const.c` — casting away `const`
- `src/examples/compiler_internals/step_over_const.cc` — write through
  `const` alias
- `src/examples/compiler_internals/fool_a_compiler.cc` — out-of-bounds
  pointer arithmetic on `&argc`
- `src/examples/cpp/destructor.cc` — non-virtual base destructor with
  polymorphic `delete`
- `src/examples/cpp/offsets.cc` — hand-rolled `offsetof` via `(T*)1`
- `src/examples/debugging/assert.cc` — `assert(x==2)` deliberately aborts
- `src/examples_standalone/ld_weak_symbols/` — type-mismatched weak symbol
- `src/examples_standalone/lib_undefined_symbols/add.c` — `add` implemented
  via `sub` on purpose
- `src/examples_standalone/profile_gprof/main.cc` — always `exit(FAILURE)`
  mid-loop to exercise gprof
- `src/examples_standalone/gcc_plugin/main.c` — "Wordl" typo triggers the
  spell-check plugin under test
- `src/include/lowlevel_utils.h` — returns address of local to detect stack
  growth direction
- `src/kernel/test_use_count.cc` — infinite loop is the test harness

## Architectural cleanup (deferred; beyond scan scope)

- `src/kernel_standalone/cant_kill/` and `sleep_wakeup/` — near-identical
  modules, should be consolidated or one should be deleted.
- Hardcoded major number `190` in `mynull`, `chrdev`, `cant_kill`,
  `sleep_wakeup` — these conflict if loaded together. Consider dynamic
  major allocation.
- `src/examples/optimization/loop_debugging/solution.cc` and
  `loop_optimized_away/solution.cc` — identical sources.
- `src/examples_standalone/gdb_simple/main.c` and `gdb_two_versions/main.c`
  — identical null-dereference code.
- `src/examples/real_time/pi_lock_guard.cc` and `priority_inheritance_mutex.cc`
  — nearly identical (now both with the print-order fix applied).

## Systemic / style

### Memory leaks in demo code
Many short-lived demo programs never free allocated memory — acceptable
because the process exits immediately, but sets a poor example. Notable
clusters: ACE timer/signal handlers (Chapter-5 files), design pattern
objects, ACE thread arrays.

### Deprecated APIs still in use
- `pthread_yield()` in `src/examples/pthreads/core/yield.cc` — use
  `sched_yield()`.
- `auto_ptr` in `src/examples_standalone/code_formatting/file.cc` — removed
  in C++17.
- `sbrk()` in `src/examples_standalone/crash_handling/test.c`.
- `gettid()` manual definition in `src/examples_standalone/crash_handling/crash.c`
  and `naive.c` — conflicts with glibc 2.30+.
- `__memory_barrier()` in `src/examples_standalone/intel_compiler_low_level/main.c`
  — Intel compiler only.
- `asm("int $3")` in `src/examples_standalone/gdb_breakpoints_in_code/main.c`
  — x86 only.

### Security anti-patterns (demos, not production code)
- Hardcoded DB credentials in `src/examples/lib_examples/hellomysql.cc`.
- `system()` with interpolated PIDs in `src/examples/logging/syslog.cc`.
- `sprintf` without bounds in ACE `Chapter-2/Message_Blocks.cc`,
  `popen/c_run.cc`, various ACE exercise files.
- `scanf("%s", ...)` without width limits in the phonebook exercises.
- `atoi()` on unchecked user input in `malloc_debug_libs/memory_error.cc`.
- Raw-socket ICMP packet forger `src/examples/networking/raw/myping.cc`
  (oversized ICMP with spoofed source address).
- Syscall table hijack in `src/kernel/mod_syscall_hijack.c` — now requires a
  module parameter for the address, but still a rootkit-style technique.
- `mprotect` promoting data to `PROT_EXEC` in `src/examples/mmu/mprotect.cc`.

### Header hygiene
- `#pragma once` placed after `#include` in some headers (should be first).
- Missing include guards in `src/examples_standalone/default_constructor/`
  and `src/examples_standalone/g++_vtables/` headers.
- Missing `<unistd.h>` where `close()`/`read()`/`sleep()` are used in a few
  files (relies on transitive includes).

## Build-environment notes

The build system requires a few tools that may not be installed by default:

- **`mdl` (Ruby gem)** — install into the project-local `gems/` directory:
  ```
  GEM_HOME=gems gem install mdl --install-dir gems
  ```
  The Makefile calls `gems/bin/mdl` with `GEM_HOME=gems`; do not install via
  `bundle install --path gems` because that lands gems under
  `gems/ruby/<version>/` and the binary wrapper breaks.
- **`aspell`** — dictionary is at `.aspell.en.pws`. When adding new
  technical terms to markdown files, append them to that file and bump the
  count in the header line.
