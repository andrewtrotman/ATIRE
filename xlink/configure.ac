#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/config.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lant':
AC_CHECK_LIB([ant], [ant_easy_init])
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [dlopen])

# Checks for header files.
AC_CHECK_HEADERS([netdb.h netinet/in.h stddef.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname memset socket strcasecmp strchr strdup strndup strpbrk strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
