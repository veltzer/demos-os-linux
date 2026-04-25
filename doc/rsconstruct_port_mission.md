# Mission: port demos-os-linux from GNU Make to rsconstruct

## Status

The project ships **both** `Makefile` and `rsconstruct.toml` today. CI runs only
through `rsconstruct` (see `.github/workflows/build.yml`). The Makefile is still
the local developer workflow and contains years of accumulated tooling that
rsconstruct does not yet replicate.

The goal of this mission is to retire the Makefile by either porting each
behavior into rsconstruct or consciously dropping it. This document is the
inventory of what still lives only in the Makefile.

## What rsconstruct already covers (per `rsconstruct.toml`)

| Capability                     | rsconstruct processor                                      |
|--------------------------------|------------------------------------------------------------|
| Compile `.c`/`.cc` with GCC    | `processor.cc_single_file.gcc`                             |
| Compile `.c`/`.cc` with Clang  | `processor.cc_single_file.clang`                           |
| `cppcheck`                     | `processor.cppcheck`                                       |
| `clang-tidy`                   | `processor.clang_tidy`                                     |
| `cpplint`                      | `processor.cpplint`                                        |
| Per-folder Makefiles           | `processor.make` (recursively invokes `make` in subdirs)   |
| Kernel module builds           | `processor.linux_module`                                   |
| `mdl`, `rumdl`, `zspell`       | `processor.mdl`, `processor.rumdl`, `processor.zspell`     |
| `taplo`, `iyamllint`           | `processor.taplo`, `processor.iyamllint`                   |
| `mypy`, `ruff`                 | `processor.mypy`, `processor.ruff`                         |
| `shellcheck`                   | `processor.shellcheck`                                     |
| Whole-project sanity script    | `processor.script.check_all` (currently `enabled=false`)   |
| OS / pip dependencies          | `[dependencies] system = [...]`, `pip = [...]`             |

## What the Makefile still does that rsconstruct does not

These need a decision (port, drop, or keep as a side-channel target).

### 1. Build artifacts beyond `.elf`
- **Disassembly** (`out/gcc/<base>.dis`) via `objdump --disassemble --source [--demangle]`.
  Useful for teaching; the targets `C_GCC_DIS` / `CC_GCC_DIS` produce one per source
  file. No rsconstruct equivalent today.
- **Preprocessed output** (`out/gcc/<base>.p`) via `gcc -E`. Same story.
- **Separate object files** (`.o`/`.oo`) and a separate link step. rsconstruct's
  `cc_single_file` collapses compile+link into one command per file, so there is
  no addressable intermediate object.

### 2. The `wrapper_compile.py` shim
`scripts/wrapper_compile.py` wraps every compile/link with optional **ccache**,
**timing**, and **dbg** behavior controlled by the `CCACHE` and `DO_MKDBG`
flags. rsconstruct invokes the compiler directly. Porting options:
- ignore (drop ccache support);
- teach rsconstruct to use ccache via `cc = "ccache gcc"` style overrides;
- keep the wrapper but call it from rsconstruct's `cc =` field.

### 3. Markdown checks
- `out/%.mdl` rule runs `gems/bin/mdl` with `GEM_HOME=gems` against each `.md`
  under `src/`. rsconstruct has `processor.mdl`, so this is mostly a port; check
  whether the bundled `gems/` directory and `.mdlrc`/`.mdl.style.rb` are
  honored.
- `out/%.aspell` rule runs `aspell list` and errors on any unknown word.
  No rsconstruct processor for aspell. Either port (custom `script.*`
  processor) or migrate to `zspell` which is already wired in.

### 4. The `check_*` targets (project-wide grep-based hygiene)
Roughly 25 phony targets named `check_ws`, `check_main`, `check_ace_include`,
`check_include`, `check_license`, `check_exit`, `check_pgrep`,
`check_firstinclude`, `check_check`, `check_perror`, `check_fixme`,
`check_while1`, `check_usage`, `check_pthread`, `check_usage_2`,
`check_gitignore`, `check_exitzero`, `check_no_symlinks`,
`check_check_header`, `check_return`, `check_braces`, `check_ace`,
`check_colons`, `check_no_std`, `check_semisemi`, `check_for`, `check_dots`,
`check_syn`, `check_files`, `check_tests_for_drivers`. They are aggregated by
`check_all` and gated by `DO_CHECK_ALL`, but `processor.script.check_all` is
currently `enabled = false` in `rsconstruct.toml`.

These are pure git-grep / shell pipelines and depend on `pymakehelper no_err`
to invert the exit code (find = pass, find = fail). Two reasonable paths:
- Reimplement each as logic inside `scripts/check_all.py` and turn the
  processor on.
- Drop the ones that have outlived their value (the file is full of comments
  showing several already disabled).

### 5. Code-formatting targets
- `format_uncrustify` runs `uncrustify` against the entire user-space tree
  (different `-l C` vs `-l CPP` invocations).
