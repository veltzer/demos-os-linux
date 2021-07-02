import subprocess
import config.deps


subprocess.check_call([
    "apt-get",
    "-y",
    "update",
])
args = [
        # "sudo",
        "apt-get",
        "-y",
        "install",
]
args.extend(config.deps.packs)
subprocess.check_call(args)
