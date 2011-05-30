#AUTOMAKE_OPTIONS = foreign
#SUBDIRS = plugins

#ACLOCAL_AMFLAGS = -I m4

SRCDIR = source
LTWDIR = Link-The-Wiki
TOOLDIR = tools
#LIBDIR = .

AM_LDFLAGS = @LIBS@

# using HEADER HASH FUNCTION
AM_CFLAGS = -g -Wall -DONE_PARSER -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
		-DSPECIAL_COMPRESSION=1 -DTOP_K_SEARCH -DTOP_K_READ_AND_DECOMPRESSOR \
       -Wno-endif-labels -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings 
       
# for parallel indexing, but it will be configured by configure script with --with-pi option
# -DPARALLEL_INDEXING
       
       
# using RANDOM HASH FUNCTION       
#AM_CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DRANDOM_HASHER=1 \
#       -Wno-endif-labels -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings     

CC = g++

AM_CFLAGS += -std=c++0x -pedantic 

#OBJDIR = 
#BINDIR = 

core_sources =  \
	$(SRCDIR)/parser.c \
	$(SRCDIR)/parser_readability.c \
    $(SRCDIR)/memory_index_hash_node.c \
    $(SRCDIR)/memory_index.c \
	$(SRCDIR)/memory_index_one.c	\
	$(SRCDIR)/memory_indexer.c	\
    $(SRCDIR)/hash_table.c \
    $(SRCDIR)/postings_piece.c \
    $(SRCDIR)/ctypes.c \
    $(SRCDIR)/unicode_tables.c \
    $(SRCDIR)/file.c \
    $(SRCDIR)/file_internals.c \
    $(SRCDIR)/file_memory.c	\
    $(SRCDIR)/maths.c \
    $(SRCDIR)/memory.c \
    $(SRCDIR)/search_engine.c \
    $(SRCDIR)/search_engine_accumulator.c \
    $(SRCDIR)/mean_average_precision.c \
    $(SRCDIR)/assessment.c \
    $(SRCDIR)/assessment_ANT.c \
    $(SRCDIR)/assessment_INEX.c \
    $(SRCDIR)/assessment_TREC.c \
    $(SRCDIR)/assessment_factory.c \
    $(SRCDIR)/relevant_document.c \
    $(SRCDIR)/stats.c \
	$(SRCDIR)/stats_time.c	\
	$(SRCDIR)/stats_memory_index.c \
	$(SRCDIR)/stats_search_engine.c	\
    $(SRCDIR)/search_engine_result.c \
    $(SRCDIR)/search_engine_result_iterator.c \
    $(SRCDIR)/search_engine_forum.c \
    $(SRCDIR)/search_engine_forum_INEX.c \
    $(SRCDIR)/search_engine_forum_INEX_efficiency.c	\
	$(SRCDIR)/search_engine_forum_INEX_bep.c \
	$(SRCDIR)/search_engine_forum_INEX_focus.c \
    $(SRCDIR)/search_engine_forum_TREC.c \
    $(SRCDIR)/search_engine_readability.c \
    $(SRCDIR)/search_engine_memory_index.c \
    $(SRCDIR)/directory_iterator_deflate.c \
    $(SRCDIR)/str.c \
    $(SRCDIR)/stop_word.c \
    $(SRCDIR)/disk.c \
    $(SRCDIR)/directory_iterator_csv.c \
	$(SRCDIR)/directory_iterator_recursive.c \
	$(SRCDIR)/directory_iterator.c	\
	$(SRCDIR)/directory_iterator_tar.c	\
	$(SRCDIR)/directory_iterator_file.c	\
	$(SRCDIR)/directory_iterator_multiple.c \
	$(SRCDIR)/directory_iterator_internals.c \
	$(SRCDIR)/directory_iterator_pkzip.c \
	$(SRCDIR)/directory_iterator_warc.c \
	$(SRCDIR)/directory_iterator_warc_gz_recursive.c \
	$(SRCDIR)/directory_iterator_compressor.c \
    $(SRCDIR)/btree_iterator.c \
    $(SRCDIR)/stem_otago.c \
    $(SRCDIR)/stem_otago_v2.c \
    $(SRCDIR)/stem_krovetz.c \
    $(SRCDIR)/stem_paice_husk.c \
    $(SRCDIR)/stem_porter.c \
    $(SRCDIR)/stem_lovins.c \
    $(SRCDIR)/stem_s.c	\
    $(SRCDIR)/stemmer.c	\
    $(SRCDIR)/stemmer_factory.c \
    $(SRCDIR)/stemmer_term_similarity.c \
    $(SRCDIR)/stemmer_term_similarity_threshold.c \
    $(SRCDIR)/stemmer_term_similarity_weighted.c \
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
	$(SRCDIR)/compress_text_deflate.c \
	$(SRCDIR)/compress_text_none.c \
	$(SRCDIR)/compress_text_bz2.c \
	$(SRCDIR)/compression_text_factory.c \
	$(SRCDIR)/instream_buffer.c \
	$(SRCDIR)/instream_lzo.c \
	$(SRCDIR)/index_document.c \
	$(SRCDIR)/readability_factory.c \
	$(SRCDIR)/readability_dale_chall.c \
	$(SRCDIR)/version.c \
	$(SRCDIR)/ranking_function.c	\
	$(SRCDIR)/ranking_function_readability.c	\
	$(SRCDIR)/ranking_function_bm25.c	\
	$(SRCDIR)/ranking_function_impact.c \
	$(SRCDIR)/ranking_function_lmd.c \
	$(SRCDIR)/ranking_function_bose_einstein.c \
	$(SRCDIR)/ranking_function_lmjm.c \
	$(SRCDIR)/ranking_function_divergence.c \
	$(SRCDIR)/ranking_function_term_count.c \
	$(SRCDIR)/ranking_function_inner_product.c \
	$(SRCDIR)/ranking_function_kbtfidf.c	\
	$(SRCDIR)/ranking_function_dlh13.c \
	$(SRCDIR)/plugin_manager.c 	\
	$(SRCDIR)/nexi.c \
	$(SRCDIR)/NEXI_ant.c \
	$(SRCDIR)/NEXI_term.c \
	$(SRCDIR)/NEXI_term_ant.c \
	$(SRCDIR)/NEXI_term_iterator.c \
	$(SRCDIR)/instream_file.c	\
	$(SRCDIR)/instream_deflate.c \
	$(SRCDIR)/instream_bz2.c \
	$(SRCDIR)/semaphores.c \
	$(SRCDIR)/threads.c \
	$(SRCDIR)/critical_section.c \
	$(SRCDIR)/sockets.c \
	$(SRCDIR)/event.c \
	$(SRCDIR)/channel_file.c \
	$(SRCDIR)/channel_socket.c \
	$(SRCDIR)/focus.c \
	$(SRCDIR)/focus_article.c \
	$(SRCDIR)/focus_lowest_tag.c \
	$(SRCDIR)/focus_results_list.c \
	$(SRCDIR)/query.c
				
