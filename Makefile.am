#AUTOMAKE_OPTIONS = foreign
#SUBDIRS = plugins

#ACLOCAL_AMFLAGS = -I m4

###############################################################################
# source files and compile commands
###############################################################################
ATIRE_DIR = atire
SRC_DIR = source
OBJ_DIR = obj
BIN_DIR = bin
PHP_DIR = php_ext
LIB_DIR = lib
JNI_DIR = jni
TOOLS_DIR = tools
TESTS_DIR = tests

###############################################################################
# ldd -d xxxx.so might produce undefined symbol: __cxa_pure_virtual errors.
# so add linking options as follows
###############################################################################
AM_LDFLAGS = -Wl,--gc-sections -fPIC -shared -lstdc++ @LIBS@

# using HEADER HASH FUNCTION
AM_CFLAGS = -g -Wall -DONE_PARSER -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
		-DSPECIAL_COMPRESSION=1 -DTOP_K_READ_AND_DECOMPRESSOR \
       -Wno-endif-labels -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings \
        -Wno-long-long
       
# for parallel indexing, but it will be configured by configure script with --with-pi option
# -DPARALLEL_INDEXING
       
       
# using RANDOM HASH FUNCTION       
#AM_CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DRANDOM_HASHER=1 \
#       -Wno-endif-labels -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings     

CC = g++

#AM_CFLAGS += -std=c++0x -pedantic 

AM_CFLAGS += -I $(srcdir)/$(SRC_DIR)

#OBJDIR = 
#BINDIR = 

core_sources =  \
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
	$(SRC_DIR)/directory_iterator_tsv.c \
	$(SRC_DIR)/directory_iterator_warc.c \
	$(SRC_DIR)/directory_iterator_warc_gz_recursive.c \
	$(SRC_DIR)/directory_iterator_scrub.c \
	$(SRC_DIR)/directory_iterator_filter.c \
	$(SRC_DIR)/directory_iterator_filter_spam.c \
	$(SRC_DIR)/directory_iterator_mime_filter.c \
	$(SRC_DIR)/disk.c \
	$(SRC_DIR)/event.c \
	$(SRC_DIR)/evaluation_binary_preference.c \
	$(SRC_DIR)/evaluation.c \
	$(SRC_DIR)/evaluation_discounted_cumulative_gain.c \
	$(SRC_DIR)/evaluation_expected_reciprocal_rank.c \
	$(SRC_DIR)/evaluation_intent_aware_expected_reciprocal_rank.c \
	$(SRC_DIR)/evaluation_intent_aware_mean_average_precision.c \
	$(SRC_DIR)/evaluation_intent_aware_normalised_discounted_cumulative_gain.c \
	$(SRC_DIR)/evaluation_intent_aware_precision_at_n.c \
	$(SRC_DIR)/evaluation_mean_average_generalised_precision_document.c \
	$(SRC_DIR)/evaluation_mean_average_precision.c \
	$(SRC_DIR)/evaluation_normalised_discounted_cumulative_gain.c \
	$(SRC_DIR)/evaluation_precision_at_n.c \
	$(SRC_DIR)/evaluation_rank_effectiveness.c \
	$(SRC_DIR)/evaluation_success_at_n.c \
	$(SRC_DIR)/evaluator.c \
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
	$(SRC_DIR)/instream_scrub.c \
	$(SRC_DIR)/maths.c \
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
	$(SRC_DIR)/ranking_function_dfi_idf.c \
	$(SRC_DIR)/ranking_function_dfi.c \
	$(SRC_DIR)/ranking_function_dfiw_idf.c \
	$(SRC_DIR)/ranking_function_dfiw.c \
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
	$(SRC_DIR)/ranking_function_lmds.c \
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
	$(SRC_DIR)/relevant_subtopic.c \
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
				
