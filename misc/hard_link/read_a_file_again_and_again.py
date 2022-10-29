#!/usr/bin/env python

"""
This demo shows the fact that removing a file does not stop the process accessing it from working
and even working correctly...
"""

import time
import shutil


def main():
    filename="/tmp/passwd"
    shutil.copy("/etc/passwd", filename)
    print(f"while I am working please remove [{filename}] and see that I keep working right...")
    with open(filename, "r") as stream:
        while True:
            for line in stream:
                print(line, end="")
                time.sleep(1)
            stream.seek(0)
