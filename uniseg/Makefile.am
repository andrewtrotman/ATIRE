#AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = $(AM_CFLAGS)
AM_LDFLAGS = @LIBS@

CC = g++

libuniseq_sources = \
		sys_file.c \
		sys_files.c \
		config.c \
		stop_word.c \
		encoding_utf8.c \
		encoding_factory.c \
		uniseg.c \
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
		seg2.c \
		dic.c \
		dic2freq.c
		
libuniseg_headers = \
		sys_file.h \
		sys_files.h \
		string_utils.h \
		config.h \
		stop_word.h \
		address.h	\
		anyoption.h	\
		clist.h	\
		convert.h	\
		cwords.h	\
		dic.h	\
		dic2freq.h	\
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
		seg2.h	\
		timings.h	\
		udoc.h	\
		utilities.h	\
		uniseg.h \
		word.h			

noinst_HEADERS = $(libuniseq_headers)

lib_LTLIBRARIES = libuniseg.la

#libuniseg_ladir = .
libuniseg_la_SOURCES = $(libuniseq_sources)
#libuniseg_la_HEADERS = $(libuniseq_headers)
#libuniseg_la_LDFLAGS = -shared

noinst_PROGRAMS = test/uniseg test/stopword

test_uniseg_SOURCES = test/uniseg.c
test_uniseg_LDADD = -L. -luniseg
test_uniseg_LDFLAGS = -static
test_uniseg_DEPENDENCIES = libuniseg.la
test_uniseg_CFLAGS =

test_stopword_SOURCES = test/stopword.c stop_word.c
test_stopword_CFLAGS =

TESTS = test/uniseg