core_noinst_headers = \
	$(SRC_DIR)/arithmetic_coding_decoder.h \
	$(SRC_DIR)/arithmetic_coding_encoder.h \
	$(SRC_DIR)/arithmetic_coding.h \
	$(SRC_DIR)/arithmetic_model_bigram.h \
	$(SRC_DIR)/arithmetic_model.h \
	$(SRC_DIR)/arithmetic_model_tables.h \
	$(SRC_DIR)/arithmetic_model_unigram.h \
	$(SRC_DIR)/assessment_ANT.h \
	$(SRC_DIR)/assessment_factory.h \
	$(SRC_DIR)/assessment.h \
	$(SRC_DIR)/assessment_INEX.h \
	$(SRC_DIR)/assessment_TREC.h \
	$(SRC_DIR)/barrier.h \
	$(SRC_DIR)/bitstream.h \
	$(SRC_DIR)/bitstring.h \
	$(SRC_DIR)/bitstring_iterator.h \
	$(SRC_DIR)/btree.h \
	$(SRC_DIR)/btree_head_node.h \
	$(SRC_DIR)/btree_iterator.h \
	$(SRC_DIR)/channel_file.h \
	$(SRC_DIR)/channel.h \
	$(SRC_DIR)/channel_socket.h \
	$(SRC_DIR)/compress_carryover12.h \
	$(SRC_DIR)/compress_carryover12_internals.h \
	$(SRC_DIR)/compress_elias_delta.h \
	$(SRC_DIR)/compress_elias_gamma.h \
	$(SRC_DIR)/compress_golomb.h \
	$(SRC_DIR)/compress.h \
	$(SRC_DIR)/compression_factory.h \
	$(SRC_DIR)/compression_factory_scheme.h \
	$(SRC_DIR)/compression_text_factory.h \
	$(SRC_DIR)/compression_text_factory_scheme.h \
	$(SRC_DIR)/compress_none.h \
	$(SRC_DIR)/compress_relative10.h \
	$(SRC_DIR)/compress_sigma_frequency.h \
	$(SRC_DIR)/compress_sigma.h \
	$(SRC_DIR)/compress_simple9.h \
	$(SRC_DIR)/compress_text_bz2.h \
	$(SRC_DIR)/compress_text_deflate.h \
	$(SRC_DIR)/compress_text_deflate_internals.h \
	$(SRC_DIR)/compress_text.h \
	$(SRC_DIR)/compress_text_none.h \
	$(SRC_DIR)/compress_text_snappy.h \
	$(SRC_DIR)/compress_variable_byte.h \
	$(SRC_DIR)/critical_section.h \
	$(SRC_DIR)/critical_section_internals.h \
	$(SRC_DIR)/ctypes.h \
	$(SRC_DIR)/directory_iterator_compressor.h \
	$(SRC_DIR)/directory_iterator_csv.h \
	$(SRC_DIR)/directory_iterator_deflate.h \
	$(SRC_DIR)/directory_iterator_file.h \
	$(SRC_DIR)/directory_iterator_file_buffered.h \
	$(SRC_DIR)/directory_iterator.h \
	$(SRC_DIR)/directory_iterator_internals.h \
	$(SRC_DIR)/directory_iterator_multiple.h \
	$(SRC_DIR)/directory_iterator_multiple_internals.h \
	$(SRC_DIR)/directory_iterator_mysql.h \
	$(SRC_DIR)/directory_iterator_object.h \
	$(SRC_DIR)/directory_iterator_pkzip.h \
	$(SRC_DIR)/directory_iterator_pkzip_internals.h \
	$(SRC_DIR)/directory_iterator_preindex.h \
	$(SRC_DIR)/directory_iterator_preindex_internals.h \
	$(SRC_DIR)/directory_iterator_recursive.h \
	$(SRC_DIR)/directory_iterator_tar_file_header.h \
	$(SRC_DIR)/directory_iterator_tar.h \
	$(SRC_DIR)/directory_iterator_tsv.h \
	$(SRC_DIR)/directory_iterator_warc_gz_recursive.h \
	$(SRC_DIR)/directory_iterator_warc.h \
	$(SRC_DIR)/directory_iterator_scrub.h \
	$(SRC_DIR)/directory_iterator_filter.h \
	$(SRC_DIR)/directory_iterator_filter_spam.h \
	$(SRC_DIR)/directory_iterator_mime_filter.h \
	$(SRC_DIR)/disk_directory.h \
	$(SRC_DIR)/disk.h \
	$(SRC_DIR)/encode_char_8bit.h \
	$(SRC_DIR)/encode_char_base32_edges.h \
	$(SRC_DIR)/encode_char_base32.h \
	$(SRC_DIR)/encode_char_base36.h \
	$(SRC_DIR)/encode_char_base37.h \
	$(SRC_DIR)/encode_char_base40.h \
	$(SRC_DIR)/encode_char.h \
	$(SRC_DIR)/encode_char_printable_ascii.h \
	$(SRC_DIR)/event.h \
	$(SRC_DIR)/event_internals.h \
	$(SRC_DIR)/evaluation_binary_preference.h \
	$(SRC_DIR)/evaluation_discounted_cumulative_gain.h \
	$(SRC_DIR)/evaluation_expected_reciprocal_rank.h \
	$(SRC_DIR)/evaluation.h \
	$(SRC_DIR)/evaluation_intent_aware_expected_reciprocal_rank.h \
	$(SRC_DIR)/evaluation_intent_aware_mean_average_precision.h \
	$(SRC_DIR)/evaluation_intent_aware_normalised_discounted_cumulative_gain.h \
	$(SRC_DIR)/evaluation_intent_aware_precision_at_n.h \
	$(SRC_DIR)/evaluation_mean_average_generalised_precision_document.h \
	$(SRC_DIR)/evaluation_mean_average_precision.h \
	$(SRC_DIR)/evaluation_normalised_discounted_cumulative_gain.h \
	$(SRC_DIR)/evaluation_precision_at_n.h \
	$(SRC_DIR)/evaluation_rank_effectiveness.h \
	$(SRC_DIR)/evaluation_success_at_n.h \
	$(SRC_DIR)/evaluator.h \
	$(SRC_DIR)/fence.h \
	$(SRC_DIR)/file.h \
	$(SRC_DIR)/file_internals.h \
	$(SRC_DIR)/file_memory.h \
	$(SRC_DIR)/focus_article.h \
	$(SRC_DIR)/focus.h \
	$(SRC_DIR)/focus_hash_node.h \
	$(SRC_DIR)/focus_lowest_tag.h \
	$(SRC_DIR)/focus_result.h \
	$(SRC_DIR)/focus_results_list.h \
	$(SRC_DIR)/fundamental_types.h \
	$(SRC_DIR)/hash_table.h \
	$(SRC_DIR)/heap.h \
	$(SRC_DIR)/impact_header.h \
	$(SRC_DIR)/index_document_global_stats.h \
	$(SRC_DIR)/index_document.h \
	$(SRC_DIR)/index_document_topsig.h \
	$(SRC_DIR)/index_document_topsig_signature.h \
	$(SRC_DIR)/instream_buffer.h \
	$(SRC_DIR)/instream_bz2.h \
	$(SRC_DIR)/instream_bz2_internals.h \
	$(SRC_DIR)/instream_deflate.h \
	$(SRC_DIR)/instream_deflate_internals.h \
	$(SRC_DIR)/instream_file.h \
	$(SRC_DIR)/instream.h \
	$(SRC_DIR)/instream_lzo.h \
	$(SRC_DIR)/instream_scrub.h \
	$(SRC_DIR)/maths.h \
	$(SRC_DIR)/memory.h \
	$(SRC_DIR)/memory_indexer.h \
	$(SRC_DIR)/memory_indexer_node.h \
	$(SRC_DIR)/memory_index.h \
	$(SRC_DIR)/memory_index_hash_node.h \
	$(SRC_DIR)/memory_index_one.h \
	$(SRC_DIR)/memory_index_one_node.h \
	$(SRC_DIR)/mersenne_twister.h \
	$(SRC_DIR)/NEXI_ant.h \
	$(SRC_DIR)/NEXI.h \
	$(SRC_DIR)/NEXI_term_ant.h \
	$(SRC_DIR)/NEXI_term.h \
	$(SRC_DIR)/NEXI_term_iterator.h \
	$(SRC_DIR)/numbers.h \
	$(SRC_DIR)/parser.h \
	$(SRC_DIR)/parser_readability.h \
	$(SRC_DIR)/parser_token.h \
	$(SRC_DIR)/pdebug.h \
	$(SRC_DIR)/phonetic_double_metaphone.h \
	$(SRC_DIR)/phonetic_double_metaphone_internals.h \
	$(SRC_DIR)/phonetic_iterator_double_metaphone.h \
	$(SRC_DIR)/phonetic_iterator_soundex.h \
	$(SRC_DIR)/phonetic_soundex.h \
	$(SRC_DIR)/plugin.h \
	$(SRC_DIR)/plugin_manager.h \
	$(SRC_DIR)/postings_piece.h \
	$(SRC_DIR)/pragma.h \
	$(SRC_DIR)/precision_recall.h \
	$(SRC_DIR)/pregen_field_type.h \
	$(SRC_DIR)/pregen_file_header.h \
	$(SRC_DIR)/pregen.h \
	$(SRC_DIR)/pregen_kendall_tau.h \
	$(SRC_DIR)/pregens_writer.h \
	$(SRC_DIR)/pregen_t.h \
	$(SRC_DIR)/pregen_writer_exact_integers.h \
	$(SRC_DIR)/pregen_writer_exact_strings.h \
	$(SRC_DIR)/pregen_writer.h \
	$(SRC_DIR)/pregen_writer_normal.h \
	$(SRC_DIR)/primary_cmp.h \
	$(SRC_DIR)/producer_consumer.h \
	$(SRC_DIR)/quantum.h \
	$(SRC_DIR)/query_boolean.h \
	$(SRC_DIR)/query.h \
	$(SRC_DIR)/query_parse_tree.h \
	$(SRC_DIR)/ranking_function_bm25.h \
	$(SRC_DIR)/ranking_function_bose_einstein.h \
	$(SRC_DIR)/ranking_function_dfi_idf.h \
	$(SRC_DIR)/ranking_function_dfi.h \
	$(SRC_DIR)/ranking_function_dfiw_idf.h \
	$(SRC_DIR)/ranking_function_dfiw.h \
	$(SRC_DIR)/ranking_function_dfree.h \
	$(SRC_DIR)/ranking_function_divergence.h \
	$(SRC_DIR)/ranking_function_dlh13.h \
	$(SRC_DIR)/ranking_function_docid.h \
	$(SRC_DIR)/ranking_function_dph.h \
	$(SRC_DIR)/ranking_function_factory.h \
	$(SRC_DIR)/ranking_function.h \
	$(SRC_DIR)/ranking_function_impact.h \
	$(SRC_DIR)/ranking_function_inner_product.h \
	$(SRC_DIR)/ranking_function_kbtfidf.h \
	$(SRC_DIR)/ranking_function_lmd.h \
	$(SRC_DIR)/ranking_function_lmds.h \
	$(SRC_DIR)/ranking_function_lmjm.h \
	$(SRC_DIR)/ranking_function_pregen.h \
	$(SRC_DIR)/ranking_function_quantum_parameters.h \
	$(SRC_DIR)/ranking_function_readability.h \
	$(SRC_DIR)/ranking_function_term_count.h \
	$(SRC_DIR)/ranking_function_topsig.h \
	$(SRC_DIR)/ranking_function_topsig_negative.h \
	$(SRC_DIR)/ranking_function_topsig_positive.h \
	$(SRC_DIR)/readability_dale_chall.h \
	$(SRC_DIR)/readability_dale_chall_wordlist.h \
	$(SRC_DIR)/readability_factory.h \
	$(SRC_DIR)/readability.h \
	$(SRC_DIR)/readability_none.h \
	$(SRC_DIR)/relevance_feedback_blind_kl.h \
	$(SRC_DIR)/relevance_feedback_factory.h \
	$(SRC_DIR)/relevance_feedback.h \
	$(SRC_DIR)/relevance_feedback_topsig.h \
	$(SRC_DIR)/relevant_document.h \
	$(SRC_DIR)/relevant_document_passage.h \
	$(SRC_DIR)/relevant_subtopic.h \
	$(SRC_DIR)/relevant_topic.h \
	$(SRC_DIR)/search_engine_accumulator.h \
	$(SRC_DIR)/search_engine_btree_leaf.h \
	$(SRC_DIR)/search_engine_btree_node.h \
	$(SRC_DIR)/search_engine_forum.h \
	$(SRC_DIR)/search_engine_forum_INEX_bep.h \
	$(SRC_DIR)/search_engine_forum_INEX_efficiency.h \
	$(SRC_DIR)/search_engine_forum_INEX_focus.h \
	$(SRC_DIR)/search_engine_forum_INEX.h \
	$(SRC_DIR)/search_engine_forum_TREC.h \
	$(SRC_DIR)/search_engine.h \
	$(SRC_DIR)/search_engine_init_flags_boolean.h \
	$(SRC_DIR)/search_engine_init_flags.h \
	$(SRC_DIR)/search_engine_memory_index.h \
	$(SRC_DIR)/search_engine_readability.h \
	$(SRC_DIR)/search_engine_result.h \
	$(SRC_DIR)/search_engine_result_iterator.h \
	$(SRC_DIR)/semaphores.h \
	$(SRC_DIR)/semaphores_internals.h \
	$(SRC_DIR)/snippet_beginning.h \
	$(SRC_DIR)/snippet_best_tag.h \
	$(SRC_DIR)/snippet_best_tag_tf.h \
	$(SRC_DIR)/snippet_factory.h \
	$(SRC_DIR)/snippet.h \
	$(SRC_DIR)/snippet_keyword.h \
	$(SRC_DIR)/snippet_tag.h \
	$(SRC_DIR)/snippet_tf.h \
	$(SRC_DIR)/snippet_tficf.h \
	$(SRC_DIR)/snippet_word_cloud.h \
	$(SRC_DIR)/sockets.h \
	$(SRC_DIR)/sockets_internals.h \
	$(SRC_DIR)/stats.h \
	$(SRC_DIR)/stats_memory_index.h \
	$(SRC_DIR)/stats_search_engine.h \
	$(SRC_DIR)/stats_time.h \
	$(SRC_DIR)/stem.h \
	$(SRC_DIR)/stem_krovetz.h \
	$(SRC_DIR)/stemmer_factory.h \
	$(SRC_DIR)/stemmer.h \
	$(SRC_DIR)/stemmer_krovetz.h \
	$(SRC_DIR)/stemmer_none.h \
	$(SRC_DIR)/stemmer_otago.h \
	$(SRC_DIR)/stemmer_otago_v2.h \
	$(SRC_DIR)/stemmer_paice_husk.h \
	$(SRC_DIR)/stemmer_porter.h \
	$(SRC_DIR)/stemmer_snowball.h \
	$(SRC_DIR)/stemmer_stem.h \
	$(SRC_DIR)/stemmer_term_similarity.h \
	$(SRC_DIR)/stemmer_term_similarity_threshold.h \
	$(SRC_DIR)/stemmer_term_similarity_weighted.h \
	$(SRC_DIR)/stem_none.h \
	$(SRC_DIR)/stem_otago.h \
	$(SRC_DIR)/stem_otago_v2.h \
	$(SRC_DIR)/stem_paice_husk.h \
	$(SRC_DIR)/stem_porter.h \
	$(SRC_DIR)/stem_s.h \
	$(SRC_DIR)/stem_snowball.h \
	$(SRC_DIR)/stop_word.h \
	$(SRC_DIR)/str.h \
	$(SRC_DIR)/string_pair_constant.h \
	$(SRC_DIR)/string_pair.h \
	$(SRC_DIR)/term_divergence.h \
	$(SRC_DIR)/term_divergence_kl.h \
	$(SRC_DIR)/thesaurus.h \
	$(SRC_DIR)/thesaurus_relationship.h \
	$(SRC_DIR)/thesaurus_rootnode.h \
	$(SRC_DIR)/thesaurus_wordnet.h \
	$(SRC_DIR)/threads.h \
	$(SRC_DIR)/unicode.h \
	$(SRC_DIR)/unicode_pair.h \
	$(SRC_DIR)/unicode_tables.h \
	$(SRC_DIR)/version.h 
	
