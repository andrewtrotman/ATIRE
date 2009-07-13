#AUTOMAKE_OPTIONS = foreign

AM_CFLAGS = -I$(srcdir)/../source/
AM_CPPFLAGS = $(AM_CFLAGS)
AM_LDFLAGS = @LIBS@

CC = g++

ant_src_dir = $(srcdir)/../source/

extra_sources = \
		$(ant_src_dir)/ctypes.c \
		$(ant_src_dir)/encoding_utf8.c \
		$(ant_src_dir)/encoding_factory.c

libuniseq_sources = \
		$(extra_sources) \
		uniseg_plugin.c \
		uniseg_settings.c \
		word.c \
		clist.c	\
		cwords.c \
		freq.c \
		freq_counter.c \
		freq_file.c	\
		freq_loader.c \
		index_file.c \
		file.c \
		qfreq.c	\
		seg.c \
		dic.c
		
libuniseg_headers = \
		address.h	\
		anyoption.h	\
		clist.h	\
		convert.h	\
		cwords.h	\
		dic_builder.h	\
		dic.h	\
		doc_base.h	\
		doc_freq.h	\
		doc.h	\
		doc_reader.h	\
		doc_seg.h	\
		fdoc.h	\
		file.h	\
		freq_file.h	\
		freq.h	\
		freq_loader.h	\
		index_file.h	\
		mdoc.h	\
		qconf.h	\
		qdoc.h	\
		qfreq.h	\
		qtypes.h	\
		qzer.h	\
		range.h	\
		seg.h	\
		timings.h	\
		udoc.h	\
		utilities.h	\
		uniseg_plugin.h \
		word.h			

lib_LTLIBRARIES = libuniseg.la

libuniseg_ladir = .
libuniseg_la_SOURCES = $(libuniseq_sources)
libuniseg_la_HEADERS = $(libuniseq_headers)
#libuniseg_la_LDFLAGS = -shared

noinst_PROGRAMS = test/uniseg

test_uniseg_SOURCES = test/uniseg.c
test_uniseg_LDADD = -L. -luniseg
test_uniseg_LDFLAGS = -static
test_uniseg_DEPENDENCIES = libuniseg.la
test_uniseg_CFLAGS =

TESTS = test/uniseg

