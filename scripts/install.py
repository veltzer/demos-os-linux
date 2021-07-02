import subprocess
import config.deps


args = [
        # "sudo",
        "apt-get",
        "-y",
        "install",
]
args.extend(config.deps.packs)
subprocess.check_call(args)
