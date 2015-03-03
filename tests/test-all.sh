#!/bin/sh

BIN=./test_ioctl

# FIXME: perf do not forward the exit code, and the test always succeed
perf record -c 1 -e faults -- $BIN go
perf record -e instructions -- $BIN go