IGNORE_LIST = $(SRCDIR)/ant_ant.c \
			  $(SRCDIR)/ant_api.c \
			  $(SRCDIR)/ant_plugins.c \
			  $(SRCDIR)/ant_dictionary.c \
			  $(SRCDIR)/test_compression.c
			  
main_sources = $(ATIRE_DIR)/atire.c \
			  $(ATIRE_DIR)/index.c \
			  $(ATIRE_DIR)/atire_client.c \
			  $(ATIRE_DIR)/atire_broker.c \
			  $(ATIRE_DIR)/atire_dictionary.c \
			  $(ATIRE_DIR)/get_doclist.c
			  
main_headers = $(ATIRE_DIR)/atire_api_remote.h \
			$(ATIRE_DIR)/atire_broke_engine.h \
			$(ATIRE_DIR)/atire_broke.h \
			$(ATIRE_DIR)/atire_broker_param_block.h \
			$(ATIRE_DIR)/atire_client_param_block.h \
			$(ATIRE_DIR)/atire_engine.h \
			$(ATIRE_DIR)/atire_engine_param_block.h \
			$(ATIRE_DIR)/atire_engine_result.h \
			$(ATIRE_DIR)/atire_engine_result_set_export.h \
			$(ATIRE_DIR)/atire_engine_result_set_export_INEX_snippet.h \
			$(ATIRE_DIR)/atire_engine_result_set_export_TREC.h \
			$(ATIRE_DIR)/atire_engine_result_set.h \
			$(ATIRE_DIR)/indexer_param_block.h \
			$(ATIRE_DIR)/indexer_param_block_pregen.h \
			$(ATIRE_DIR)/indexer_param_block_rank.h \
			$(ATIRE_DIR)/indexer_param_block_stem.h \
			$(ATIRE_DIR)/indexer_param_block_topsig.h			  			  
			  