core_noinst_headers = \
	$(SRCDIR)/ant_param_block.h	\
	$(SRCDIR)/assessment_ANT.h	\
	$(SRCDIR)/assessment_factory.h	\
	$(SRCDIR)/assessment.h	\
	$(SRCDIR)/assessment_INEX.h	\
	$(SRCDIR)/assessment_TREC.h \
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
	$(SRCDIR)/compression_text_factory.h \
	$(SRCDIR)/compress_none.h	\
	$(SRCDIR)/compress_relative10.h	\
	$(SRCDIR)/compress_sigma_frequency.h	\
	$(SRCDIR)/compress_sigma.h	\
	$(SRCDIR)/compress_simple9.h	\
	$(SRCDIR)/compress_variable_byte.h	\
	$(SRCDIR)/compress_text.h \
	$(SRCDIR)/compression_text_factory_scheme.h \
	$(SRCDIR)/compress_text_deflate_internals.h \
	$(SRCDIR)/compress_text_deflate.h \
	$(SRCDIR)/compress_text_none.h \
	$(SRCDIR)/compress_text_bz2.h \
	$(SRCDIR)/ctypes.h	\
	$(SRCDIR)/critical_section.h \
	$(SRCDIR)/directory_iterator.h	\
	$(SRCDIR)/directory_iterator_csv.h \
	$(SRCDIR)/directory_iterator_file.h	\
	$(SRCDIR)/directory_iterator_tar.h	\
	$(SRCDIR)/directory_iterator_pkzip.h \
	$(SRCDIR)/directory_iterator_multiple.h \
	$(SRCDIR)/directory_iterator_internals.h \
	$(SRCDIR)/directory_iterator_object.h \
	$(SRCDIR)/directory_iterator_tar_file_header.h	\
	$(SRCDIR)/directory_iterator_recursive.h	\
	$(SRCDIR)/directory_iterator_warc.h \
	$(SRCDIR)/directory_iterator_warc_gz_recursive.h \
	$(SRCDIR)/directory_iterator_compressor.h \
	$(SRCDIR)/directory_iterator_deflate.h \
	$(SRCDIR)/disk_directory.h	\
	$(SRCDIR)/disk.h	\
	$(SRCDIR)/file.h	\
	$(SRCDIR)/file_internals.h	\
	$(SRCDIR)/file_memory.h	\
	$(SRCDIR)/fundamental_types.h	\
	$(SRCDIR)/hash_table.h	\
	$(SRCDIR)/instream_buffer.h \
	$(SRCDIR)/indexer_param_block.h	\
	$(SRCDIR)/indexer_param_block_rank.h \
	$(SRCDIR)/index_document.h \
	$(SRCDIR)/learned_wikipedia_stem.h	\
	$(SRCDIR)/maths.h	\
	$(SRCDIR)/mean_average_precision.h	\
	$(SRCDIR)/memory.h	\
	$(SRCDIR)/memory_index.h	\
	$(SRCDIR)/memory_index_hash_node.h	\
	$(SRCDIR)/memory_index_one_node.h	\
	$(SRCDIR)/memory_index_one.h	\
	$(SRCDIR)/memory_indexer_node.h	\
	$(SRCDIR)/memory_indexer.h	\
	$(SRCDIR)/NEXI.h	\
	$(SRCDIR)/NEXI_ant.h \
	$(SRCDIR)/NEXI_term.h \
	$(SRCDIR)/NEXI_term_ant.h \
	$(SRCDIR)/NEXI_term_iterator.h \
	$(SRCDIR)/parser.h	\
	$(SRCDIR)/parser_readability.h	\
	$(SRCDIR)/universal_parser.h \
	$(SRCDIR)/postings_piece.h	\
	$(SRCDIR)/relevant_document.h	\
	$(SRCDIR)/relevant_topic.h	\
	$(SRCDIR)/search_engine_accumulator.h	\
	$(SRCDIR)/search_engine_btree_leaf.h	\
	$(SRCDIR)/search_engine_btree_node.h	\
	$(SRCDIR)/search_engine_forum.h	\
	$(SRCDIR)/search_engine_forum_INEX.h	\
	$(SRCDIR)/search_engine_forum_INEX_efficiency.h	\
	$(SRCDIR)/search_engine_forum_INEX_bep.h \
	$(SRCDIR)/search_engine_forum_INEX_focus.h \
	$(SRCDIR)/search_engine_forum_TREC.h	\
	$(SRCDIR)/search_engine.h	\
	$(SRCDIR)/search_engine_posting.h	\
	$(SRCDIR)/search_engine_result.h \
	$(SRCDIR)/search_engine_result_iterator.h \
	$(SRCDIR)/search_engine_memory_index.h \
	$(SRCDIR)/semaphores.h \
	$(SRCDIR)/producer_consumer.h \
	$(SRCDIR)/stats.h	\
	$(SRCDIR)/stats_time.h	\
	$(SRCDIR)/stats_search_engine.h	\
	$(SRCDIR)/stats_memory_index.h \
	$(SRCDIR)/stem_otago.h \
	$(SRCDIR)/stem_otago_v2.h \
    $(SRCDIR)/stem_krovetz.h \
	$(SRCDIR)/stem_porter.h	\
	$(SRCDIR)/stem_paice_husk.h	\
	$(SRCDIR)/stem_lovins.h	\
	$(SRCDIR)/stem_s.h	\
	$(SRCDIR)/stemmer_factory.h	\
	$(SRCDIR)/stemmer.h	\
	$(SRCDIR)/stemmer_lovins.h	\
	$(SRCDIR)/stemmer_none.h	\
	$(SRCDIR)/stemmer_otago.h	\
	$(SRCDIR)/stemmer_paice_husk.h	\
	$(SRCDIR)/stemmer_porter.h	\
	$(SRCDIR)/stemmer_term_similarity.h \
	$(SRCDIR)/stemmer_term_similarity_threshold.h \
	$(SRCDIR)/stemmer_term_similarity_weighted.h \
	$(SRCDIR)/sockets.h \
	$(SRCDIR)/stop_word.h	\
	$(SRCDIR)/str.h	\
	$(SRCDIR)/string_pair.h	\
	$(SRCDIR)/threads.h \
	$(SRCDIR)/readability_factory.h \
	$(SRCDIR)/readability_dale_chall.h \
	$(SRCDIR)/relevant_document_passage.h \
	$(SRCDIR)/version.h \
	$(SRCDIR)/ranking_function.h	\
	$(SRCDIR)/ranking_function_readability.h	\
	$(SRCDIR)/ranking_function_bm25.h \
	$(SRCDIR)/ranking_function_impact.h \
	$(SRCDIR)/ranking_function_lmd.h \
	$(SRCDIR)/ranking_function_bose_einstein.h \
	$(SRCDIR)/ranking_function_divergence.h \
	$(SRCDIR)/ranking_function_term_count.h \
	$(SRCDIR)/ranking_function_inner_product.h \
	$(SRCDIR)/ranking_function_kbtfidf.h	\
	$(SRCDIR)/ranking_function_dlh13.c \
	$(SRCDIR)/instream_file.h	\
	$(SRCDIR)/instream_deflate.h \
	$(SRCDIR)/instream_bz2.h \
	$(SRCDIR)/instream_lzo.h \
	$(SRCDIR)/instream.h \
	$(SRCDIR)/event.h \
	$(SRCDIR)/event_internals.h \
	$(SRCDIR)/plugin.h \
	$(SRCDIR)/plugin_manager.h \
	$(SRCDIR)/unicode_pair.h \
	$(SRCDIR)/unicode_tables.h \
	$(SRCDIR)/channel.h \
	$(SRCDIR)/channel_file.h \
	$(SRCDIR)/channel_socket.h \
	$(SRCDIR)/sockets_internals.h \
	$(SRCDIR)/focus.h \
	$(SRCDIR)/focus_article.h \
	$(SRCDIR)/focus_result.h \
	$(SRCDIR)/focus_lowest_tag.h \
	$(SRCDIR)/focus_hash_node.h \
	$(SRCDIR)/focus_results_list.h \
	$(SRCDIR)/query.h
	
