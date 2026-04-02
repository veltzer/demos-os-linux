#!/usr/bin/env python

"""
Consolidated code style checks for the demos-os-linux project.
This replaces the check_all Makefile target and its sub-targets.

Each check searches tracked C/C++ source files for patterns that
violate project conventions. The script exits with code 1 if any
check finds violations, printing all violations to stderr first.
"""

import glob
import os
import re
import subprocess
import sys


SOURCE_EXTENSIONS = {".c", ".cc", ".h", ".hh"}

# Directories to exclude from firstinclude check
FIRSTINCLUDE_EXCLUDES = [
    "kernel_standalone",
    "mod_",
    "examples_standalone",
    "firstinclude",
    "shared.h",
    "kernel_helper.h",
]

# Directories for brace check
BRACE_CHECK_DIRS = [
    "src/examples",
    "src/examples_standalone",
    "src/exercises",
    "src/exercises_standalone",
    "src/include",
    "src/tests",
]


def git_tracked_sources():
    """Return list of git-tracked files with source extensions."""
    result = subprocess.run(
        ["git", "ls-files"],
        capture_output=True, text=True, check=True,
    )
    files = []
    for f in result.stdout.splitlines():
        if any(f.endswith(ext) for ext in SOURCE_EXTENSIONS):
            files.append(f)
    return files


def read_file(path):
    """Read file contents, return None on error."""
    try:
        with open(path) as f:
            return f.read()
    except (OSError, UnicodeDecodeError):
        return None


def check_ws(files):
    """No double spaces, trailing spaces, or trailing tabs in source."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "  " in line:
                errors.append(f"{f}:{i}: double space")
            if line.endswith(" ") or line.endswith("\t"):
                errors.append(f"{f}:{i}: trailing whitespace")
    return errors


def check_ace_include(files):
    """No ACE includes with wrong style."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if 'include"ace' in line or 'include "ace' in line:
                errors.append(f"{f}:{i}: bad ACE include style")
    return errors


def check_include(files):
    """#include must be followed by exactly one space."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            stripped = line.lstrip()
            if not stripped.startswith("#include"):
                continue
            after = stripped[len("#include"):]
            if after and after[0] not in (" ", "\t"):
                errors.append(f"{f}:{i}: #include not followed by space")
            if after.startswith("  "):
                errors.append(f"{f}:{i}: #include followed by double space")
    return errors


def check_license(files):
    """All source files must start with the project license."""
    license_path = "support/license_new.txt"
    if not os.path.exists(license_path):
        return [f"license file {license_path} not found"]
    with open(license_path) as f:
        license_text = f.read()
    errors = []
    for path in files:
        content = read_file(path)
        if content is None:
            continue
        if content.startswith(license_text):
            continue
        # kernel files may have SPDX first line
        lines = content.split("\n")
        if lines[0] == "// SPDX-License-Identifier: GPL-2.0":
            rest = "\n".join(lines[1:])
            if rest.startswith(license_text):
                continue
        errors.append(f"{path}: missing or wrong license header")
    return errors


def check_exit(files):
    """No exit(1) calls — use EXIT_FAILURE instead."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "exit(1)" in line:
                errors.append(f"{f}:{i}: use EXIT_FAILURE instead of exit(1)")
    return errors


def check_firstinclude(files):
    """All source files must start with #include <firstinclude.h>."""
    errors = []
    for f in files:
        if any(excl in f for excl in FIRSTINCLUDE_EXCLUDES):
            continue
        content = read_file(f)
        if content is None:
            continue
        # Find first non-comment, non-empty, non-SPDX line
        found = False
        for line in content.splitlines():
            if line.strip() == "#include <firstinclude.h>":
                found = True
                break
            # Skip comments and blank lines at the top
            stripped = line.strip()
            if stripped == "" or stripped.startswith("/*") or stripped.startswith("*") or stripped.startswith("//"):
                continue
            break
        if not found:
            errors.append(f"{f}: missing #include <firstinclude.h>")
    return errors


def check_perror(files):
    """No raw perror() calls — use err_utils.h macros."""
    errors = []
    for f in files:
        if "err_utils.h" in f or "perror.cc" in f:
            continue
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "perror" in line and "assert_perror" not in line:
                errors.append(f"{f}:{i}: use err_utils.h instead of perror")
    return errors


def check_fixme(files):
    """No FIXME comments."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "FIXME" in line:
                errors.append(f"{f}:{i}: FIXME found")
    return errors


def check_while1(files):
    """Use while(true) instead of while(1)."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "while(1)" in line:
                errors.append(f"{f}:{i}: use while(true) instead of while(1)")
    return errors


