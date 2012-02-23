LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .c

ATIRE_DIR := ../../atire
SRC_DIR := ../../source
INCLUDE := include

CORE_SOURCES =  \
	$(SRC_DIR)/arithmetic_model_bigram.c \
	$(SRC_DIR)/arithmetic_model_tables.c \
	$(SRC_DIR)/arithmetic_model_unigram.c \
	$(SRC_DIR)/assessment_ANT.c \
	$(SRC_DIR)/assessment.c \
	$(SRC_DIR)/assessment_factory.c \
	$(SRC_DIR)/assessment_INEX.c \
	$(SRC_DIR)/assessment_TREC.c \
	$(SRC_DIR)/barrier.c \
	$(SRC_DIR)/bitstream.c \
	$(SRC_DIR)/bitstring.c \
	$(SRC_DIR)/bitstring_iterator.c \
	$(SRC_DIR)/btree_iterator.c \
	$(SRC_DIR)/channel_file.c \
	$(SRC_DIR)/channel_socket.c \
	$(SRC_DIR)/compress_carryover12.c \
	$(SRC_DIR)/compress_elias_delta.c \
	$(SRC_DIR)/compress_elias_gamma.c \
	$(SRC_DIR)/compress_golomb.c \
	$(SRC_DIR)/compression_factory.c \
	$(SRC_DIR)/compression_text_factory.c \
	$(SRC_DIR)/compress_none.c \
	$(SRC_DIR)/compress_relative10.c \
	$(SRC_DIR)/compress_sigma.c \
	$(SRC_DIR)/compress_simple9.c \
	$(SRC_DIR)/compress_text_bz2.c \
	$(SRC_DIR)/compress_text_deflate.c \
	$(SRC_DIR)/compress_text_none.c \
	$(SRC_DIR)/compress_text_snappy.c \
	$(SRC_DIR)/compress_variable_byte.c \
	$(SRC_DIR)/critical_section.c \
	$(SRC_DIR)/ctypes.c \
	$(SRC_DIR)/directory_iterator.c \
	$(SRC_DIR)/directory_iterator_compressor.c \
	$(SRC_DIR)/directory_iterator_csv.c \
	$(SRC_DIR)/directory_iterator_deflate.c \
	$(SRC_DIR)/directory_iterator_file.c \
	$(SRC_DIR)/directory_iterator_file_buffered.c \
	$(SRC_DIR)/directory_iterator_internals.c \
	$(SRC_DIR)/directory_iterator_multiple.c \
	$(SRC_DIR)/directory_iterator_mysql.c \
	$(SRC_DIR)/directory_iterator_pkzip.c \
	$(SRC_DIR)/directory_iterator_preindex.c \
	$(SRC_DIR)/directory_iterator_preindex_internals.c \
	$(SRC_DIR)/directory_iterator_recursive.c \
	$(SRC_DIR)/directory_iterator_tar.c \
	$(SRC_DIR)/directory_iterator_warc.c \
	$(SRC_DIR)/directory_iterator_warc_gz_recursive.c \
	$(SRC_DIR)/disk.c \
	$(SRC_DIR)/event.c \
	$(SRC_DIR)/file.c \
	$(SRC_DIR)/file_internals.c \
	$(SRC_DIR)/file_memory.c \
	$(SRC_DIR)/focus_article.c \
	$(SRC_DIR)/focus.c \
	$(SRC_DIR)/focus_lowest_tag.c \
	$(SRC_DIR)/focus_results_list.c \
	$(SRC_DIR)/hash_table.c \
	$(SRC_DIR)/index_document.c \
	$(SRC_DIR)/index_document_topsig.c \
	$(SRC_DIR)/index_document_topsig_signature.c \
	$(SRC_DIR)/instream_buffer.c \
	$(SRC_DIR)/instream_bz2.c \
	$(SRC_DIR)/instream_deflate.c \
	$(SRC_DIR)/instream_file.c \
	$(SRC_DIR)/instream_lzo.c \
	$(SRC_DIR)/maths.c \
	$(SRC_DIR)/mean_average_precision.c \
	$(SRC_DIR)/memory.c \
	$(SRC_DIR)/memory_index.c \
	$(SRC_DIR)/memory_indexer.c \
	$(SRC_DIR)/memory_index_hash_node.c \
	$(SRC_DIR)/memory_index_one.c \
	$(SRC_DIR)/mersenne_twister.c \
	$(SRC_DIR)/NEXI_ant.c \
	$(SRC_DIR)/nexi.c \
	$(SRC_DIR)/NEXI_term_ant.c \
	$(SRC_DIR)/NEXI_term.c \
	$(SRC_DIR)/NEXI_term_iterator.c \
	$(SRC_DIR)/numbers.c \
	$(SRC_DIR)/parser.c \
	$(SRC_DIR)/parser_readability.c \
	$(SRC_DIR)/plugin_manager.c \
	$(SRC_DIR)/postings_piece.c \
	$(SRC_DIR)/pregen.c \
	$(SRC_DIR)/pregen_kendall_tau.c \
	$(SRC_DIR)/pregens_writer.c \
	$(SRC_DIR)/pregen_writer.c \
	$(SRC_DIR)/pregen_writer_exact_integers.c \
	$(SRC_DIR)/pregen_writer_exact_strings.c \
	$(SRC_DIR)/pregen_writer_normal.c \
	$(SRC_DIR)/query_boolean.c \
	$(SRC_DIR)/query.c \
	$(SRC_DIR)/query_parse_tree.c \
	$(SRC_DIR)/ranking_function_bm25.c \
	$(SRC_DIR)/ranking_function_bose_einstein.c \
	$(SRC_DIR)/ranking_function.c \
	$(SRC_DIR)/ranking_function_dfree.c \
	$(SRC_DIR)/ranking_function_divergence.c \
	$(SRC_DIR)/ranking_function_dlh13.c \
	$(SRC_DIR)/ranking_function_docid.c \
	$(SRC_DIR)/ranking_function_dph.c \
	$(SRC_DIR)/ranking_function_impact.c \
	$(SRC_DIR)/ranking_function_inner_product.c \
	$(SRC_DIR)/ranking_function_kbtfidf.c \
	$(SRC_DIR)/ranking_function_lmd.c \
	$(SRC_DIR)/ranking_function_lmjm.c \
	$(SRC_DIR)/ranking_function_pregen.c \
	$(SRC_DIR)/ranking_function_readability.c \
	$(SRC_DIR)/ranking_function_term_count.c \
	$(SRC_DIR)/ranking_function_topsig_negative.c \
	$(SRC_DIR)/ranking_function_topsig_positive.c \
	$(SRC_DIR)/readability_dale_chall.c \
	$(SRC_DIR)/readability_factory.c \
	$(SRC_DIR)/relevance_feedback_blind_kl.c \
	$(SRC_DIR)/relevance_feedback.c \
	$(SRC_DIR)/relevance_feedback_factory.c \
	$(SRC_DIR)/relevance_feedback_topsig.c \
	$(SRC_DIR)/relevant_document.c \
	$(SRC_DIR)/relevant_topic.c \
	$(SRC_DIR)/search_engine_accumulator.c \
	$(SRC_DIR)/search_engine.c \
	$(SRC_DIR)/search_engine_forum.c \
	$(SRC_DIR)/search_engine_forum_INEX_bep.c \
	$(SRC_DIR)/search_engine_forum_INEX.c \
	$(SRC_DIR)/search_engine_forum_INEX_efficiency.c \
	$(SRC_DIR)/search_engine_forum_INEX_focus.c \
	$(SRC_DIR)/search_engine_forum_TREC.c \
	$(SRC_DIR)/search_engine_memory_index.c \
	$(SRC_DIR)/search_engine_readability.c \
	$(SRC_DIR)/search_engine_result.c \
	$(SRC_DIR)/search_engine_result_iterator.c \
	$(SRC_DIR)/semaphores.c \
	$(SRC_DIR)/snippet_beginning.c \
	$(SRC_DIR)/snippet_best_tag.c \
	$(SRC_DIR)/snippet.c \
	$(SRC_DIR)/snippet_factory.c \
	$(SRC_DIR)/snippet_tag.c \
	$(SRC_DIR)/snippet_tficf.c \
	$(SRC_DIR)/snippet_word_cloud.c \
	$(SRC_DIR)/sockets.c \
	$(SRC_DIR)/stats.c \
	$(SRC_DIR)/stats_memory_index.c \
	$(SRC_DIR)/stats_search_engine.c \
	$(SRC_DIR)/stats_time.c \
	$(SRC_DIR)/stem_krovetz.c \
	$(SRC_DIR)/stemmer.c \
	$(SRC_DIR)/stemmer_factory.c \
	$(SRC_DIR)/stemmer_term_similarity.c \
	$(SRC_DIR)/stemmer_term_similarity_threshold.c \
	$(SRC_DIR)/stemmer_term_similarity_weighted.c \
	$(SRC_DIR)/stem_otago.c \
	$(SRC_DIR)/stem_otago_v2.c \
	$(SRC_DIR)/stem_paice_husk.c \
	$(SRC_DIR)/stem_porter.c \
	$(SRC_DIR)/stem_s.c \
	$(SRC_DIR)/stem_snowball.c \
	$(SRC_DIR)/stop_word.c \
	$(SRC_DIR)/str.c \
	$(SRC_DIR)/term_divergence_kl.c \
	$(SRC_DIR)/thesaurus.c \
	$(SRC_DIR)/thesaurus_relationship.c \
	$(SRC_DIR)/thesaurus_wordnet.c \
	$(SRC_DIR)/threads.c \
	$(SRC_DIR)/unicode.c \
	$(SRC_DIR)/unicode_tables.c \
	$(SRC_DIR)/version.c 

