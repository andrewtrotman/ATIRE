#
#	Windows makefile for ATIRE
#	by Andrew Trotman
#

#
#	DEBUG. set DEBUG=1 for compile in DEBUG mode.  Set DEBUG=0 for RELEASE mode.
#
DEBUG = 0

#
#	Which compiler are we using
#
COMPILER=MICROSOFT
#COMPILER=INTEL

#
#	BITNESS.  Are we building on 32 bit or 64 bit windows?
#
!IF [ml64 /nologo /? > nul] == 0
BITNESS = 64
!ELSEIF [ml /nologo /? > nul] == 0
BITNESS = 32
!ELSE
BITNESS = 128
!ENDIF

#
#	Define TRUE and FALSE
#
FALSE = 0
TRUE = 1

#
#	Directories
#
SRCDIR = source
ATIREDIR = atire
OBJDIR = obj
BINDIR = bin
LTWDIR = Link-The-Wiki
TOOLDIR = tools
TESTDIR = tests
LIBDIR = lib

#
#	Declare which external BSD libraries to include
#
ANT_HAS_ZLIB = $(TRUE)
ANT_HAS_BZLIB = $(TRUE)
ANT_HAS_SNAPPYLIB = $(TRUE)
ANT_HAS_SNOWBALL = $(TRUE)
ANT_HAS_PAICE_HUSK = $(TRUE)

#
# Declare which GPL libraries to include
#
ANT_HAS_GPL = $(TRUE)

!IF $(ANT_HAS_GPL) == $(TRUE)
ANT_HAS_MYSQL = $(FALSE)
ANT_HAS_LZO = $(TRUE)
!ELSE
ANT_HAS_MYSQL = $(FALSE)
ANT_HAS_LZO = $(FALSE)
!ENDIF

#
#	The locaiton of the SWIG compiler and th PHP libraries
#
SWIG = @external\gpl\swig\swigwin-2.0.4\swig.exe
PHP_DIR = \php-sdk\php53dev\vc9\x86\php5.3-201201260030
PHP_FLAGS = -I$(PHP_DIR) -I$(PHP_DIR)\main -I$(PHP_DIR)\TSRM -I$(PHP_DIR)\Zend -I$(PHP_DIR)\ext -I$(PHP_DIR)\ext\date\lib -DZEND_WIN32 -DPHP_WIN32 -DWIN32 -DZEND_DEBUG=0 -DZTS=1 -Dstrtoll=_strtoi64
!IF $(BITNESS) == 32
PHP_LIBS = $(PHP_DIR)\Release_TS\php5ts.lib
!ELSE
PHP_LIBS = $(PHP_DIR)\x64\Release_TS\php5ts.lib
!ENDIF

#
#	If we have ZLIB (for the GZIP compression scheme)
#
!IF $(ANT_HAS_ZLIB) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_ZLIB
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I external\unencumbered\zlib\zlib-1.2.5
EXTRA_LIBS = $(EXTRA_LIBS) external\unencumbered\zlib\zlib-1.2.5\zlib.lib
!ENDIF

#
# If we have BZLIB (for the BZ2 compression scheme)
#
!IF $(ANT_HAS_BZLIB) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_BZLIB
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I external\unencumbered\bzip\bzip2-1.0.6
EXTRA_LIBS = $(EXTRA_LIBS) external\unencumbered\bzip\bzip2-1.0.6\libbz2.lib
!ENDIF

#
# If we have SNAPPYLIB (Google's snappy compression library)
#
!IF $(ANT_HAS_SNAPPYLIB) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_SNAPPYLIB
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I external\unencumbered\snappy\snappy-1.0.4 -I external\unencumbered\snappy
EXTRA_LIBS = $(EXTRA_LIBS) external\unencumbered\snappy\snappy.lib
!ENDIF

#
# If we have SNOWBAL (Porter's stemming collecton)
#
!IF $(ANT_HAS_SNOWBALL) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_SNOWBALL
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I external\unencumbered\snowball\libstemmer_c\include
EXTRA_LIBS = $(EXTRA_LIBS) external\unencumbered\snowball\libstemmer_c\libstemmer.lib
!ENDIF

#
#	Paice Husk Stemmer
#
!IF $(ANT_HAS_PAICE_HUSK) == $(TRUE)
STEM_PAICE_HUSK = $(OBJDIR)\stem_paice_husk.obj
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_PAICE_HUSK
!ELSE
STEM_PAICE_HUSK = 
!ENDIF


