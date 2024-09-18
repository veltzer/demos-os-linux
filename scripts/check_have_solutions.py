#!/usr/bin/env python

"""
This script will check that every exercise in src/exercises has a solution.
"""

import os
import os.path
import sys
import glob


def main():
    root_folder = "src/exercises"
    errors=[]
    for root, _dirs, _files in os.walk(root_folder):
        exercise=os.path.join(root,"exercise.md")
        if os.path.exists(exercise):
            glob_pattern=os.path.join(root, "solution*.c*")
            if not glob.glob(glob_pattern):
                errors.append(exercise)
    if errors:
        for error in errors:
            print(error)
        sys.exit(1)
    else:
        sys.exit(0)


if __name__ == "__main__":
    main()
