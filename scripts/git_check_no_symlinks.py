#!/usr/bin/env python

"""
This script runs "git ls-files" and checks that no files are symbolic links
"""

import subprocess
import os.path


def main():
    with subprocess.Popen(["git","ls-files"], stdout=subprocess.PIPE) as pipe:
        for line in pipe.stdout:
            current=line.decode("utf-8").rstrip()
            if os.path.islink(current):
                raise ValueError(f"found a symlink (this is bad) [{current}]")

if __name__ == "__main__":
    main()
