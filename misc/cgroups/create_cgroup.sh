#!/bin/bash -e
sudo cgdelete cpu:/cpu_demo
sudo cgcreate -g cpu:/cpu_demo
# to see all parameters
# sudo cgget -g cpu:/cpu_demo
sudo cgset -r cpu.max=50000 cpu_demo
