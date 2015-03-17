#!/bin/sh

BIN=./test_ioctl

# FIXME: perf do not forward the exit code of the executed app,
# the test always succeed

echo 0 | sudo tee /proc/sys/kernel/kptr_restrict
perf record -c 1 -e faults -- $BIN go
if ! grep QEMU /proc/cpuinfo > /dev/null; then
	perf record -e instructions -- $BIN go
fi