IGNORE_LIST = $(SRCDIR)/ant_ant.c \
			  $(SRCDIR)/ant_api.c \
			  $(SRCDIR)/ant_plugins.c \
			  $(SRCDIR)/ant_dictionary.c \
			  $(SRCDIR)/test_compression.c
			  
#ANT_PARAM_LIST = $(shell ls $(SRCIDR)/ant_param_*.c)
#INDEX_PARAM_LIST = $(shell ls $(SRCDIR)/indexer_param_*.c)		  
#
#all_sources = $(shell ls $(SRCDIR)/*.c)
#core_noinst_headers = $(shell ls $(SRCDIR)/*.h)
#core_sources = $(filter-out $(IGNORE_LIST) $(ANT_PARAM_LIST) $(INDEX_PARAM_LIST), $(all_sources))
	
ANT_PARAM_LIST = $(SRCDIR)/ant_param_block.c
INDEX_PARAM_LIST = $(SRCDIR)/indexer_param_block.c \
				   $(SRCDIR)/indexer_param_block_rank.c \
				   $(SRCDIR)/indexer_param_block_stem.c
	
library_headers = \
	$(SRCDIR)/ant_api.h \
	$(SRCDIR)/ant_params.h	
	
includedir = $(prefix)/include
include_HEADERS = $(library_headers)
	
