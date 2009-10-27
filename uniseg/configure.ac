#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([uniseg], [0.1], [lingxiang.tang@gmail.com])
AC_CONFIG_SRCDIR([freq_counter.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-luniseg':
# AC_CHECK_LIB([uniseg], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday pow sqrt])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
