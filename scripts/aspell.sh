#!/bin/bash -e

aspell --conf-dir=. --conf=.aspell.conf check "$1"
