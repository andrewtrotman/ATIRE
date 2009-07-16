#AUTOMAKE_OPTIONS = foreign
#SUBDIRS = plugins

SRCDIR = source


AM_LDFLAGS = @LIBS@

# using HEADER HASH FUNCTION
AM_CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
       -Wno-endif-labels -Wno-missing-braces
       
# using RANDOM HASH FUNCTION       
#AM_CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DRANDOM_HASHER=1 \
#       -Wno-endif-labels -Wno-missing-braces       

CC = g++

#OBJDIR = 
#BINDIR = 

library_sources =  \
	$(SRCDIR)/encoding_utf8.c \
	$(SRCDIR)/encoding_factory.c \
	$(SRCDIR)/parser.c \
	$(SRCDIR)/parser_readability.c \
	$(SRCDIR)/universal_parser.c \
    $(SRCDIR)/memory_index_hash_node.c \
    $(SRCDIR)/memory_index.c \
    $(SRCDIR)/memory_index_stats.c \
    $(SRCDIR)/hash_table.c \
    $(SRCDIR)/postings_piece.c \
    $(SRCDIR)/ctypes.c \
    $(SRCDIR)/file.c \
    $(SRCDIR)/file_internals.c \
    $(SRCDIR)/maths.c \
    $(SRCDIR)/memory.c \
    $(SRCDIR)/search_engine.c \
    $(SRCDIR)/search_engine_accumulator.c \
    $(SRCDIR)/mean_average_precision.c \
    $(SRCDIR)/assessment.c \
    $(SRCDIR)/assessment_ANT.c \
    $(SRCDIR)/assessment_INEX.c \
    $(SRCDIR)/assessment_factory.c \
    $(SRCDIR)/relevant_document.c \
    $(SRCDIR)/stats.c \
    $(SRCDIR)/time_stats.c \
    $(SRCDIR)/search_engine_stats.c \
    $(SRCDIR)/search_engine_forum.c \
    $(SRCDIR)/search_engine_forum_INEX.c \
    $(SRCDIR)/search_engine_forum_TREC.c \
    $(SRCDIR)/search_engine_readability.c \
    $(SRCDIR)/str.c \
    $(SRCDIR)/stop_word.c \
    $(SRCDIR)/disk.c \
    $(SRCDIR)/disk_internals.c \
	$(SRCDIR)/directory_recursive_iterator.c \
    $(SRCDIR)/btree_iterator.c \
    $(SRCDIR)/top_k_sort.c \
    $(SRCDIR)/stemmer.c	\
    $(SRCDIR)/porter.c \
    $(SRCDIR)/lovins.c \
    $(SRCDIR)/stemmer_factory.c \
    $(SRCDIR)/paice_husk.c \
    $(SRCDIR)/learned_wikipedia_stem.c \
    $(SRCDIR)/relevant_topic.c \
	$(SRCDIR)/bitstream.c \
	$(SRCDIR)/compress_elias_gamma.c \
	$(SRCDIR)/compress_elias_delta.c \
	$(SRCDIR)/compress_golomb.c \
	$(SRCDIR)/compress_simple9.c \
	$(SRCDIR)/compress_relative10.c \
	$(SRCDIR)/compress_carryover12.c \
	$(SRCDIR)/compress_variable_byte.c \
	$(SRCDIR)/compress_none.c \
	$(SRCDIR)/compress_sigma.c \
	$(SRCDIR)/compression_factory.c \
	$(SRCDIR)/indexer_param_block.c \
	$(SRCDIR)/ant_param_block.c \
	$(SRCDIR)/readability_factory.c \
	$(SRCDIR)/readability_none.c \
	$(SRCDIR)/readability_dale_chall.c \
	$(SRCDIR)/plugin_manager.c \
	$(SRCDIR)/version.c
				
