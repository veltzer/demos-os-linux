#!/usr/bin/env python

"""
This script runs anything for you and ignores the run if the file has a tag
that you specify.

What is the reason for this script?
We want the option not to run tidy on files which have:
        FLAGS: NOTIDY
in them to enable us to write not-well-structured HTML files
to demo various things
"""

import sys
import subprocess


def get_flags(filename: str) -> set[str]:
    """ read the content of a file and get all the flags declared in it """
    flags = set()
    with open(filename, encoding="utf-8") as stream:
        for line in stream:
            line = line.strip()
            if line.find("FLAGS:") != -1:
                all_flags = line.split(":")[1]
                for flag in all_flags.split(","):
                    flags.add(flag.strip())
    return flags

def main():
    """ main entry point """
    filename = sys.argv[1]
    flag = sys.argv[2]
    cmd = sys.argv[3:]
    flags = get_flags(filename)
    if flag in flags:
        sys.exit(0)
    subprocess.check_call(cmd)


if __name__ == "__main__":
    main()