#
#	Now for GPL / BSD license conflicts (i.e. the GLP stuff)
#

#
#	LZO compression
#
!IF $(ANT_HAS_LZO) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_LZO
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I external\gpl\lzo\lzo-2.06\include\lzo
EXTRA_LIBS = $(EXTRA_LIBS) external\gpl\lzo\lzo2.lib
!ENDIF

#
#	If we have MySQL Connector/C (libmysql).  The MySQL Connectors are GPL.
#
!IF $(ANT_HAS_MYSQL) == $(TRUE)
!IF $(BITNESS) == 64
MYSQL_BITNESS = x64
!ELSEIF $(BITNESS) == 32
MYSQL_BITNESS = 32
!ELSE
MYSQL_BITNESS = 128
!ENDIF
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_MYSQL
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I external\gpl\MySQL\mysql-connector-c-noinstall-6.0.2-win$(MYSQL_BITNESS)\include
EXTRA_LIBS = $(EXTRA_LIBS) external\gpl\MySQL\mysql-connector-c-noinstall-6.0.2-win$(MYSQL_BITNESS)\lib\libmysql.lib
DIRECTORY_ITERATOR_MYSQL = $(OBJDIR)\directory_iterator_mysql.obj
!ELSE
DIRECTORY_ITERATOR_MYSQL = 
!ENDIF


#
#	Post configuration, so back to the hard work...
#
FIXED = /link /fixed:no /opt:ref

MINUS_D = $(EXTRA_MINUS_D) -DHASHER=1 -DHEADER_NUM=1 
MINUS_D = $(MINUS_D) -DSPECIAL_COMPRESSION=1
MINUS_D = $(MINUS_D) -DTWO_D_ACCUMULATORS
MINUS_D = $(MINUS_D) -DTOP_K_READ_AND_DECOMPRESSOR
MINUS_D = $(MINUS_D) -DPARALLEL_INDEXING
MINUS_D = $(MINUS_D) -DPARALLEL_INDEXING_DOCUMENTS
MINUS_D = $(MINUS_D) -DANT_ACCUMULATOR_T="double"
MINUS_D = $(MINUS_D) -DANT_PREGEN_T="unsigned long long"
MINUS_D = $(MINUS_D) -DNOMINMAX
MINUS_D = $(MINUS_D) -DIMPACT_HEADER
MINUS_D = $(MINUS_D) -DFILENAME_INDEX

#MINUS_D = $(MINUS_D) -DPURIFY

#
#	Compiler and flags (the top line is debug, the bottom is release)
#

!IF "$(COMPILER)" == "MICROSOFT"
CC = @cl /nologo
#CC = insure -Zvm -Zvm -Zvm -Zvm 
!IF "$(DEBUG)" == "1"
CFLAGS = -D_DEBUG   /MTd /Od /W4 -D_CRT_SECURE_NO_WARNINGS /Zi $(MINUS_D) $(EXTRA_INCLUDE) /GL 
!ELSE
CFLAGS = -D_RELEASE /MT /Ox /W4 -D_CRT_SECURE_NO_WARNINGS /Zi $(MINUS_D) $(EXTRA_INCLUDE) /GL /Gy /fp:fast /MP8 
!ENDIF
!ENDIF

!IF "$(COMPILER)" == "INTEL"
CC = @icl /nologo
!IF "$(DEBUG)" == "1"
CFLAGS = -D_DEBUG   /Od /W4 /Zi $(MINUS_D) $(EXTRA_INCLUDE) /GL 
!ELSE
CFLAGS  = -D_RELEASE /O3 /W4 /Zi $(MINUS_D) $(EXTRA_INCLUDE) /GL /Gy /fp:fast /MP8 /QxHost /Qparallel /Qipo /Qip
!ENDIF
!ENDIF

#
#	Libraries
#
WINDOWS_LIBS = user32.lib advapi32.lib kernel32.lib shlwapi.lib ws2_32.lib

