#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([xlink], [0.2.0], [lingxiang.tang@gmail.com])
AC_CONFIG_SRCDIR([src/config.h])
AM_INIT_AUTOMAKE
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S

AC_ARG_WITH(stpl,
            AC_HELP_STRING([--with-stpl=DIR],
                       [Define STPL directory for its include files]),
        [ if test -d "$withval" ; then
            CFLAGS="$CFLAGS -I$withval"
            CXXFLAGS="$CXXFLAGS -I$withval"
          else
            AC_MSG_ERROR([Cannot stat directory $withval])
          fi ] )
          
# Checks for libraries.
# FIXME: Replace `main' with a function in `-lant':
AC_CHECK_LIB([ant], [ant_easy_init])
# FIXME: Replace `main' with a function in `-ldl':
#AC_CHECK_LIB([dl], [dlopen], [LIBS="$LIBS -ldl"])
AC_CHECK_LIB([curl], [curl_easy_init], [LIBS="$LIBS -lcurl"])

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