def check_usage(files):
    """No 'usage' strings printed to stderr (use a consistent pattern)."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if '"usage' in line.lower() and "stderr" in line:
                errors.append(f"{f}:{i}: usage message to stderr")
    return errors


def check_pthread(files):
    """Use CHECK_ZERO_ERRNO(pthread_...) not CHECK_ZERO(pthread_...)."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "CHECK_ZERO(pthread" in line:
                errors.append(f"{f}:{i}: use CHECK_ZERO_ERRNO for pthread functions")
    return errors


def check_usage_2(files):
    """No 'Usage' string (use lowercase or a different pattern)."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "Usage" in line:
                errors.append(f"{f}:{i}: 'Usage' found (use lowercase)")
    return errors


def check_exitzero(files):
    """No exit(0) calls — use EXIT_SUCCESS or return."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "exit(0)" in line:
                errors.append(f"{f}:{i}: use EXIT_SUCCESS instead of exit(0)")
    return errors


def check_check_header(files):
    """Files including us_helper should not use CHECK_ macros."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        if "us_helper" in content and "CHECK" in content:
            for i, line in enumerate(content.splitlines(), 1):
                if "include" in line and "us_helper" in line:
                    errors.append(f"{f}:{i}: us_helper.h included but CHECK_ macros used")
                    break
    return errors


def check_for(files):
    """Use for(...) not for (...) — no space before paren."""
    errors = []
    for f in files:
        if "kernel" in f:
            continue
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if "for (" in line:
                errors.append(f"{f}:{i}: use for( not for (")
    return errors


def check_semisemi(files):
    """No double semicolons."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if ";;" in line:
                errors.append(f"{f}:{i}: double semicolon")
    return errors


def check_return(files):
    r"""No space before return's paren: return(x) not return (x)."""
    errors = []
    pattern = re.compile(r"\s+return\(")
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if pattern.search(line):
                errors.append(f"{f}:{i}: use 'return x' not 'return(x)'")
    return errors


def check_braces(files):
    """No opening brace on its own line at column 0 in source dirs."""
    errors = []
    for f in files:
        if not any(f.startswith(d) for d in BRACE_CHECK_DIRS):
            continue
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            if line.startswith("{"):
                errors.append(f"{f}:{i}: opening brace on its own line")
    return errors


def check_ace(files):
    """No ACE_TMAIN, ACE_TCHAR, or ACE_TEXT — use standard equivalents."""
    bad_patterns = ["ACE_TMAIN", "ACE_TCHAR", "ACE_TEXT"]
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        for i, line in enumerate(content.splitlines(), 1):
            for pat in bad_patterns:
                if pat in line:
                    errors.append(f"{f}:{i}: use standard equivalent instead of {pat}")
    return errors


def check_no_std(files):
    """No std:: prefix — use 'using namespace std' instead."""
    errors = []
    for f in files:
        content = read_file(f)
        if content is None:
            continue
        if "std::" in content:
            errors.append(f"{f}: contains std:: (use 'using namespace std')")
    return errors


def check_have_solutions():
    """Every exercise with exercise.md must have a solution*.c* file."""
    errors = []
    root_folder = "src/exercises"
    if not os.path.isdir(root_folder):
        return errors
    for root, _dirs, _files in os.walk(root_folder):
        exercise = os.path.join(root, "exercise.md")
        if os.path.exists(exercise):
            pattern = os.path.join(root, "solution*.c*")
            if not glob.glob(pattern):
                errors.append(f"{exercise}: no solution file found")
    return errors


ALL_CHECKS = [
    ("check_ws", check_ws),
    ("check_ace_include", check_ace_include),
    ("check_include", check_include),
    ("check_license", check_license),
    ("check_exit", check_exit),
    ("check_firstinclude", check_firstinclude),
    ("check_perror", check_perror),
    ("check_fixme", check_fixme),
    ("check_while1", check_while1),
    ("check_usage", check_usage),
    ("check_pthread", check_pthread),
    ("check_usage_2", check_usage_2),
    ("check_exitzero", check_exitzero),
    ("check_check_header", check_check_header),
    ("check_for", check_for),
    ("check_semisemi", check_semisemi),
    ("check_return", check_return),
    ("check_braces", check_braces),
    ("check_ace", check_ace),
    ("check_no_std", check_no_std),
]


def main():
    files = git_tracked_sources()
    all_errors = []

    for name, check_func in ALL_CHECKS:
        errors = check_func(files)
        if errors:
            all_errors.append((name, errors))

    # check_have_solutions doesn't take files
    errors = check_have_solutions()
    if errors:
        all_errors.append(("check_have_solutions", errors))

    if all_errors:
        for name, errors in all_errors:
            print(f"=== {name} ({len(errors)} violations) ===", file=sys.stderr)
            for err in errors:
                print(f"  {err}", file=sys.stderr)
            print(file=sys.stderr)
        total = sum(len(e) for _, e in all_errors)
        print(
            f"{len(all_errors)} checks failed with {total} total violations",
            file=sys.stderr,
        )
        sys.exit(1)
    else:
        sys.exit(0)


if __name__ == "__main__":
    main()
