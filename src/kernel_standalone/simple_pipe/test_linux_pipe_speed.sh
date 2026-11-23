#!/bin/bash

# This script tests the speed of the regular linux pipe...
# Just break this script with 'CTRL+C'.

#cat /dev/zero | pipemeter > /dev/null
{ dd if=/dev/zero bs=50000 count=1000000 || true; } | pipemeter > /dev/null