lib_LTLIBRARIES = libantcore.la libant.la

libantcore_la_SOURCES = $(core_sources)
libantcore_la_LIBADD = -ldl -lpthread

libant_la_SOURCES = $(SRCDIR)/ant_api.c
libant_la_DEPENDENCIES = libantcore.la
libant_la_LIBADD = -L. -lantcore
libant_la_LIBFLAGS = -static
# libant_ladir = $(prefix)/lib
# libant_la_HEADERS = $(library_headers)
# $(library_noinst_headers)

#ANT_LIBS = -L. -lantcore -ldl -lpthread
ANT_LIBS = -L. -lantcore

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
noinst_PROGRAMS = ant ant_ant index ant_dictionary \
			ant_plugins filelist remove_head link_extract link_index \
			link_this link_index_merge link_extract_pass2 link_length_correlate \
			topic_tree_cas bindiff term_frequencies topic_tree INEXqrels_to_run \
			foltbl_to_aspt zipf_graph test_stop

ant_SOURCES = \
	$(SRCDIR)/ant.c \
	$(ANT_PARAM_LIST) \
	$(INDEX_PARAM_LIST)
#	$(SRCDIR)/indexer_param_block_rank.c \
#	$(SRCDIR)/ant_param_block.c
ant_LDADD = ${ANT_LIBS}
#ant_LDFLAGS = -rdynamic
ant_LDFLAGS = -static -export-dynamic
ant_DEPENDENCIES = libantcore.la