- `format_astyle` and `format_indent` exist but `$(error ...)` out — explicitly
  disabled.

rsconstruct has no formatter integration; these are intended to be invoked by
the developer, not on every build. They could stay as a small `Makefile.dev`
or be ported to `scripts/format.py` and removed from rsconstruct's scope.

### 6. Kernel-related ergonomics
The Makefile has many `kernel_*` phony targets that are pure operator
conveniences:
- `kernel_clean`, `kernel_check`, `kernel_build`, `kernel_help`
- `kernel_tail[f]`, `kernel_syslog_tail[f]`, `kernel_dmesg`, `kernel_dmesg_clean`
- `kernel_halt`, `kernel_reboot`, `kernel_makeeasy`

These are not build steps — they are interactive shortcuts (some require
sudo). Drop or move to a `scripts/kernel.sh` helper.

### 7. Source-tree introspection targets
- `sloccount`, `cloc`, `count_files`
- `find_not_source`, `find_not_target`, `find_not_source_target`,
  `find_exercises`
- `archive_src`, `archive_ace` (build tarballs from `git archive`)
- `git_maintain` (= `git gc`)
- `debug` (dumps every internal Make variable)
- `todo` (= `grep TODO`)

All convenience targets, none of which rsconstruct should grow. Move to
`scripts/`.

### 8. Pylint
`out/pylint.stamp` runs `pylint` over every `.py` under the tree, gated by
`DO_PYLINT`. rsconstruct already runs `ruff` and `mypy` via processors, so
`pylint` is redundant unless we keep it for the stricter style checks.
Decision: drop, unless someone asks for it back.

### 9. Shell-script syntax check
`$(ALL_STAMP)` rule runs `shellcheck --severity=error --shell=bash` against
every `*.sh` under `src/`. Rsconstruct has `processor.shellcheck` already
configured for `["src", "misc", "scripts"]` — verify the flags match
(`--severity=error`, `--external-sources`, `--source-path="$HOME"`) and remove
this Makefile rule.

### 10. CI-aware behavior
The Makefile flips `DO_STP=0` / `DO_CHP=0` when `GITHUB_WORKFLOW` is set. In an
rsconstruct world this becomes either:
- Two `rsconstruct.toml` profiles (one per environment), or
- Per-processor `enabled = !ci` switches, or
- Just removing the variants if CI is the source of truth.

### 11. Build-flag switches
Many `DO_*` toggles at the top of the Makefile:
`DO_GCC`, `DO_CLANG`, `DO_CPPCHECK`, `DO_TIDY`, `DO_CPPLINT`, `DO_PYLINT`,
`DO_MD_MDL`, `DO_MD_ASPELL`, `DO_CHECK_ALL`, `DO_CHECK_SYNTAX`, `DO_STP`,
`DO_CHP`, `DO_ADD_STD`, `DO_DEP_WRAPPER`, `DO_ALLDEP`, `DEBUG`, `OPT`,
`CCACHE`. rsconstruct toggles processors at the section level
(`enabled = false`) — most map cleanly. The flag-style ones (`DEBUG`, `OPT`,
`-std=`) need either separate processor instances (`cc_single_file.gcc_debug`)
or a build-profile mechanism if rsconstruct grows one.

### 12. `out/` directory layout
Make builds into `out/` mirroring source paths. rsconstruct uses `out/<processor>/...`.
Any tool, IDE config, or `.gitignore` rule that assumed the Makefile's layout
needs updating.

## Suggested order of operations

1. Confirm `processor.shellcheck` flags match the Makefile's `$(ALL_STAMP)` rule;
   delete the Makefile rule.
2. Decide on ccache: if keeping, route via rsconstruct's `cc` field. Drop
   `wrapper_compile.py` once nothing references it.
3. Port the `check_*` targets that still earn their keep into
   `scripts/check_all.py` and flip the processor to `enabled = true`.
4. Move all interactive/admin targets (`kernel_*`, `archive_*`, `sloccount`,
   `cloc`, `find_not_*`, `format_*`) into shell scripts under `scripts/`.
5. Decide whether disassembly / preprocessed-output artifacts are still part
   of the teaching workflow. If yes, request a rsconstruct processor; if no,
   drop the rules.
6. Replace `out/%.aspell` with `zspell` (already configured) or write a
   one-off `script.aspell` processor.
7. Delete `Makefile` and `Makefile.mk`. Update `HOWTO`, `README.md`, and
   `doc/TODO_build_system.txt` accordingly.

## Open questions

- Are disassembly / `.p` outputs used in classes? If yes, they need a porting
  story; if no, they are dead weight.
- Is the bundled `gems/` directory (Bundler-managed `mdl`) intentional, or
  can we depend on the system-installed `mdl` only? The CI uses the system one.
- Does anyone still run the format targets locally? `format_astyle` and
  `format_indent` already self-`$(error)`; only `format_uncrustify` is live.
