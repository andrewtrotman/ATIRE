#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ANT], [0.3], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([source/ant_param_block.c])
#AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL

# Checks for programs.
#AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h values.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
#AC_FUNC_STRNLEN
AC_CHECK_FUNCS([getcwd gettimeofday memmove memset sqrt strchr strcspn strdup strpbrk strrchr strstr])

#AC_CONFIG_FILES([Link-The-Wiki/Makefile
#                 evaluation/Makefile
#                 gp_rank/Makefile
#                 Makefile
#                 source/Makefile
#                 stemmer/Makefile])
AC_CONFIG_FILES([Makefile
				source/Makefile
				])
AC_OUTPUT
