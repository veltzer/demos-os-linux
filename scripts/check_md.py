#!/usr/bin/env python

"""
Script to check the correctness of a course syllabus written in .md format
"""

from typing import List
import sys

if len(sys.argv) != 2:
    print(f"usage: {sys.argv[0]} [filename]", file=sys.stderr)
    sys.exit(1)

REQUIRED = [
    "Description",
    "Tags",
    "Duration",
    "Intended Audience",
    "Prerequisites",
    "Objectives",
    "Exercises",
    "Outline",
    "References",
    "We will not cover",
    "Installations",
    "Notes",
    "Links",
]
MUST = [
    "Description",
    "Duration",
    # "Prerequisites",
    # "Objectives",
    "Outline",
]


def is_sublist(list1: List[str], list2: List[str]) -> bool:
    """
    Check that one list is a sublist of the other
    """
    pos2 = 0
    pos1 = 0
    while pos2 < len(list2) and pos1 < len(list1):
        if list2[pos2] == list1[pos1]:
            pos1 += 1
        pos2 += 1
    return pos1 == len(list1)


def main():
    """ main entry point """
    filename = sys.argv[1]
    headers = []
    title = None
    with open(filename, "r", encoding="utf-8") as stream:
        for line in stream:
            line = line.rstrip()
            if line.startswith("## "):
                headers.append(line[3:])
            if line.startswith("# "):
                if title is None:
                    title = line[3:]
                else:
                    raise ValueError("two titles in one file")
    assert title is not None, "have no title"
    if not is_sublist(headers, REQUIRED):
        print("Sublist errors")
        print("found headers ->")
        print(str(headers))
        print("required headers -> ")
        print(str(REQUIRED))
        sys.exit(1)
    assert all(header in headers for header in MUST), (headers, MUST)


if __name__ == "__main__":
    main()
