#!/bin/bash

# This script simply checks that stap(1) basically works

# export ARCH=x86_64
# export CROSS_COMPILE=x86_64-linux-gnu-
stap -ve 'probe begin { printf("hello world\n"); exit() }'