ant_ant_SOURCES = \
	$(SRCDIR)/ant_ant.c \
	$(ANT_PARAM_LIST) \
	$(INDEX_PARAM_LIST)
#	$(SRCDIR)/indexer_param_block_rank.c \
#	$(SRCDIR)/ant_param_block.c
ant_ant_LDADD = ${ANT_LIBS} -lant
#ant_ant_LDFLAGS = -rdynamic
ant_ant_LDFLAGS = -static -export-dynamic
ant_ant_DEPENDENCIES = libant.la

index_SOURCES = \
	$(SRCDIR)/index.c \
	$(INDEX_PARAM_LIST)
#	$(SRCDIR)/indexer_param_block.c \
#	$(SRCDIR)/indexer_param_block_rank.c \
#	$(SRCDIR)/ant_param_block.c
index_LDADD = ${ANT_LIBS}
#index_LDFLAGS = -rdynamic
index_LDFLAGS = -static -export-dynamic
index_DEPENDENCIES = libantcore.la

ant_dictionary_SOURCES = $(SRCDIR)/ant_dictionary.c 
ant_dictionary_LDADD = ${ANT_LIBS}
ant_dictionary_DEPENDENCIES = libant.la
##############################################################################
#  use this if it is really necessary to have same source files with libant.la
#  otherwise, it will cause automake problem:
#  object `XXXX.$(OBJEXT)' created both with libtool and without
# ant_dictionary_CFLAGS = $(AM_CFLAGS)
##############################################################################
						 
ant_plugins_SOURCES = $(SRCDIR)/ant_plugins.c \
					$(SRCDIR)/plugin_manager.c
ant_plugins_CFLAGS = $(AM_CFLAGS)
ant_plugins_LDADD = ${ANT_LIBS}
ant_plugins_LDFLAGS = -rdynamic
ant_plugins_DEPENDENCIES = 
	

#################################################################################
#                               TOOLS
#################################################################################
filelist_SOURCES = $(TOOLDIR)/filelist.c
filelist_LDFLAGS = -rdynamic
filelist_LDADD = ${ANT_LIBS}
filelist_DEPENDENCIES = libantcore.la

