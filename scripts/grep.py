#!/usr/bin/env python

"""
Implemting a better grep in python

    Mark Veltzer <mark@veltzer.net>
"""
import re # for .compile, .finditer
import sys # for argv, exit
import os # for .path.join

# command line usage...
if len(sys.argv)!=4:
    print("usage: grep.py [expr] [fileregexp] [folder]")
    sys.exit(1)
# first compile the regular expression to search for...
c=re.compile(sys.argv[1])
cf=re.compile(sys.argv[2])
folder=sys.argv[3]
debug=False
printOnlyFiles=False
printedFiles=set()
#debug=True

for root,dirs,files in os.walk(folder):
    for file in files:
        full=os.path.join(root,file)
        #if debug:
        #    print(f"file is [{full}]")
        if cf.match(full):
            if debug:
                print(f"doing file [{full}]")
            with open(full, "r") as stream:
                for num,line in enumerate(stream):
                    for x in c.finditer(line):
                        if printOnlyFiles:
                            if not full in printedFiles:
                                print(full)
                                printedFiles.add(full)
                        else:
                            print(f"{full}, {num}: {line[:-1]}")
