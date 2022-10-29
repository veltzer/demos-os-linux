#!/usr/bin/env python

"""
This script will ultimately:
- run a 64 bit virtual machine.
- wait till its up
- pull all the sources on it.
- clean and compile all code.
- return the result and logs from that compilation.

Currently it just runs the virtual machine...
"""

import sys
import subprocess

if len(sys.argv)!=1:
    raise ValueError("command line issue")

# run the command
subprocess.check_output(["virtualbox","--startvm","ubuntu64bit"])