#ATIRE_PARAM_LIST = $(shell ls $(SRCIDR)/ant_param_*.c)
#INDEX_PARAM_LIST = $(shell ls $(SRCDIR)/indexer_param_*.c)		  
#
#all_sources = $(shell ls $(SRCDIR)/*.c)
#core_noinst_headers = $(shell ls $(SRCDIR)/*.h)
#core_sources = $(filter-out $(IGNORE_LIST) $(ATIRE_PARAM_LIST) $(INDEX_PARAM_LIST), $(all_sources))
	
ATIRE_PARAM_LIST = $(ATIRE_DIR)/ant_param_block.c

INDEX_PARAM_LIST = $(ATIRE_DIR)/indexer_param_block.c \
				 $(ATIRE_DIR)/indexer_param_block_rank.c \
				 $(ATIRE_DIR)/indexer_param_block_topsig.c \
				 $(ATIRE_DIR)/indexer_param_block_pregen.c \
				 $(ATIRE_DIR)/indexer_param_block_stem.c
	
library_headers = \
	$(SRC_DIR)/heap.h \
	$(SRC_DIR)/max_quantum.h \
	$(SRC_DIR)/primary_cmp.h \
	$(SRC_DIR)/impact_header.h \
	$(SRC_DIR)/fundamental_types.h \
	$(SRC_DIR)/compress.h \
	$(ATIRE_DIR)/atire_api.h \
	$(ATIRE_DIR)/atire_api_remote.h \
	$(ATIRE_DIR)/atire_engine_result_set.h \
	$(ATIRE_DIR)/atire_engine_result_set_export_INEX_snippet.h \
	$(ATIRE_DIR)/atire_engine_result_set_export_TREC.h
	
