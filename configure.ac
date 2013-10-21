#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([Atire], [0.3], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([atire/atire.c])
#AC_CONFIG_HEADERS([config.h])
#AC_CONFIG_MACRO_DIR([m4])

dnl for target_os
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL

# Checks for programs.
#AC_PROG_CC
AC_PROG_CXX

AM_PROG_CC_C_O

# Check for macros
AC_FUNC_MALLOC

AC_ARG_WITH([pi],
            AC_HELP_STRING([--with-pi],
                       [Define PARALLEL_INDEXING macro]),
        [CXXFLAGS="$CXXFLAGS -DPARALLEL_INDEXING"; CFLAGS="$CXXFLAGS"])
        
AC_ARG_WITH(mysql,
            AC_HELP_STRING([--with-mysql]),
        [
            CFLAGS="$CFLAGS -DANT_HAS_MYSQL"
            CXXFLAGS="$CXXFLAGS -DANT_HAS_MYSQL"
            HAS_MYSQL="yes"
		],
		[	
		]
		)
		
# Get MySQL library and include locations if set
AS_IF([test -z "$HAS_MYSQL"], [],
[
AC_ARG_WITH([mysql-include-path],
  [AS_HELP_STRING([--with-mysql-include-path],
    [location of the MySQL headers, defaults to /usr/include/mysql])],
  [
  	MYSQL_CFLAGS="-I$withval"
  	CXXFLAGS="$CXXFLAGS $MYSQL_CFLAGS"
  	CFLAGS="$CXXFLAGS"
  ],
  [
  	MYSQL_CFLAGS="-I/usr/include/mysql"
  	CXXFLAGS="$CXXFLAGS $MYSQL_CFLAGS"
  	CFLAGS="$CXXFLAGS"
  ])
AC_SUBST([MYSQL_CFLAGS])

AC_ARG_WITH([mysql-lib-path],
  [AS_HELP_STRING([--with-mysql-lib-path], [location of the MySQL libraries])],
  [
  	MYSQL_LIBS="-L$withval -lmysqlclient"
  	LIBS="$LIBS $MYSQL_LIBS"
  ],
  [
  	MYSQL_LIBS='-lmysqlclient'
  	LIBS="$LIBS $MYSQL_LIBS"
  ])
AC_SUBST([MYSQL_LIBS])
]
)
          
# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([z], [inflateReset], [LIBS="$LIBS -lz"; CXXFLAGS="$CXXFLAGS -DANT_HAS_ZLIB"; CFLAGS="$CXXFLAGS"])
AC_CHECK_LIB([bz2], [BZ2_bzDecompressInit], [LIBS="$LIBS -lbz2"; CXXFLAGS="$CXXFLAGS -DANT_HAS_BZLIB"; CFLAGS="$CXXFLAGS"])
#AC_CHECK_LIB([mysqlclient], [mysql_init], [LIBS="$LIBS $MYSQL_LIBS";])

# Checks for header files.
AC_HEADER_STDC++
AC_CHECK_HEADERS([float.h limits.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h values.h limits])

AC_ARG_ENABLE([java-feature],
              [AC_HELP_STRING([--disable-java-feature],
                              [disable Java feature])],
                              [                             
AC_ARG_VAR([JAVA_HOME], [Java Runtime Environment (JRE) location])

case $target_os in
	linux*)		JVM_OS=linux ;;
	solaris*) 	JVM_OS=solaris ;;
	*)          JVM_OS=$target_os ;;
esac  

case $target_cpu in
     x86_64) JVM_ARCH=amd64 ;;
     i?86)   JVM_ARCH=i386 ;;
     *)      JVM_ARCH=$target_cpu ;;
esac

dnl check for JAVA_HOME and add the java include to CFLAGS
AS_IF([test -z "$JAVA_HOME"],
[AC_MSG_FAILURE([JAVA_HOME has not been set.  JAVA_HOME must be set at run time to locate jni.h.])],       
[ 
CFLAGS="-I$JAVA_HOME/include -I$JAVA_HOME/include/$JVM_OS $CFLAGS"
CXXFLAGS="-I$JAVA_HOME/include -I$JAVA_HOME/include/$JVM_OS $CXXFLAGS"
])   

AC_CHECK_HEADER([jni.h], [have_jni=yes]) 
AC_SUBST([JAVA_HOME])
#AC_SUBST([JVM_ARCH])
#AS_IF([test X$enable_java_feature != Xno],
#[AS_IF([test X$have_jni != Xyes],
#       [AC_MSG_FAILURE([The Java Native Interface is required for Java feature.])])
#AS_IF([test -z "$JAVA_HOME"],
#[AC_MSG_WARN([JAVA_HOME has not been set.  JAVA_HOME must be set at run time to locate libjvm.])],
#[save_LDFLAGS=$LDFLAGS
#LDFLAGS="-L$JAVA_HOME/lib/$JVM_ARCH/client -L$JAVA_HOME/lib/$JVM_ARCH/server $LDFLAGS"
#AC_CHECK_LIB([jvm], [JNI_CreateJavaVM], [LIBS=$LIBS],
#            [AC_MSG_WARN([no libjvm found at JAVA_HOME])])
#LDFLAGS=$save_LDFLAGS
#])])                              
                              ])


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
AC_CONFIG_FILES([
				Makefile
				java/jni/Makefile
				])
AC_OUTPUT
