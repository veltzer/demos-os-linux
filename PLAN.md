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

### Step 0 - enable inline suppressions (config only)

Add `--inline-suppr` to the cppcheck args in `rsconstruct.toml`. Without it the
per-line suppressions cannot replace the global ones. The Makefile cppcheck rule
already passes it.

### Step 1 - delete the 14 already-clean IDs (config only)

These have zero findings today and their lines are simply removed:

`arrayIndexOutOfBounds, clarifyCondition, duplicateCondition,
identicalInnerCondition, invalidContainer, invalidFunctionArgStr,
invalidPrintfArgType_s, negativeIndex, returnDanglingLifetime, stringLiteralWrite,
uninitStructMember, uninitvar, unknownMacro, zerodiv`

### Step 2 - Tier A: the ~30 small IDs (about 90 findings, <=6 each)

Includes `memleak, deallocuse, syntaxError, invalidscanf, nullPointer,
shiftTooManyBitsSigned, allocaCalled, useInitializationList, uninitMemberVar,
postfixOperator, invalidPrintfArgType_uint, ignoredReturnValue, duplicateExpression,
constVariableReference, constParameter, unusedLabel, unreachableCode,
redundantAssignment, nullPointerArithmeticOutOfMemory, duplInheritedMember,
constParameterReference, checkCastIntToCharAndBack, assertWithSideEffect,
useStlAlgorithm, unassignedVariable, throwInEntryPoint, sizeofwithnumericparameter,
signConversion, redundantInitialization, passedByValue, knownPointerToBool,
virtualDestructor, uselessOverride, uselessAssignmentArg, unusedVariable,
unusedLabelConfiguration, returnByReference, passedByValueCallback,
nullPointerOutOfResources, containerOutOfBounds`.

Mixed real fixes and intentional-demo annotations. The 4 `syntaxError` findings are
investigated individually (likely cppcheck parse limitations -> file-level inline
suppression with a comment).

### Step 3 - Tier B: mechanical medium IDs (about 400 findings)

- `missingOverride` (71) - add `override`
- `noExplicitConstructor` (77) - add `explicit`
- `functionStatic` (53) - make member functions static or suppress where pedagogical
- `constVariablePointer` (57), `constParameterPointer` (52),
  `constParameterCallback` (26) - add `const`
- `unusedStructMember` (32), `variableScope` (27) - per-site small edits

### Step 4 - Tier C: judgment-call IDs (about 330 findings)

- `invalidPrintfArgType_sint` (120) - fix printf format specifiers
- `nullPointerOutOfMemory` (41) - unchecked `malloc` results: check or annotate
- `knownConditionTrueFalse` (28), `dangerousTypeCast` (57), `intToPointerCast` (26)
- `noConstructor` (20), `noCopyConstructor` (9), `noOperatorEq` (9)
- `unreadVariable` (15), `shadowVariable` (13), `postfixOperator` (6) and the rest

### Step 5 - Tier D: `cstyleCast` (338)

Largest single ID. Convert C-style casts in `.cc` files to `static_cast`/
`reinterpret_cast` in a few sweeps, or annotate where the C-style cast is the demo.

### Step 6 - finish

`.cppcheck-suppressions` is an empty file (kept because `rsconstruct.toml` references
it). Final verification: full cppcheck run over all 1015 files with
`--suppressions-list` pointing at the empty file exits 0; full GCC+Clang builds pass.

## Caveats

- cppcheck version skew: validated against cppcheck 2.19.0 locally; CI must use a
  compatible version or findings will differ.
- Roughly 150-250 findings are intentional demo behavior; those files gain visible
  `// cppcheck-suppress` comments.