#
#	Objects
#
PARTS = \
	$(OBJDIR)\atire_api.obj 						\
	$(OBJDIR)\atire_api_remote.obj 					\
	$(OBJDIR)\relevance_feedback.obj 				\
	$(OBJDIR)\relevance_feedback_blind_kl.obj 		\
	$(OBJDIR)\relevance_feedback_blind_kl_rm.obj 	\
	$(OBJDIR)\relevance_feedback_topsig.obj 		\
	$(OBJDIR)\relevance_feedback_factory.obj 		\
	$(OBJDIR)\term_divergence_kl.obj 				\
	$(OBJDIR)\query.obj 							\
	$(OBJDIR)\query_parse_tree.obj 					\
	$(OBJDIR)\query_boolean.obj 					\
	$(OBJDIR)\bitstring.obj 						\
	$(OBJDIR)\bitstring_iterator.obj 				\
	$(OBJDIR)\channel_file.obj						\
	$(OBJDIR)\channel_socket.obj					\
	$(OBJDIR)\channel_trec.obj						\
	$(OBJDIR)\channel_inex.obj						\
	$(OBJDIR)\parser.obj 							\
	$(OBJDIR)\parser_readability.obj				\
	$(OBJDIR)\index_document.obj					\
	$(OBJDIR)\index_document_topsig.obj				\
	$(OBJDIR)\index_document_topsig_signature.obj	\
	$(OBJDIR)\memory_index_hash_node.obj			\
	$(OBJDIR)\memory_index_filename_index.obj 		\
	$(OBJDIR)\memory_index.obj 						\
	$(OBJDIR)\memory_indexer.obj 					\
	$(OBJDIR)\memory_index_one.obj 					\
	$(OBJDIR)\stats_memory_index.obj				\
	$(OBJDIR)\mersenne_twister.obj					\
	$(OBJDIR)\postings_piece.obj					\
	$(OBJDIR)\ctypes.obj 							\
	$(OBJDIR)\file.obj								\
	$(OBJDIR)\file_memory.obj						\
	$(OBJDIR)\file_internals.obj					\
	$(OBJDIR)\maths.obj 							\
	$(OBJDIR)\numbers.obj 							\
	$(OBJDIR)\memory.obj 							\
	$(OBJDIR)\search_engine.obj 					\
	$(OBJDIR)\search_engine_accumulator.obj 		\
	$(OBJDIR)\search_engine_result.obj 				\
	$(OBJDIR)\search_engine_result_id_iterator.obj 	\
	$(OBJDIR)\search_engine_result_iterator.obj 	\
	$(OBJDIR)\search_engine_memory_index.obj 		\
	$(OBJDIR)\evaluator.obj							\
	$(OBJDIR)\evaluation.obj						\
	$(OBJDIR)\evaluation_binary_preference.obj    	\
	$(OBJDIR)\evaluation_discounted_cumulative_gain.obj                             \
	$(OBJDIR)\evaluation_expected_reciprocal_rank.obj                               \
	$(OBJDIR)\evaluation_intent_aware_expected_reciprocal_rank.obj                  \
	$(OBJDIR)\evaluation_intent_aware_mean_average_precision.obj                    \
	$(OBJDIR)\evaluation_intent_aware_normalised_discounted_cumulative_gain.obj     \
	$(OBJDIR)\evaluation_intent_aware_precision_at_n.obj                            \
	$(OBJDIR)\evaluation_mean_average_generalised_precision_document.obj            \
	$(OBJDIR)\evaluation_mean_average_precision.obj                                 \
	$(OBJDIR)\evaluation_normalised_discounted_cumulative_gain.obj                  \
	$(OBJDIR)\evaluation_precision_at_n.obj       	\
	$(OBJDIR)\evaluation_rank_effectiveness.obj   	\
	$(OBJDIR)\evaluation_success_at_n.obj         	\
	$(OBJDIR)\assessment.obj						\
	$(OBJDIR)\assessment_TREC.obj					\
	$(OBJDIR)\assessment_INEX.obj					\
	$(OBJDIR)\assessment_factory.obj				\
	$(OBJDIR)\relevant_document.obj 				\
	$(OBJDIR)\relevant_topic.obj					\
	$(OBJDIR)\relevant_subtopic.obj					\
	$(OBJDIR)\stats.obj								\
	$(OBJDIR)\stats_time.obj						\
	$(OBJDIR)\stats_search_engine.obj				\
	$(OBJDIR)\search_engine_forum.obj				\
	$(OBJDIR)\search_engine_forum_INEX.obj			\
	$(OBJDIR)\search_engine_forum_INEX_bep.obj		\
	$(OBJDIR)\search_engine_forum_INEX_focus.obj	\
	$(OBJDIR)\search_engine_forum_INEX_efficiency.obj			\
	$(OBJDIR)\search_engine_forum_TREC.obj			\
	$(OBJDIR)\str.obj 								\
	$(OBJDIR)\stop_word.obj 						\
	$(OBJDIR)\disk.obj 								\
	$(OBJDIR)\directory_iterator.obj 				\
	$(OBJDIR)\directory_iterator_multiple.obj 		\
	$(OBJDIR)\directory_iterator_preindex.obj 		\
	$(OBJDIR)\directory_iterator_preindex_internals.obj 	\
	$(OBJDIR)\directory_iterator_compressor.obj 	\
	$(OBJDIR)\directory_iterator_deflate.obj 		\
	$(OBJDIR)\directory_iterator_internals.obj 		\
	$(OBJDIR)\directory_iterator_tar.obj			\
	$(OBJDIR)\directory_iterator_warc.obj			\
	$(OBJDIR)\directory_iterator_warc_gz_recursive.obj		\
	$(OBJDIR)\directory_iterator_trec_recursive.obj		\
	$(OBJDIR)\directory_iterator_pkzip.obj			\
	$(OBJDIR)\directory_iterator_file.obj			\
	$(OBJDIR)\directory_iterator_file_buffered.obj	\
	$(OBJDIR)\directory_iterator_csv.obj			\
	$(OBJDIR)\directory_iterator_tsv.obj			\
	$(OBJDIR)\directory_iterator_recursive.obj		\
	$(OBJDIR)\directory_iterator_scrub.obj			\
	$(OBJDIR)\directory_iterator_filter.obj			\
	$(OBJDIR)\directory_iterator_filter_spam.obj	\
	$(OBJDIR)\directory_iterator_mime_filter.obj	\
	$(OBJDIR)\hash_random.obj \
	$(OBJDIR)\hash_header.obj \
	$(DIRECTORY_ITERATOR_MYSQL)						\
	$(OBJDIR)\btree_iterator.obj 					\
	$(OBJDIR)\thesaurus.obj							\
	$(OBJDIR)\thesaurus_relationship.obj			\
	$(OBJDIR)\thesaurus_wordnet.obj					\
	$(OBJDIR)\stemmer.obj							\
	$(OBJDIR)\stemmer_term_similarity.obj			\
	$(OBJDIR)\stemmer_term_similarity_threshold.obj	\
	$(OBJDIR)\stemmer_term_similarity_weighted.obj	\
	$(OBJDIR)\stem_porter.obj						\
	$(OBJDIR)\stem_krovetz.obj						\
	$(OBJDIR)\stem_snowball.obj						\
	$(STEM_PAICE_HUSK)								\
	$(OBJDIR)\stemmer_factory.obj					\
	$(OBJDIR)\stem_otago.obj						\
	$(OBJDIR)\stem_otago_v2.obj						\
	$(OBJDIR)\stem_s.obj							\
	$(OBJDIR)\bitstream.obj							\
	$(OBJDIR)\compress_four_integer_variable_byte.obj		\
	$(OBJDIR)\compress_elias_gamma.obj				\
	$(OBJDIR)\compress_elias_delta.obj				\
	$(OBJDIR)\compress_golomb.obj					\
	$(OBJDIR)\compress_simple9.obj					\
	$(OBJDIR)\compress_relative10.obj				\
	$(OBJDIR)\compress_carryover12.obj				\
	$(OBJDIR)\compress_variable_byte.obj			\
	$(OBJDIR)\compress_none.obj						\
	$(OBJDIR)\compress_sigma.obj					\
	$(OBJDIR)\compress_simple16.obj					\
	$(OBJDIR)\compress_simple9_packed.obj		\
	$(OBJDIR)\compress_simple16_packed.obj		\
	$(OBJDIR)\compress_simple8b.obj		\
	$(OBJDIR)\compress_simple8b_packed.obj		\
	$(OBJDIR)\compression_factory.obj				\
	$(OBJDIR)\compress_text_none.obj				\
	$(OBJDIR)\compress_text_snappy.obj				\
	$(OBJDIR)\compress_text_bz2.obj					\
	$(OBJDIR)\compress_text_deflate.obj				\
	$(OBJDIR)\compression_text_factory.obj			\
	$(OBJDIR)\readability_dale_chall.obj			\
	$(OBJDIR)\readability_factory.obj				\
	$(OBJDIR)\readability_tag_weighting.obj			\
	$(OBJDIR)\search_engine_readability.obj			\
	$(OBJDIR)\plugin_manager.obj					\
	$(OBJDIR)\indexer_param_block_rank.obj			\
	$(OBJDIR)\indexer_param_block_stem.obj			\
	$(OBJDIR)\indexer_param_block_pregen.obj		\
	$(OBJDIR)\indexer_param_block_topsig.obj		\
	$(OBJDIR)\indexer_param_block.obj				\
	$(OBJDIR)\merger_param_block.obj				\
	$(OBJDIR)\ant_param_block.obj					\
	$(OBJDIR)\version.obj							\
	$(OBJDIR)\ranking_function.obj					\
	$(OBJDIR)\ranking_function_factory.obj			\
	$(OBJDIR)\ranking_function_impact.obj			\
	$(OBJDIR)\ranking_function_readability.obj		\
	$(OBJDIR)\ranking_function_lmds.obj				\
	$(OBJDIR)\ranking_function_lmd.obj				\
	$(OBJDIR)\ranking_function_lmjm.obj				\
	$(OBJDIR)\ranking_function_puurula_idf.obj		\
	$(OBJDIR)\ranking_function_puurula.obj			\
	$(OBJDIR)\ranking_function_docid.obj			\
	$(OBJDIR)\ranking_function_pregen.obj			\
	$(OBJDIR)\ranking_function_bose_einstein.obj	\
	$(OBJDIR)\ranking_function_divergence.obj		\
	$(OBJDIR)\ranking_function_bm25.obj				\
	$(OBJDIR)\ranking_function_bm25l.obj			\
	$(OBJDIR)\ranking_function_bm25plus.obj			\
	$(OBJDIR)\ranking_function_bm25adpt.obj			\
	$(OBJDIR)\ranking_function_bm25t.obj			\
	$(OBJDIR)\ranking_function_tflodop.obj			\
	$(OBJDIR)\ranking_function_dlh13.obj			\
	$(OBJDIR)\ranking_function_dph.obj				\
	$(OBJDIR)\ranking_function_dfree.obj			\
	$(OBJDIR)\ranking_function_term_count.obj		\
	$(OBJDIR)\ranking_function_inner_product.obj	\
	$(OBJDIR)\ranking_function_kbtfidf.obj			\
	$(OBJDIR)\ranking_function_topsig_positive.obj	\
	$(OBJDIR)\ranking_function_topsig_negative.obj	\
	$(OBJDIR)\ranking_function_dfi.obj 				\
	$(OBJDIR)\ranking_function_dfiw.obj 			\
	$(OBJDIR)\ranking_function_dfiw_idf.obj 		\
	$(OBJDIR)\ranking_function_dfi_idf.obj 			\
	$(OBJDIR)\instream_memory.obj					\
	$(OBJDIR)\instream_pkzip.obj					\
	$(OBJDIR)\instream_file_star.obj				\
	$(OBJDIR)\instream_file.obj						\
	$(OBJDIR)\instream_deflate.obj					\
	$(OBJDIR)\instream_bz2.obj						\
	$(OBJDIR)\instream_buffer.obj					\
	$(OBJDIR)\instream_lzo.obj						\
	$(OBJDIR)\instream_scrub.obj					\
	$(OBJDIR)\nexi.obj								\
	$(OBJDIR)\nexi_ant.obj							\
	$(OBJDIR)\nexi_term_iterator.obj				\
	$(OBJDIR)\nexi_term_ant.obj						\
	$(OBJDIR)\nexi_term.obj							\
	$(OBJDIR)\sockets.obj							\
	$(OBJDIR)\event.obj								\
	$(OBJDIR)\semaphores.obj						\
	$(OBJDIR)\critical_section.obj					\
	$(OBJDIR)\barrier.obj							\
	$(OBJDIR)\threads.obj							\
	$(OBJDIR)\focus.obj								\
	$(OBJDIR)\focus_article.obj						\
	$(OBJDIR)\focus_lowest_tag.obj					\
	$(OBJDIR)\focus_results_list.obj				\
	$(OBJDIR)\unicode_tables.obj					\
	$(OBJDIR)\pregen.obj							\
	$(OBJDIR)\pregen_kendall_tau.obj				\
	$(OBJDIR)\pregen_writer.obj						\
	$(OBJDIR)\pregens_writer.obj					\
	$(OBJDIR)\pregen_writer_exact_integers.obj		\
	$(OBJDIR)\pregen_writer_exact_strings.obj		\
	$(OBJDIR)\pregen_writer_normal.obj				\
	$(OBJDIR)\arithmetic_model_tables.obj			\
	$(OBJDIR)\arithmetic_model_bigram.obj			\
	$(OBJDIR)\arithmetic_model_unigram.obj			\
	$(OBJDIR)\unicode.obj							\
	$(OBJDIR)\snippet.obj							\
	$(OBJDIR)\snippet_factory.obj					\
	$(OBJDIR)\snippet_tficf.obj						\
	$(OBJDIR)\snippet_best_tag.obj					\
	$(OBJDIR)\snippet_tag.obj						\
	$(OBJDIR)\snippet_beginning.obj					\
	$(OBJDIR)\snippet_word_cloud.obj				\
	$(OBJDIR)\atire_client_param_block.obj			\
	$(OBJDIR)\atire_broker_param_block.obj			\
	$(OBJDIR)\atire_broke_engine.obj				\
	$(OBJDIR)\atire_engine_result_set.obj			\
	$(OBJDIR)\atire_engine_result_set_export_TREC.obj			\
	$(OBJDIR)\atire_engine_result_set_export_INEX_snippet.obj	\
	$(OBJDIR)\atire_broke.obj