library_headers = \
	$(SRCDIR)/ant_param_block.h	\
	$(SRCDIR)/assessment_ANT.h	\
	$(SRCDIR)/assessment_factory.h	\
	$(SRCDIR)/assessment.h	\
	$(SRCDIR)/assessment_INEX.h	\
	$(SRCDIR)/bitstream.h	\
	$(SRCDIR)/btree.h	\
	$(SRCDIR)/btree_head_node.h	\
	$(SRCDIR)/btree_iterator.h	\
	$(SRCDIR)/compress_carryover12.h	\
	$(SRCDIR)/compress_carryover12_internals.h	\
	$(SRCDIR)/compress_elias_delta.h	\
	$(SRCDIR)/compress_elias_gamma.h	\
	$(SRCDIR)/compress_golomb.h	\
	$(SRCDIR)/compress.h	\
	$(SRCDIR)/compression_factory.h	\
	$(SRCDIR)/compression_factory_scheme.h	\
	$(SRCDIR)/compress_none.h	\
	$(SRCDIR)/compress_relative10.h	\
	$(SRCDIR)/compress_sigma_frequency.h	\
	$(SRCDIR)/compress_sigma.h	\
	$(SRCDIR)/compress_simple9.h	\
	$(SRCDIR)/compress_variable_byte.h	\
	$(SRCDIR)/ctypes.h	\
	$(SRCDIR)/directory_iterator.h	\
	$(SRCDIR)/directory_recursive_iterator.h	\
	$(SRCDIR)/disk_directory.h	\
	$(SRCDIR)/disk.h	\
	$(SRCDIR)/disk_internals.h	\
	$(SRCDIR)/file.h	\
	$(SRCDIR)/file_internals.h	\
	$(SRCDIR)/fundamental_types.h	\
	$(SRCDIR)/hash_table.h	\
	$(SRCDIR)/indexer_param_block.h	\
	$(SRCDIR)/learned_wikipedia_stem.h	\
	$(SRCDIR)/lovins.h	\
	$(SRCDIR)/maths.h	\
	$(SRCDIR)/mean_average_precision.h	\
	$(SRCDIR)/memory.h	\
	$(SRCDIR)/memory_index.h	\
	$(SRCDIR)/memory_index_hash_node.h	\
	$(SRCDIR)/memory_index_stats.h	\
	$(SRCDIR)/NEXI.h	\
	$(SRCDIR)/paice_husk.h	\
	$(SRCDIR)/parser.h	\
	$(SRCDIR)/parser_readability.h	\
	$(SRCDIR)/universal_parser.h \
	$(SRCDIR)/porter.h	\
	$(SRCDIR)/postings_piece.h	\
	$(SRCDIR)/relevant_document.h	\
	$(SRCDIR)/relevant_topic.h	\
	$(SRCDIR)/search_engine_accumulator.h	\
	$(SRCDIR)/search_engine_btree_leaf.h	\
	$(SRCDIR)/search_engine_btree_node.h	\
	$(SRCDIR)/search_engine_forum.h	\
	$(SRCDIR)/search_engine_forum_INEX.h	\
	$(SRCDIR)/search_engine_forum_TREC.h	\
	$(SRCDIR)/search_engine.h	\
	$(SRCDIR)/search_engine_posting.h	\
	$(SRCDIR)/search_engine_stats.h	\
	$(SRCDIR)/stats.h	\
	$(SRCDIR)/stemmer_factory.h	\
	$(SRCDIR)/stemmer.h	\
	$(SRCDIR)/stemmer_lovins.h	\
	$(SRCDIR)/stemmer_none.h	\
	$(SRCDIR)/stemmer_otago.h	\
	$(SRCDIR)/stemmer_paice_husk.h	\
	$(SRCDIR)/stemmer_porter.h	\
	$(SRCDIR)/stop_word.h	\
	$(SRCDIR)/str.h	\
	$(SRCDIR)/string_pair.h	\
	$(SRCDIR)/time_stats.h	\
	$(SRCDIR)/top_k_sort.h	\
	$(SRCDIR)/readability_factory.h \
	$(SRCDIR)/readability_none.h \
	$(SRCDIR)/readability_dale_chall.h \
	$(SRCDIR)/plugin.h \	
	$(SRCDIR)/plugin_manager.h \
	$(SRCDIR)/version.h
				
noinst_LTLIBRARIES = libant.la

libant_ladir = $(LIBDIR)
libant_la_SOURCES = $(library_sources)
libant_la_HEADERS = $(library_headers)


# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = ant index

ant_SOURCES = $(SRCDIR)/ant.c
ant_LDADD = -L${OBJDIR} -lant -ldl
ant_LDFLAGS = -rdynamic
ant_DEPENDENCIES = libant.la

index_SOURCES = $(SRCDIR)/index.c
index_LDADD = -L${OBJDIR} -lant -ldl
index_LDFLAGS = -rdynamic
index_DEPENDENCIES = libant.la

noinst_PROGRAMS = ant_dictionary ant_plugins

ant_dictionary_SOURCES = $(SRCDIR)/ant_dictionary.c 
ant_dictionary_LDADD = -L${OBJDIR} -lant
ant_dictionary_DEPENDENCIES = libant.la
# ############################################################################
#  use this if it is really necessary to have same source files with libant.la
#  otherwise, it will cause automake problem:
#  object `XXXX.$(OBJEXT)' created both with libtool and without
# ant_dictionary_CFLAGS = $(AM_CFLAGS)
						 
ant_plugins_SOURCES = $(SRCDIR)/ant_plugins.c \
					$(SRCDIR)/plugin_manager.c
ant_plugins_CFLAGS = $(AM_CFLAGS)
ant_plugins_LDADD = -ldl
ant_plugins_LDFLAGS = -rdynamic
ant_plugins_DEPENDENCIES = 


