AM_CFLAGS = -I $(top_builddir)/lib
bin_PROGRAMS = test_ioctl

test_ioctl_SOURCES = test_ioctl.c
test_ioctl_LDADD = $(top_builddir)/lib/libperfuser.la

check_SCRIPTS = test-all.sh
TESTS = $(check_SCRIPTS)

