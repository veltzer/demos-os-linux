#!/bin/sh
sudo cgcreate -g cpu:/cpu_demo
sudo cgcreate -g cpu:/
sudo cgset -r cpu.cfs_quota_us=50000 cpu_demo
