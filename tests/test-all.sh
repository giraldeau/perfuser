#!/bin/sh

BIN=./test_ioctl

perf record -c 1 -e faults -- $BIN go

