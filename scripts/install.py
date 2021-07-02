#!/usr/bin/env python3

"""
This script does not use 'sudo' so you need to run it as sudo yourself with:
    $ sudo python -m scripts.install
"""

import subprocess
import os
import config.deps


# make apt noninteractive
os.environ['DEBIAN_FRONTEND']='noninteractive'
subprocess.check_call([
    "apt-get",
    "--yes",
    "--quiet",
    "update",
])
args = [
    "apt-get",
    "--yes",
    "--quiet",
    "-y",
    "install",
]
args.extend(config.deps.packs)
subprocess.check_call(args)