#
#	Targets
#
ANT_TARGETS = \
	$(BINDIR)\index.exe 				\
	$(BINDIR)\atire.exe 				\
	$(BINDIR)\atire_broker.exe 			\
	$(BINDIR)\atire_client.exe			\
	$(BINDIR)\atire_dictionary.exe		\
	$(BINDIR)\atire_merge.exe			\
	$(BINDIR)\atire_doclist.exe			\
	$(LIBIDR)\atire.dll

OTHER_TARGETS = \
	$(BINDIR)\link_extract_inex_wikipedia.exe 	\
	$(BINDIR)\link_compute_inlink.exe 			\
	$(BINDIR)\link_analyse_inex_wikipedia.exe 	\
	$(BINDIR)\make_unicode_tables.exe 			\
	$(BINDIR)\filelist.exe 				\
	$(BINDIR)\remove_head.exe 			\
	$(BINDIR)\link_extract.exe 			\
	$(BINDIR)\link_index.exe 			\
	$(BINDIR)\link_this.exe				\
	$(BINDIR)\link_index_merge.exe		\
	$(BINDIR)\link_extract_pass2.exe	\
	$(BINDIR)\link_length_correlate.exe	\
	$(BINDIR)\topic_tree_cas.exe		\
	$(BINDIR)\bindiff.exe				\
	$(BINDIR)\term_frequencies.exe		\
	$(BINDIR)\topic_tree.exe			\
	$(BINDIR)\INEXqrels_to_run.exe		\
	$(BINDIR)\foltbl_to_aspt.exe		\
	$(BINDIR)\zipf_graph.exe			\
	$(BINDIR)\get_doclist.exe			\
	$(BINDIR)\ant_unzip.exe				\
	$(BINDIR)\producer_consumer.exe		\
	$(BINDIR)\test_tar.exe				\
	$(BINDIR)\test_stop.exe				\
	$(BINDIR)\textsplitter.exe			\
	$(BINDIR)\test_boolean_parser.exe	\
	$(BINDIR)\test_unicode_case_convert.exe			\
	$(BINDIR)\pregen_precision_measurement.exe		\
	$(BINDIR)\pregen_create.exe		\
	$(BINDIR)\pregen_examine.exe					\
	$(BINDIR)\test_pregen.exe						\
	$(BINDIR)\test_kendall_tau.exe					\
	$(BINDIR)\wordnet_to_ant_thesaurus.exe		\
	$(BINDIR)\wordnet_test.exe		\
	$(BINDIR)\arithmetic_encoding_model_gen.exe

