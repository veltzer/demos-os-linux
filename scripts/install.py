#!/usr/bin/env python3

"""
This script supports running it with sudo or without. With arguments it runs with sudo.
If you want to run the sudo yourself use:
    $ sudo python -m scripts.install
"""

import subprocess
import os
import sys
import config.deps

do_sudo = len(sys.argv) > 1
# make apt noninteractive
os.environ['DEBIAN_FRONTEND']='noninteractive'
args=[]
if do_sudo:
    args.append("sudo")
args.extend([
    "apt-get",
    "--yes",
    "--quiet",
    "update",
])
subprocess.check_call(args)
args=[]
if do_sudo:
    args.append("sudo")
args.extend([
    "apt-get",
    "--yes",
    "--quiet",
    "-y",
    "install",
])
args.extend(config.deps.packages)
subprocess.check_call(args)
