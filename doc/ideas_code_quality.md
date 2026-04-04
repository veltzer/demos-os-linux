# Ideas for Improving Code Quality Checking

## Replace style checks with clang-format

Set up a `.clang-format` config that enforces the project's style (tabs, brace
placement, space before parens, etc.). This would replace several checks from
`check_all.py`:

- `check_ws` (double spaces, trailing whitespace)
- `check_for` (space before paren in `for (`)
- `check_braces` (opening brace on its own line)
- `check_return` (`return(x)` style)
- `check_semisemi` (double semicolons)

clang-format can run as a checker (verify mode) or fixer. Check if rsconstruct
has a clang-format processor, otherwise use a `script` processor.

## Enable more clang-tidy checks

Currently most analyzer checks are disabled because they produce false positives
on demo code. Could gradually re-enable specific useful ones:

- `misc-redundant-semicolons` could replace `check_semisemi`
- `readability-braces-around-statements` for brace style
- `modernize-*` checks for catching outdated C++ patterns

## Trim check_all.py to project-specific checks only

Once clang-format handles style, keep only checks that no existing tool handles:

- `check_license` -- custom license header
- `check_firstinclude` -- `#include <firstinclude.h>` convention
- `check_have_solutions` -- exercises must have solutions
- `check_exit` / `check_exitzero` -- EXIT_SUCCESS/EXIT_FAILURE
- `check_perror` -- use err_utils.h macros
- `check_pthread` -- CHECK_ZERO_ERRNO for pthread
- `check_ace` / `check_ace_include` -- ACE macro conventions
- `check_no_std` -- no `std::` prefix
- `check_fixme` -- no FIXMEs
- `check_while1` -- `while(true)` not `while(1)`
- `check_check_header` -- us_helper vs CHECK macro conflict
- `check_usage` / `check_usage_2` -- usage message conventions

## Enable checkpatch

`checkpatch.pl` lives at `/lib/modules/$(uname -r)/build/scripts/checkpatch.pl`
and is not in PATH. Options:

- Create a wrapper script or symlink
- Add the kernel scripts dir to PATH in the rsconstruct environment

## Re-enable cpplint with fewer filters

Currently filtering out most categories in `.cpplint`. Could gradually reduce
filters as code is cleaned up to enforce more of Google's C++ style.

## Add include-what-you-use (iwyu)

Would replace `check_include` from `check_all.py` and also catch
missing/unnecessary includes. More thorough than the simple grep-based check.

## Consider a pre-commit hook

Run the fast checks (clang-format, `check_all.py`) on staged files before each
commit. Keeps the codebase clean incrementally rather than catching everything
at build time.
