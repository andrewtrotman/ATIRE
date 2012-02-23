AM_CPPFLAGS = $(AM_CFLAGS)
AM_LDFLAGS = @LIBS@

CC = g++

AM_CPPFLAGS += -std=c++0x -pedantic 


lib_LTLIBRARIES = libatire_jni.la


libatire_jni_la_SOURCES = atire_wrap.c 
libatire_jni_la_LDFLAGS = 
libatire_jni_la_DEPENDENCIES = ../../libatire_index_param.la ../../libatire_index.la
libatire_jni_la_LIBADD = -L../../ -latire_index_param -L../../ -latire_index 

noinst_PROGRAMS = index_test

index_test_SOURCES = test/index_test.c
index_test_CFLAGS = -I$(srcdir)/include
index_test_LDADD = -L. -latire_jni -L../../ -latire_core -L../../ -latire_index_param -L../../ -latire_index
index_test_LDFLAGS = -static -export-dynamic
index_test_DEPENDENCIES = libatire_jni.la ../../libatire_core.la ../../libatire_index_param.la ../../libatire_index.la