ATIRE_API_LIB = libatire_api.la

ATIRE_CORE_LIB = libatire_core.la
ATIRE_INDEX_PARAM_LIB = libatire_index_param.la
ATIRE_ANT_PARAM_LIB = libatire_ant_param.la

ATIRE_LIBS = $(ATIRE_CORE_LIB)  $(ATIRE_INDEX_PARAM_LIB) $(ATIRE_ANT_PARAM_LIB)
	
includedir = $(prefix)/include
include_HEADERS = $(library_headers)
		
noinst_LTLIBRARIES = libatire_index.la 
lib_LTLIBRARIES = $(ATIRE_CORE_LIB) $(ATIRE_INDEX_PARAM_LIB) $(ATIRE_ANT_PARAM_LIB) $(ATIRE_API_LIB) 

libatire_core_la_SOURCES = $(core_sources)
libatire_core_la_LIBADD = -ldl -lpthread
libatire_core_la_LIBFLAGS = 

libatire_api_la_SOURCES = $(ATIRE_DIR)/atire_api.c \
				$(ATIRE_DIR)/atire_api_remote.c \
				$(ATIRE_DIR)/atire_engine_result_set.c \
				$(ATIRE_DIR)/atire_engine_result_set_export_INEX_snippet.c \
				$(ATIRE_DIR)/atire_engine_result_set_export_TREC.c
				
