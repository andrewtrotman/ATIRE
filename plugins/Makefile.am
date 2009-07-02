#AUTOMAKE_OPTIONS = foreign

AM_CFLAGS = -I$(srcdir)/../source/
AM_CPPFLAGS = $(AM_CFLAGS)
AM_LDFLAGS = @LIBS@

CC = g++

libuniseq_sources = \
		word.c	
		
A	=	clist.c	\
		cwords.c	\
		dic_builder.c	\
		dic.c	\
		doc_base.c	\
		doc.c	\
		doc_freq.c	\
		doc_reader.c	\
		doc_seg.c	\
		fdoc.c	\
		file.c	\
		freq.c	\
		freq_file.c	\
		freq_loader.c	\
		index_file.c	\
		mdoc.c	\
		qconf.c	\
		qdoc.c	\
		qfreq.c	\
		qzer.c	\
		seg.c	\
		udoc.c	
		
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
		word.h			

lib_LTLIBRARIES = libuniseg.la

libuniseg_ladir = .
libuniseg_la_SOURCES = $(libuniseq_sources)
libuniseg_la_HEADERS = $(libuniseq_headers)
libuniseg_la_LDFLAGS = -shared