#
#	List of objects to build
#
atire : makefile $(ANT_TARGETS)

all : makefile $(PARTS) \
      $(ANT_TARGETS) \
      $(OTHER_TARGETS)
      
swig :
!IF EXIST(atire/php) == 0
	mkdir atire/php
!ENDIF
	$(SWIG) -o atire/php/atire_remote_wrap.c -outdir atire/php -php -c++ atire/atire_api_remote.swig
	$(CC) $(CFLAGS) $(PHP_FLAGS) /LD /Tp atire\php\atire_remote_wrap.c $(PARTS) $(WINDOWS_LIBS) $(EXTRA_LIBS) /Featire\php\php_atire_remote.dll $(PHP_LIBS) /Foatire\php\php_atire_remote.obj /w

bin\link_index_merge.exe : $(OBJDIR)\link_index_merge.obj
bin\make_unicode_tables.exe : $(OBJDIR)\make_unicode_tables.obj
bin\test_unicode_case_convert.exe : $(OBJDIR)\test_unicode_case_convert.obj
bin\test_boolean_parser.exe : $(OBJDIR)\test_boolean_parser.obj
bin\link_extract_index_wikipedia.exe : $(OBJDIR)\link_extract_index_wikipedia.obj
bin\pregen_precision_measurement.exe : $(OBJDIR)\pregen_precision_measurement.obj $(OBJDIR)\pregen_kendall_tau.obj
bin\pregen_create.exe : $(OBJDIR)\pregen_create.obj
bin\pregen_examine.exe : $(OBJDIR)\pregen_examine.obj
bin\test_pregen.exe : $(OBJDIR)\test_pregen.obj
bin\test_kendall_tau.exe : $(OBJDIR)\test_kendall_tau.obj $(OBJDIR)\pregen_kendall_tau.obj
bin\arithmetic_encoding_model_gen.exe : $(OBJDIR)\arithmetic_encoding_model_gen.obj $(OBJDIR)\arithmetic_model_bigram.obj $(OBJDIR)\arithmetic_model_unigram.obj

