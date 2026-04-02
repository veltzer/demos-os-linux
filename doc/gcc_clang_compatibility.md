# GCC/Clang Compatibility Fix Patterns

This document catalogs techniques used to fix build errors when compiling with both GCC and Clang under strict warnings (`-Werror -Wall -Wextra -pedantic`).

## Source-level fixes (preferred)

### 1. `__typeof__` instead of `typeof`
`typeof` is a GNU extension keyword. `__typeof__` is the double-underscore form accepted by both compilers without triggering `-Wlanguage-extension-token` in Clang.

### 2. `__asm__` instead of `asm`
Same as above. `asm` triggers `-Wlanguage-extension-token` in Clang C mode. `__asm__` is the portable spelling.

### 3. `std::move()` instead of unqualified `move()`
Clang warns (`-Wunqualified-std-cast-call`) when `move()` is called without `std::` even with `using namespace std;`. GCC doesn't. Explicit `std::move()` works on both.

### 4. `int main(void)` instead of `int main()` in C files
In C, `f()` means unspecified parameters (deprecated). Clang enforces `-Wstrict-prototypes`. `f(void)` is correct C and works on both. Applies to `main()` and all other functions in `.c` files and shared headers.

### 5. `(void)var;` to suppress "set but not used" warnings
Clang has `-Wunused-but-set-variable` which fires when a variable is written but never read. The `var++` hack doesn't help. `(void)var;` suppresses it on both compilers.

### 6. `volatile` on intentional null pointer dereferences
`*(char*)0 = 0;` -- Clang optimizes away non-volatile null dereference (`-Wnull-dereference`). `*(volatile char*)0 = 0;` forces the trap on both compilers.

### 7. `volatile` on intentionally unused computed variables
For variables like `sum` that are computed in a loop to do CPU work but never read, marking them `volatile` tells both compilers the side-effect matters.

### 8. `__attribute__((unused))` on intentionally unused variables/fields
For `const` globals, private fields, and typedefs that exist for demo purposes. Works on both compilers.

### 9. Attribute declarations BEFORE function definitions
Clang errors (`-Wignored-attributes`) when `__attribute__((format(...)))`, `__attribute__((noinline))`, etc. appear after the function body. GCC accepts it either way. Moving the declaration before the definition works on both.

### 10. `#ifndef __clang__` guards around GCC-specific pragmas
`#pragma GCC push_options`, `#pragma GCC optimize("O0")`, `#pragma GCC pop_options` are GCC-only. Clang errors with `-Wunknown-pragmas`. Wrapping in `#ifndef __clang__` skips them for Clang.

### 11. `#ifdef __clang__` guards around Clang-specific warning pragmas
GCC errors (`-Werror=pragmas`) on unknown warning names like `-Wgnu-designator`, `-Wvla-cxx-extension`, `-Wgnu-inline-cpp-without-extern`. Wrapping the pragma in `#ifdef __clang__` ensures only Clang sees it.

### 12. Variadic macros: `(...)` with `__VA_ARGS__` instead of `(fmt, ...)` with `## __VA_ARGS__`
The `## __VA_ARGS__` token pasting trick is a GNU extension (`-Wgnu-zero-variadic-macro-arguments`). Changing to `#define MACRO(...) func(__VA_ARGS__)` avoids both the extension and the "zero variadic args" warning. Works on both compilers.

### 13. `(size_t)` cast instead of null pointer subtraction in `offsetof` macro
`(char*)ptr - (char*)0` triggers `-Wnull-pointer-subtraction` in Clang. `(size_t)(&((type*)0)->field)` avoids the subtraction entirely. Works on both.

### 14. Value initialization `= {}` instead of GNU/C99 designated initializers in C++
GNU old-style `{field: value}` triggers `-Wgnu-designator`, and even C99 `.field = value` triggers `-Wc++20-designator` in C++ before C++20. Plain `= {}` followed by assignment works everywhere.

### 15. `noexcept` on replacement `operator delete`
Clang requires `operator delete` to match the implicit `noexcept` exception specification. GCC doesn't enforce it. Adding `noexcept` is correct per the standard and works on both.

### 16. Removing `inline` from replacement `operator new/delete`
Clang forbids `inline` on global replacement allocation functions (`-Winline-new-delete`). Removing `inline` works on both compilers.

## Build-level exclusions (last resort)

When source-level fixes are impossible because the code uses fundamentally GCC-only features, exclude from Clang builds via `exclude_paths` or `exclude_dirs` in `rsconstruct.toml`.

Cases that require exclusion:
- `__builtin_va_arg_pack_len` (GCC-only builtin)
- GCC-only compiler flags via `EXTRA_COMPILE_FLAGS` (`-finstrument-functions-exclude-function-list`, `-fno-diagnostics-show-caret`, `-ftrack-macro-expansion`, `-Wno-clobbered`, `-Wno-use-after-free`)
- Redefining glibc `extern inline` functions (e.g., `atoi`) -- Clang can't redefine them
- `#define wchar_t char` -- Clang treats `wchar_t` as a keyword in C++
- OpenMP (`omp.h`) -- excluded via directory-level exclusion
