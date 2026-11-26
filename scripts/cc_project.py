#!/usr/bin/env python

"""
Build the cc.yaml projects of this repository as an rsconstruct mass generator.

    cc_project.py plan  SRC_ROOT OUT_DIR   # print the manifest of every output
    cc_project.py build SRC_ROOT OUT_DIR   # compile and link all of them

A cc.yaml sits in the directory of a multi-file C/C++ project and names its
libraries and programs (the format rsconstruct's cc creator used to read):

    cc: gcc                      # optional, default gcc
    cxx: g++                     # optional, default g++
    cflags: [-Wall]              # optional
    cxxflags: [-Wall]            # optional
    ldflags: []                  # optional
    include_dirs: [src/include]  # optional, relative to the project root
    libraries:
      - name: mymath
        lib_type: shared         # shared | static | both, default shared
        sources: [math.c]        # relative to the cc.yaml directory
        include_dirs: []         # optional, relative to the project root
        cflags: []
        cxxflags: []
        ldflags: []
    programs:
      - name: main.elf
        sources: [main.c]
        link: [mymath]           # libraries of this cc.yaml to link against
        include_dirs: []
        cflags: []
        cxxflags: []
        ldflags: [-lpthread]

Every output lands under OUT_DIR/<cc.yaml directory>/: objects in
obj/<target>/, libraries in lib/, programs in bin/. `plan` and `build`
enumerate the same targets with the same function, so what the plan
promises is exactly what the build writes.
"""

import json
import os
import subprocess
import sys
from dataclasses import dataclass, field
from typing import NoReturn

import yaml

MANIFEST_NAME = "cc.yaml"
CXX_EXTENSIONS = {".cc", ".cpp", ".cxx", ".C"}


@dataclass
class Target:
    """A library or a program of one cc.yaml."""

    name: str
    sources: list[str]
    include_dirs: list[str] = field(default_factory=list)
    cflags: list[str] = field(default_factory=list)
    cxxflags: list[str] = field(default_factory=list)
    ldflags: list[str] = field(default_factory=list)
    lib_type: str = "shared"
    link: list[str] = field(default_factory=list)


@dataclass
class Project:
    """One cc.yaml, with the fields that apply to all of its targets."""

    manifest: str
    cc: str
    cxx: str
    cflags: list[str]
    cxxflags: list[str]
    ldflags: list[str]
    include_dirs: list[str]
    libraries: list[Target]
    programs: list[Target]

    @property
    def directory(self) -> str:
        return os.path.dirname(self.manifest)

    def source(self, rel: str) -> str:
        """A manifest-relative source as a project-root-relative path."""
        return os.path.normpath(os.path.join(self.directory, rel))


@dataclass
class Output:
    """One predicted file and the inputs whose content determines it."""

    path: str
    sources: list[str]


def fail(message: str) -> NoReturn:
    sys.stderr.write(f"{sys.argv[0]}: {message}\n")
    sys.exit(1)


def string_list(table: dict[str, object], key: str, where: str) -> list[str]:
    value = table.get(key, [])
    if not isinstance(value, list):
        fail(f"{where}: '{key}' must be a list of strings")
    strings: list[str] = []
    for item in value:
        if not isinstance(item, str):
            fail(f"{where}: '{key}' must be a list of strings")
        strings.append(item)
    return strings


def parse_target(raw: object, where: str) -> Target:
    if not isinstance(raw, dict):
        fail(f"{where}: every target must be a mapping")
    name = raw.get("name")
    if not isinstance(name, str) or not name:
        fail(f"{where}: a target needs a non-empty 'name'")
    sources = string_list(raw, "sources", f"{where} [{name}]")
    if not sources:
        fail(f"{where} [{name}]: 'sources' must not be empty")
    lib_type = raw.get("lib_type", "shared")
    if lib_type not in ("shared", "static", "both"):
        fail(f"{where} [{name}]: lib_type must be shared, static or both")
    return Target(
        name=name,
        sources=sources,
        include_dirs=string_list(raw, "include_dirs", where),
        cflags=string_list(raw, "cflags", where),
        cxxflags=string_list(raw, "cxxflags", where),
        ldflags=string_list(raw, "ldflags", where),
        lib_type=lib_type,
        link=string_list(raw, "link", where),
    )


def parse_project(manifest: str) -> Project:
    with open(manifest, encoding="utf-8") as handle:
        raw = yaml.safe_load(handle)
    if not isinstance(raw, dict):
        fail(f"{manifest}: the manifest must be a mapping")
    libraries = [parse_target(t, manifest) for t in raw.get("libraries", [])]
    programs = [parse_target(t, manifest) for t in raw.get("programs", [])]
    known = {t.name for t in libraries}
    for program in programs:
        for lib in program.link:
            if lib not in known:
                fail(f"{manifest} [{program.name}]: links unknown library '{lib}'")
    return Project(
        manifest=manifest,
        cc=str(raw.get("cc", "gcc")),
        cxx=str(raw.get("cxx", "g++")),
        cflags=string_list(raw, "cflags", manifest),
        cxxflags=string_list(raw, "cxxflags", manifest),
        ldflags=string_list(raw, "ldflags", manifest),
        include_dirs=string_list(raw, "include_dirs", manifest),
        libraries=libraries,
        programs=programs,
    )


def find_projects(src_root: str) -> list[Project]:
    manifests: list[str] = []
    for dirpath, dirnames, filenames in os.walk(src_root):
        dirnames.sort()
        if MANIFEST_NAME in filenames:
            manifests.append(os.path.join(dirpath, MANIFEST_NAME))
    return [parse_project(m) for m in sorted(manifests)]