libatire_api_la_DEPENDENCIES = $(ATIRE_ANT_PARAM_LIB)
libatire_api_la_LIBADD = $(ATIRE_ANT_PARAM_LIB)
#libatire_api_la_LIBFLAGS = -static
#libatire_api_ladir = $(prefix)/lib
#libatire_api_la_HEADERS = $(library_headers)
#$(library_noinst_headers)

libatire_index_param_la_SOURCES = \
	$(INDEX_PARAM_LIST)
libatire_index_param_la_LDFLAGS = 
libatire_index_param_la_DEPENDENCIES = $(ATIRE_CORE_LIB)
libatire_index_param_la_LIBADD = $(ATIRE_CORE_LIB)

libatire_index_la_SOURCES = \
	$(ATIRE_DIR)/index.c 
libatire_index_la_LDFLAGS = 
libatire_index_la_CFLAGS = $(AM_CFLAGS) -DATIRE_LIBRARY
libatire_index_la_DEPENDENCIES = $(ATIRE_INDEX_PARAM_LIB)
libatire_index_la_LIBADD = $(ATIRE_INDEX_PARAM_LIB)

libatire_ant_param_la_SOURCES = \
	$(ATIRE_PARAM_LIST)
libatire_ant_param_la_LDFLAGS =
libatire_ant_param_la_DEPENDENCIES = $(ATIRE_INDEX_PARAM_LIB)
libatire_ant_param_la_LIBADD = $(ATIRE_INDEX_PARAM_LIB)

