#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([uniseg], [0.7.1], [lingxiang.tang@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([freq_counter.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

AM_PROG_CC_C_O

AC_ARG_ENABLE(ant,
            AC_HELP_STRING([--enable-ant],
                       [to make uniseg as a plugin a ANT search engine]),
        [
            CFLAGS="$CFLAGS -DWITH_ANT_PLUGIN"
            CXXFLAGS="$CXXFLAGS -DWITH_ANT_PLUGIN"
            ]
		)

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
