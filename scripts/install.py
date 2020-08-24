#!/usr/bin/env python

import subprocess
import config.deps


args = [
        "sudo",
        "apt-get",
        "install",
]
args.extend(config.deps.packs)
subprocess.check_call(args)