#ATIRE_API_LIB = -L. -latire_core -ldl -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = index atire atire_client atire_broker atire_dictionary get_doclist

atire_SOURCES = \
	$(ATIRE_DIR)/atire.c 
#	$(ATIRE_DIR)/indexer_param_block_rank.c \
#	$(ATIRE_DIR)/ant_param_block.c
atire_LDADD = $(ATIRE_LIBS) $(ATIRE_API_LIB)
#atire_LDFLAGS = -rdynamic
atire_LDFLAGS = -static -export-dynamic
atire_DEPENDENCIES = $(noinst_LTLIBRARIES) $(lib_LTLIBRARIES)

#atire_atire_SOURCES = \
#	$(ATIRE_DIR)/atire_ant.c \
#	$(ATIRE_PARAM_LIST) \
#	$(INDEX_PARAM_LIST)
#	$(ATIRE_DIR)/indexer_param_block_rank.c \
#	$(ATIRE_DIR)/atire_param_block.c
#atire_atire_LDADD = $(ATIRE_API_LIB)
#atire_atire_LDFLAGS = -rdynamic
#atire_atire_LDFLAGS = -static -export-dynamic
#atire_atire_DEPENDENCIES = $(ATIRE_API_LIB)

atire_clientdir=$(ATIRE_DIR)
atire_client_SOURCES = \
	$(ATIRE_DIR)/atire_client.c \
	$(ATIRE_DIR)/atire_client_param_block.c
#atire_client_HEADERS = atire_client_param_block.h
atire_client_LDADD = $(ATIRE_API_LIB)
#atire_client_LDFLAGS = -rdynamic
atire_client_LDFLAGS = -static -export-dynamic
atire_client_DEPENDENCIES = $(ATIRE_API_LIB)

atire_brokerdir=$(ATIRE_DIR)
atire_broker_SOURCES = \
	$(ATIRE_DIR)/atire_broke_engine.c \
	$(ATIRE_DIR)/atire_broke.c \
	$(ATIRE_DIR)/atire_broker.c \
	$(ATIRE_DIR)/atire_broker_param_block.c
#atire_broker_HEADERS = $(ATIRE_DIR)/atire_broker_param_block.h \
	$(ATIRE_DIR)/atire_broke_engine.h \
	$(ATIRE_DIR)/atire_broke.h
