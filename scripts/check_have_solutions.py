#!/usr/bin/env python

"""
This script will check that every exercise in src/exercises has a solution.
"""

import os
import os.path


def main():
    root_folder = "src/exercises"
    for root, _dirs, _files in os.walk(root_folder):
        exercise=os.path.join(root,"exercise.md")
        print(exercise)
        if os.path.exists(exercise):
            solution_cc=os.path.join(root,"solution.cc")
            solution_c=os.path.join(root,"solution.c")
            assert os.path.exists(solution_c) or os.path.exists(solution_cc)


if __name__ == "__main__":
    main()