#
#	Default dependency rules
#
{$(SRCDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /EHsc /c /Tp $< /Fo$@

{$(ATIREDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /EHsc /c -Isource /Tp $< /Fo$@

{$(LTWDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(TOOLDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /EHsc /c -Isource /Tp $< /Fo$@

{$(TESTDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /EHsc /c -Isource /Tp $< /Fo$@

{$(OBJDIR)\}.obj{$(BINDIR)\}.exe:
	@echo Building $@...
	$(CC) $(CFLAGS) $< $(PARTS) $(WINDOWS_LIBS) $(EXTRA_LIBS) /Fe$@  $(FIXED)

$(ANT_TARGETS) : $(PARTS) 
$(OTHER_TARGETS) : $(OBJDIR)\disk.obj
$(PARTS) : $(EXTRA_LIBS)

#
#	Make the external libraries
#
external\unencumbered\bzip\bzip2-1.0.6\libbz2.lib :
	@cd external\unencumbered\bzip\bzip2-1.0.6
	@nmake -nologo -f ..\makefile.msc COMPILER=$(COMPILER) DEBUG=$(DEBUG)
	@cd ..\..\..\..

external\unencumbered\zlib\zlib-1.2.5\zlib.lib :
	@cd external\unencumbered\zlib\zlib-1.2.5
	@nmake -nologo -f ..\makefile.msc COMPILER=$(COMPILER) DEBUG=$(DEBUG)
	@cd ..\..\..\..

external\unencumbered\snappy\snappy.lib :
	@cd external\unencumbered\snappy
	@nmake -nologo -f makefile.msc COMPILER=$(COMPILER) DEBUG=$(DEBUG)
	@cd ..\..\..

external\gpl\lzo\lzo2.lib :
	@cd external\gpl\lzo
	@nmake -nologo -f makefile.msc COMPILER=$(COMPILER) DEBUG=$(DEBUG)
	@cd ..\..\..

external\unencumbered\snowball\libstemmer_c\libstemmer.lib :
	@cd external\unencumbered\snowball\libstemmer_c
	@nmake -nologo -f ..\makefile.msc COMPILER=$(COMPILER) DEBUG=$(DEBUG)
	@cd ..\..\..\..

#
#	Management
#
clean :
	del *.obj *.exe *.ilk *.pdb *.suo *.lib *.dll /s

depend:
	makedepend  -f- -Y -o.obj -w1024 -p$(OBJDIR)/ source/*.c tools/*.c atire/*.c Link-The-Wiki/*.c | sed -e "s/$(OBJDIR)\/source/$(OBJDIR)/" | sed -e "s/$(OBJDIR)\/tools/$(OBJDIR)/" | sed -e "s/$(OBJDIR)\/atire/$(OBJDIR)/" | sed -e "s/$(OBJDIR)\/Link-The-Wiki/$(OBJDIR)/" > makefile.dependencies

#
#	And include the dependencie generated using makedepend from cygwin and "make depend"
#
!include makefile.dependencies
# DO NOT DELETE
