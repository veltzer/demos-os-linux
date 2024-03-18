#!/bin/bash -e
# cgexec -g cpu:/cpu_demo ./app.sh
sudo bash -c "./app.sh > /sys/fs/cgroup/cpu_demo/cgroup.procs"
