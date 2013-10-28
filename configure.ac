#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ([2.69])
#AC_INIT (package, version, [bug-report], [tarname], [url])
AC_INIT([inputsystem], [1.0], [vldmr256@gmail.com], [inputsystem])
AM_INIT_AUTOMAKE
#AC_CONFIG_SRCDIR([test.c])
AC_CONFIG_SRCDIR([include/InputSystem.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([rt], [aio_read aio_error aio_return])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h memory.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday memset select socket strchr strrchr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 include/Makefile
])
AC_OUTPUT
#AC_CONFIG_FILES([Makefile])
#AC_OUTPUT(Makefile.auto src/Makefile doc/Makefile examples/Makefile man/Makefile scripts/Makefile)

