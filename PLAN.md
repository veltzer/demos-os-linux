# Plan: empty out `.cppcheck-suppressions`

## Goal

Remove all 73 globally suppressed cppcheck IDs from `.cppcheck-suppressions` and make
the file empty, while keeping the whole project clean under:

    cppcheck --error-exitcode=1 --enable=warning,style,performance,portability \
        --check-level=exhaustive --suppressions-list=.cppcheck-suppressions --inline-suppr

as run by the `[processor.cppcheck]` section of `rsconstruct.toml` (cppcheck 2.19.0,
1015 `.c`/`.cc` files under `src/`, kernel dirs excluded).

Baseline without the suppressions list: **1194 findings in 455 files**.

## Principles

- This is a teaching repo. Findings that are the *point* of a demo (intentional leaks,
  null derefs, out-of-bounds accesses, alloca, branchless bit tricks) are annotated
  in place with `// cppcheck-suppress <id>` plus a short reason - not "fixed".
  The repo already uses this convention (68 existing inline suppressions).
- Everything else gets a real code fix.
- Every touched file must still compile with both
  `gcc/g++ -O2 -Wall -Werror -Wextra -pedantic -Isrc/include` and the clang
  equivalents (matching `[processor.cc_single_file.*]`), since fixes like adding
  `const`/`override` can trip compiler warnings.
- Work proceeds one suppression ID at a time: fix or annotate all findings for the ID,
  delete its line from `.cppcheck-suppressions`, re-run cppcheck over the project,
  commit.

## Steps

### [ ] Step 6 - finish

`.cppcheck-suppressions` has been completely deleted and its `--suppressions-list` flag removed
from `rsconstruct.toml`. Final verification: full cppcheck run over all 1015 files without any suppressions file exits 0; full GCC+Clang builds pass.

## Caveats

- cppcheck version skew: validated against cppcheck 2.19.0 locally; CI must use a
  compatible version or findings will differ.
- Roughly 150-250 findings are intentional demo behavior; those files gain visible
  `// cppcheck-suppress` comments.