def is_cxx(path: str) -> bool:
    return os.path.splitext(path)[1] in CXX_EXTENSIONS


def object_path(obj_dir: str, target: Target, rel_source: str) -> str:
    """Mirror the source's manifest-relative path under obj/<target>/, so two
    sources with the same stem in different directories cannot collide."""
    parts = [p for p in rel_source.split("/") if p not in ("", ".", "..")]
    stem = os.path.splitext("/".join(parts))[0] + ".o"
    return os.path.join(obj_dir, target.name, stem)


def library_paths(lib_dir: str, lib: Target) -> list[str]:
    paths = []
    if lib.lib_type in ("static", "both"):
        paths.append(os.path.join(lib_dir, f"lib{lib.name}.a"))
    if lib.lib_type in ("shared", "both"):
        paths.append(os.path.join(lib_dir, f"lib{lib.name}.so"))
    return paths


def plan_project(project: Project, out_dir: str) -> list[Output]:
    base = os.path.join(out_dir, project.directory)
    obj_dir = os.path.join(base, "obj")
    lib_dir = os.path.join(base, "lib")
    bin_dir = os.path.join(base, "bin")
    outputs: list[Output] = []
    lib_sources = {
        lib.name: [project.source(s) for s in lib.sources] for lib in project.libraries
    }
    for lib in project.libraries:
        outputs.extend(
            Output(
                object_path(obj_dir, lib, rel),
                [project.manifest, project.source(rel)],
            )
            for rel in lib.sources
        )
        outputs.extend(
            Output(path, [project.manifest, *lib_sources[lib.name]])
            for path in library_paths(lib_dir, lib)
        )
    for program in project.programs:
        outputs.extend(
            Output(
                object_path(obj_dir, program, rel),
                [project.manifest, project.source(rel)],
            )
            for rel in program.sources
        )
        linked = [s for lib in program.link for s in lib_sources[lib]]
        outputs.append(
            Output(
                os.path.join(bin_dir, program.name),
                [
                    project.manifest,
                    *(project.source(s) for s in program.sources),
                    *linked,
                ],
            )
        )
    return outputs


def run(cmd: list[str]) -> None:
    sys.stderr.write(" ".join(cmd) + "\n")
    result = subprocess.run(cmd, check=False)
    if result.returncode != 0:
        fail(f"command failed with exit code {result.returncode}: {' '.join(cmd)}")


def compile_object(
    project: Project, target: Target, rel: str, obj: str, *, pic: bool
) -> None:
    source = project.source(rel)
    cxx = is_cxx(source)
    cmd = [project.cxx if cxx else project.cc, "-c"]
    cmd.extend(project.cxxflags if cxx else project.cflags)
    cmd.extend(target.cxxflags if cxx else target.cflags)
    cmd.extend(f"-I{d}" for d in project.include_dirs)
    cmd.extend(f"-I{d}" for d in target.include_dirs)
    if pic:
        cmd.append("-fPIC")
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    cmd.extend(["-o", obj, source])
    run(cmd)


def build_project(project: Project, out_dir: str) -> None:
    base = os.path.join(out_dir, project.directory)
    obj_dir = os.path.join(base, "obj")
    lib_dir = os.path.join(base, "lib")
    bin_dir = os.path.join(base, "bin")
    for lib in project.libraries:
        objects = [object_path(obj_dir, lib, rel) for rel in lib.sources]
        for rel, obj in zip(lib.sources, objects, strict=True):
            compile_object(project, lib, rel, obj, pic=lib.lib_type != "static")
        has_cxx = any(is_cxx(s) for s in lib.sources)
        for path in library_paths(lib_dir, lib):
            os.makedirs(lib_dir, exist_ok=True)
            if path.endswith(".a"):
                run(["ar", "rcs", path, *objects])
            else:
                driver = project.cxx if has_cxx else project.cc
                run(
                    [
                        driver,
                        "-shared",
                        "-o",
                        path,
                        *objects,
                        *project.ldflags,
                        *lib.ldflags,
                    ]
                )
    for program in project.programs:
        objects = [object_path(obj_dir, program, rel) for rel in program.sources]
        for rel, obj in zip(program.sources, objects, strict=True):
            compile_object(project, program, rel, obj, pic=False)
        has_cxx = any(is_cxx(s) for s in program.sources)
        driver = project.cxx if has_cxx else project.cc
        os.makedirs(bin_dir, exist_ok=True)
        cmd = [driver, "-o", os.path.join(bin_dir, program.name), *objects]
        if program.link:
            cmd.append(f"-L{lib_dir}")
            cmd.extend(f"-l{lib}" for lib in program.link)
        cmd.extend(project.ldflags)
        cmd.extend(program.ldflags)
        run(cmd)


def main() -> None:
    if len(sys.argv) != 4 or sys.argv[1] not in ("plan", "build"):
        fail("usage: cc_project.py plan|build SRC_ROOT OUT_DIR")
    mode, src_root, out_dir = sys.argv[1:]
    projects = find_projects(src_root)
    if mode == "plan":
        outputs = [o for p in projects for o in plan_project(p, out_dir)]
        manifest = {
            "version": 1,
            "outputs": [{"path": o.path, "sources": o.sources} for o in outputs],
        }
        json.dump(manifest, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return
    for project in projects:
        build_project(project, out_dir)


if __name__ == "__main__":
    main()
