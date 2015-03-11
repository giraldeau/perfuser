AM_CFLAGS = -I $(top_builddir)/lib
bin_PROGRAMS = test_ioctl test_sigbench test_siginfo

test_ioctl_SOURCES = test_ioctl.c
test_ioctl_LDADD = $(top_builddir)/lib/libperfuser.la

test_sigbench_SOURCES = test_sigbench.c
test_sigbench_LDADD = $(top_builddir)/lib/libperfuser.la

test_siginfo_SOURCES = test_siginfo.c

check_SCRIPTS = test-all.sh
TESTS = $(check_SCRIPTS)

