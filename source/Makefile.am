
AM_LDFLAGS = @LIBS@

AM_CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
       -Wno-endif-labels -Wno-missing-braces

CC = g++

#OBJDIR = 
#BINDIR = 

library_sources =  \
	encoding.c \
	encoding_factory.c \
	parser.c \
	universal_parser.c \
    memory_index_hash_node.c \
    memory_index.c \
    memory_index_stats.c \
    hash_table.c \
    postings_piece.c \
    ctypes.c \
    file.c \
    file_internals.c \
    maths.c \
    memory.c \
    search_engine.c \
    search_engine_accumulator.c \
    mean_average_precision.c \
    assessment.c \
    assessment_ANT.c \
    assessment_INEX.c \
    assessment_factory.c \
    relevant_document.c \
    stats.c \
    time_stats.c \
    search_engine_stats.c \
    search_engine_forum.c \
    search_engine_forum_INEX.c \
    search_engine_forum_TREC.c \
    str.c \
    stop_word.c \
    disk.c \
    disk_internals.c \
	directory_recursive_iterator.c \
    btree_iterator.c \
    top_k_sort.c \
    stemmer.c	\
    porter.c \
    lovins.c \
    stemmer_factory.c \
    paice_husk.c \
    learned_wikipedia_stem.c \
    relevant_topic.c \
	bitstream.c \
	compress_elias_gamma.c \
	compress_elias_delta.c \
	compress_golomb.c \
	compress_simple9.c \
	compress_relative10.c \
	compress_carryover12.c \
	compress_variable_byte.c \
	compress_none.c \
	compress_sigma.c \
	compression_factory.c \
	indexer_param_block.c \
    ant_param_block.c \
    version.c
				
library_headers = ant_param_block.h	\
	assessment_ANT.h	\
	assessment_factory.h	\
	assessment.h	\
	assessment_INEX.h	\
	bitstream.h	\
	btree.h	\
	btree_head_node.h	\
	btree_iterator.h	\
	compress_carryover12.h	\
	compress_carryover12_internals.h	\
	compress_elias_delta.h	\
	compress_elias_gamma.h	\
	compress_golomb.h	\
	compress.h	\
	compression_factory.h	\
	compression_factory_scheme.h	\
	compress_none.h	\
	compress_relative10.h	\
	compress_sigma_frequency.h	\
	compress_sigma.h	\
	compress_simple9.h	\
	compress_variable_byte.h	\
	ctypes.h	\
	directory_iterator.h	\
	directory_recursive_iterator.h	\
	disk_directory.h	\
	disk.h	\
	disk_internals.h	\
	file.h	\
	file_internals.h	\
	fundamental_types.h	\
	hash_table.h	\
	indexer_param_block.h	\
	learned_wikipedia_stem.h	\
	lovins.h	\
	maths.h	\
	mean_average_precision.h	\
	memory.h	\
	memory_index.h	\
	memory_index_hash_node.h	\
	memory_index_stats.h	\
	NEXI.h	\
	paice_husk.h	\
	parser.h	\
	porter.h	\
	postings_piece.h	\
	relevant_document.h	\
	relevant_topic.h	\
	search_engine_accumulator.h	\
	search_engine_btree_leaf.h	\
	search_engine_btree_node.h	\
	search_engine_forum.h	\
	search_engine_forum_INEX.h	\
	search_engine_forum_TREC.h	\
	search_engine.h	\
	search_engine_posting.h	\
	search_engine_stats.h	\
	stats.h	\
	stemmer_factory.h	\
	stemmer.h	\
	stemmer_lovins.h	\
	stemmer_none.h	\
	stemmer_otago.h	\
	stemmer_paice_husk.h	\
	stemmer_porter.h	\
	stop_word.h	\
	str.h	\
	string_pair.h	\
	time_stats.h	\
	top_k_sort.h	\
	version.h
				
noinst_LTLIBRARIES = libant.la

libant_ladir = $(LIBDIR)
libant_la_SOURCES = $(library_sources)
libant_la_HEADERS = $(library_headers)


# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = ant index

ant_SOURCES = ant.c
ant_LDADD = -L${OBJDIR} -lant
ant_DEPENDENCIES = libant.la

index_SOURCES = index.c
index_LDADD = -L${OBJDIR} -lant
index_DEPENDENCIES = libant.la