atire_broker_LDADD = $(ATIRE_API_LIB)
#atire_broker_LDFLAGS = -rdynamic
atire_broker_LDFLAGS = -static -export-dynamic
atire_broker_DEPENDENCIES = $(ATIRE_API_LIB)

index_SOURCES = \
	$(ATIRE_DIR)/index.c 
index_LDADD = $(ATIRE_CORE_LIB) $(ATIRE_INDEX_PARAM_LIB)
#index_LDFLAGS = -rdynamic
index_LDFLAGS = -static -export-dynamic
index_DEPENDENCIES = $(ATIRE_CORE_LIB) $(ATIRE_INDEX_PARAM_LIB)
index_CFLAGS = $(AM_CFLAGS)

atire_dictionary_SOURCES = $(ATIRE_DIR)/atire_dictionary.c 
atire_dictionary_LDADD = $(ATIRE_CORE_LIB)
atire_dictionary_DEPENDENCIES = $(ATIRE_CORE_LIB)

get_doclist_SOURCES = $(ATIRE_DIR)/get_doclist.c 
get_doclist_LDADD = $(ATIRE_CORE_LIB)
get_doclist_DEPENDENCIES = $(ATIRE_CORE_LIB)

##############################################################################
#  use this if it is really necessary to have same source files with $(ATIRE_API_LIB)
#  otherwise, it will cause automake problem:
#  object `XXXX.$(OBJEXT)' created both with libtool and without
# atire_dictionary_CFLAGS = $(AM_CFLAGS)
##############################################################################
						 
#atire_plugins_SOURCES = $(ATIRE_DIR)/plugins.c \
					$(ATIRE_DIR)/plugin_manager.c
#atire_plugins_CFLAGS = $(AM_CFLAGS)
#atire_plugins_LDADD = $(ATIRE_API_LIB)
#atire_plugins_LDFLAGS = -rdynamic
#atire_plugins_DEPENDENCIES = 
	

#################################################################################
#                               TOOLS
#################################################################################
#filelist_SOURCES = $(TOOLDIR)/filelist.c
#filelist_LDFLAGS = -rdynamic
#filelist_LDADD = $(ATIRE_API_LIB)
#filelist_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#topic_tree_cas_SOURCES = $(TOOLDIR)/topic_tree_cas.c
#topic_tree_cas_LDFLAGS = -rdynamic
#topic_tree_cas_LDADD = $(ATIRE_API_LIB)
#topic_tree_cas_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#bindiff_SOURCES = $(TOOLDIR)/bindiff.c
#bindiff_LDFLAGS = -rdynamic
#bindiff_LDADD = $(ATIRE_API_LIB)
#bindiff_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#term_frequencies_SOURCES = $(TOOLDIR)/term_frequencies.c
#term_frequencies_LDFLAGS = -rdynamic
#term_frequencies_LDADD = $(ATIRE_API_LIB)
#term_frequencies_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#topic_tree_SOURCES = $(TOOLDIR)/topic_tree.c
#topic_tree_LDFLAGS = -rdynamic
#topic_tree_LDADD = $(ATIRE_API_LIB)
#topic_tree_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#INEXqrels_to_run_SOURCES = $(TOOLDIR)/INEXqrels_to_run.c
#INEXqrels_to_run_LDFLAGS = -rdynamic
#INEXqrels_to_run_LDADD = $(ATIRE_API_LIB)
#INEXqrels_to_run_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#foltbl_to_aspt_SOURCES = $(TOOLDIR)/foltbl_to_aspt.c
#foltbl_to_aspt_LDFLAGS = -rdynamic
#foltbl_to_aspt_LDADD = $(ATIRE_API_LIB)
#foltbl_to_aspt_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#zipf_graph_SOURCES = $(TOOLDIR)/zipf_graph.c
#zipf_graph_LDFLAGS = -rdynamic
#zipf_graph_LDADD = $(ATIRE_API_LIB)
#zipf_graph_DEPENDENCIES = $(ATIRE_CORE_LIB)
#
#test_stop_SOURCES = $(TOOLDIR)/test_stop.c \
#				$(SRCDIR)/stop_word.c \
#				$(SRCDIR)/str.c
#test_stop_CFLAGS = $(AM_CFLAGS)
#