#API_SOURCES = $(ATIRE_DIR)/atire_api.c \
#				$(ATIRE_DIR)/atire_api_remote.c 

OTHER_SOURCES := glob.c

LOCAL_MODULE    := atire_android

LOCAL_SRC_FILES := $(OTHER_SOURCES) \
				$(CORE_SOURCES) 
#				$(API_SOURCES)
				
LOCAL_LDLIBS    := 

LOCAL_CFLAGS    += -DONE_PARSER -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
		-DSPECIAL_COMPRESSION=1 -DTOP_K_READ_AND_DECOMPRESSOR -DANT_WITHOUT_STL \
		-I ./include
		
#LOCAL_C_INCLUDES := ./include $(SRC_DIR)

	
include $(BUILD_STATIC_LIBRARY)

# the api lib, which will depend on and include the core one
#

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .c


API_SOURCES = $(ATIRE_DIR)/atire_api.c \
			$(ATIRE_DIR)/atire_api_remote.c
			
INDEX_SOURCES =	$(ATIRE_DIR)/index.c \
			 $(ATIRE_DIR)/indexer_param_block.c \
			 $(ATIRE_DIR)/indexer_param_block_rank.c \
			 $(ATIRE_DIR)/indexer_param_block_topsig.c \
			 $(ATIRE_DIR)/indexer_param_block_pregen.c \
			 $(ATIRE_DIR)/indexer_param_block_stem.c
			 
JNI_SOURCES = ./atire_wrap.c

LOCAL_MODULE    := atire_android_jni
LOCAL_SRC_FILES := \
			$(JNI_SOURCES) \
			$(API_SOURCES) \
			$(INDEX_SOURCES)
			
LOCAL_STATIC_LIBRARIES := atire_android

LOCAL_CFLAGS    := -DATIRE_LIBRARY -DONE_PARSER -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
		-DSPECIAL_COMPRESSION=1 -DTOP_K_READ_AND_DECOMPRESSOR \
		-I $(SRC_DIR) -I ./include -I $(ATIRE_DIR)
		
LOCAL_C_INCLUDES += $(SRC_DIR)

include $(BUILD_SHARED_LIBRARY)