topic_tree_cas_SOURCES = $(TOOLDIR)/topic_tree_cas.c
topic_tree_cas_LDFLAGS = -rdynamic
topic_tree_cas_LDADD = ${ANT_LIBS}
topic_tree_cas_DEPENDENCIES = libantcore.la

bindiff_SOURCES = $(TOOLDIR)/bindiff.c
bindiff_LDFLAGS = -rdynamic
bindiff_LDADD = ${ANT_LIBS}
bindiff_DEPENDENCIES = libantcore.la

term_frequencies_SOURCES = $(TOOLDIR)/term_frequencies.c
term_frequencies_LDFLAGS = -rdynamic
term_frequencies_LDADD = ${ANT_LIBS}
term_frequencies_DEPENDENCIES = libantcore.la

topic_tree_SOURCES = $(TOOLDIR)/topic_tree.c
topic_tree_LDFLAGS = -rdynamic
topic_tree_LDADD = ${ANT_LIBS}
topic_tree_DEPENDENCIES = libantcore.la

INEXqrels_to_run_SOURCES = $(TOOLDIR)/INEXqrels_to_run.c
INEXqrels_to_run_LDFLAGS = -rdynamic
INEXqrels_to_run_LDADD = ${ANT_LIBS}
INEXqrels_to_run_DEPENDENCIES = libantcore.la

foltbl_to_aspt_SOURCES = $(TOOLDIR)/foltbl_to_aspt.c
foltbl_to_aspt_LDFLAGS = -rdynamic
foltbl_to_aspt_LDADD = ${ANT_LIBS}
foltbl_to_aspt_DEPENDENCIES = libantcore.la

zipf_graph_SOURCES = $(TOOLDIR)/zipf_graph.c
zipf_graph_LDFLAGS = -rdynamic
zipf_graph_LDADD = ${ANT_LIBS}
zipf_graph_DEPENDENCIES = libantcore.la

test_stop_SOURCES = $(TOOLDIR)/test_stop.c \
				$(SRCDIR)/stop_word.c \
				$(SRCDIR)/str.c
test_stop_CFLAGS = $(AM_CFLAGS)

#################################################################################
#                               LINK-THE-WIKI
#################################################################################

link_extract_SOURCES = $(LTWDIR)/link_extract.c
#link_extract_LDFLAGS = -rdynamic
link_extract_LDFLAGS = -static
link_extract_LDADD = ${ANT_LIBS}
link_extract_DEPENDENCIES = libantcore.la

link_index_SOURCES = $(LTWDIR)/link_index.c
#link_index_LDFLAGS = -rdynamic
link_index_LDFLAGS = -static
link_index_LDADD = ${ANT_LIBS}
link_index_DEPENDENCIES = libantcore.la

link_this_SOURCES = $(LTWDIR)/link_this.c
#link_this_LDFLAGS = -rdynamic
link_this_LDFLAGS = -static
link_this_LDADD = ${ANT_LIBS}
link_this_DEPENDENCIES = libantcore.la

link_index_merge_SOURCES = $(LTWDIR)/link_index_merge.c
link_index_merge_LDFLAGS = -rdynamic
link_index_merge_LDADD = ${ANT_LIBS}
link_index_merge_DEPENDENCIES = libantcore.la

link_extract_pass2_SOURCES = $(LTWDIR)/link_extract_pass2.c
#link_extract_pass2_LDFLAGS = -rdynamic
link_extract_pass2_LDFLAGS = -static
link_extract_pass2_LDADD = ${ANT_LIBS}
link_extract_pass2_DEPENDENCIES = libantcore.la

link_length_correlate_SOURCES = $(LTWDIR)/link_length_correlate.c
link_length_correlate_LDFLAGS = -rdynamic
link_length_correlate_LDADD = ${ANT_LIBS}
link_length_correlate_DEPENDENCIES = libantcore.la

remove_head_SOURCES = $(LTWDIR)/remove_head.c
remove_head_LDFLAGS = -rdynamic
remove_head_LDADD = ${ANT_LIBS}
remove_head_DEPENDENCIES = libantcore.la

