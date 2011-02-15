#
#	Windows makefile for ANT and the ANT tools
#

#
#	DEBUG. Comment out the line below to compile in DEBUG mode
#
DEBUG = 0

#
#	Which compiler are we using
#
COMPILER=MICROSOFT
#COMPILER=INTEL

#
#	Define TRUE and FALSE
#
FALSE = 0
TRUE = 1

#
#	Declare which external libraries to include
#
ANT_HAS_ZLIB = $(TRUE)
ANT_HAS_BZLIB = $(TRUE)
ANT_HAS_LOVINS = $(TRUE)
ANT_HAS_PAICE_HUSK = $(TRUE)
ANT_HAS_MYSQL = $(FALSE)
ANT_HAS_PHP_EXT = $(TRUE)

#
#	Directories
#
SRCDIR = source
OBJDIR = bin
BINDIR = bin
LTWDIR = Link-The-Wiki
TOOLDIR = tools
PHPDIR = php_ext
LIBDIR = lib

#
#	Directory to PHP source for headers
#
PHPSRC = C:\Work\php53dev\vc9\x86\php-5.3.5

#
#	Directory to PHP install location for library
#
PHPAPP = "C:\Program Files\PHP"

#
#	If we have MySQL (for access to MySQL)  The MySQL is GPL so there's a possible conflict here.
#
!IF $(ANT_HAS_MYSQL) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_MYSQL
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I \MySQL\server\include
EXTRA_LIBS = $(EXTRA_LIBS) \mysql\server\lib\opt\libmysql.lib
DIRECTORY_ITERATOR_MYSQL = $(OBJDIR)\directory_iterator_mysql.obj
!ELSE
DIRECTORY_ITERATOR_MYSQL = 
!ENDIF

#
#	If we have ZLIB (for the GZIP compression scheme)
#
!IF $(ANT_HAS_ZLIB) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_ZLIB
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I zlib\zlib-1.2.3
EXTRA_LIBS = $(EXTRA_LIBS) zlib\zlib-1.2.3\zlib.lib
!ENDIF

#
# If we have BZLIB (for the BZ2 compression scheme)
#
!IF $(ANT_HAS_BZLIB) == $(TRUE)
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_BZLIB
EXTRA_INCLUDE = $(EXTRA_INCLUDE) -I bzip\bzip2-1.0.5
EXTRA_LIBS = $(EXTRA_LIBS) bzip\bzip2-1.0.5\libbz2.lib
!ENDIF

#
#	Now for GPL / BSD license conflicts.
#

#
#	Lovins Stemmer
#
!IF $(ANT_HAS_LOVINS) == $(TRUE)
STEM_LOVINS = $(OBJDIR)\stem_lovins.obj
EXTRA_MINUS_D = $(EXTRA_MINUS_D) -DANT_HAS_LOVINS
!ELSE
STEM_LOVINS = 
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
#	PHP EXTENSION 
#
!IF $(ANT_HAS_PHP_EXT) == $(TRUE)
PHP_EXT = $(lib)\atire.dll
PHP_EXT_MINUS_D = -DANT_HAS_PHP_EXT -DWIN32 -DPHP_WIN32 -DZEND_WIN32 -DZTS=1 -DZEND_DEBUG=0 -D_USRDLL -D_WINDLL
PHP_EXT_INCLUDES = -I $(PHPSRC) -I $(PHPSRC)\TSRM -I $(PHPSRC)\Zend -I $(PHPSRC)\main -I \$(SRCDIR)
PHP_EXT_LIB = $(PHPAPP)\dev\php5ts.lib
!ENDIF
#
#	Post configuration, so back to the hard work...
#
FIXED = /link /fixed:no /opt:ref

MINUS_D = $(EXTRA_MINUS_D) -DHASHER=1 -DHEADER_HASHER=1 
MINUS_D = $(MINUS_D) -DSPECIAL_COMPRESSION=1
#MINUS_D = $(MINUS_D) -DTOP_K_SEARCH
MINUS_D = $(MINUS_D) -DTWO_D_ACCUMULATORS
MINUS_D = $(MINUS_D) -DHEAP_K_SEARCH
MINUS_D = $(MINUS_D) -DANDREW_HEAP_BITS
MINUS_D = $(MINUS_D) -DTOP_K_READ_AND_DECOMPRESSOR
MINUS_D = $(MINUS_D) -DPARALLEL_INDEXING
MINUS_D = $(MINUS_D) -DPARALLEL_INDEXING_DOCUMENTS
MINUS_D = $(MINUS_D) -DANT_ACCUMULATOR_T=short
#MINUS_D = $(MINUS_D) -DPURIFY

#
#	Compiler and flags (the top line is debug, the bottom is release)
#

!IF "$(COMPILER)" == "MICROSOFT"
CC = @cl /nologo
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
	$(OBJDIR)\relevance_feedback_factory.obj 		\
	$(OBJDIR)\term_divergence_kl.obj 				\
	$(OBJDIR)\query.obj 							\
	$(OBJDIR)\query_parse_tree.obj 					\
	$(OBJDIR)\query_boolean.obj 					\
	$(OBJDIR)\bitstring.obj 						\
	$(OBJDIR)\bitstring_iterator.obj 				\
	$(OBJDIR)\channel_file.obj						\
	$(OBJDIR)\channel_socket.obj					\
	$(OBJDIR)\parser.obj 							\
	$(OBJDIR)\parser_readability.obj				\
	$(OBJDIR)\memory_index_hash_node.obj			\
	$(OBJDIR)\memory_index.obj 						\
	$(OBJDIR)\memory_indexer.obj 					\
	$(OBJDIR)\memory_index_one.obj 					\
	$(OBJDIR)\stats_memory_index.obj				\
	$(OBJDIR)\hash_table.obj						\
	$(OBJDIR)\postings_piece.obj					\
	$(OBJDIR)\ctypes.obj 							\
	$(OBJDIR)\file.obj								\
	$(OBJDIR)\file_memory.obj						\
	$(OBJDIR)\file_internals.obj					\
	$(OBJDIR)\maths.obj 							\
	$(OBJDIR)\memory.obj 							\
	$(OBJDIR)\search_engine.obj 					\
	$(OBJDIR)\search_engine_accumulator.obj 		\
	$(OBJDIR)\search_engine_result.obj 				\
	$(OBJDIR)\search_engine_result_iterator.obj 	\
	$(OBJDIR)\mean_average_precision.obj 			\
	$(OBJDIR)\assessment.obj						\
	$(OBJDIR)\assessment_ANT.obj					\
	$(OBJDIR)\assessment_TREC.obj					\
	$(OBJDIR)\assessment_INEX.obj					\
	$(OBJDIR)\assessment_factory.obj				\
	$(OBJDIR)\relevant_document.obj 				\
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
	$(OBJDIR)\directory_iterator_preindex_internals.obj \
	$(OBJDIR)\directory_iterator_compressor.obj 	\
	$(OBJDIR)\directory_iterator_deflate.obj 		\
	$(OBJDIR)\directory_iterator_internals.obj 		\
	$(OBJDIR)\directory_iterator_tar.obj			\
	$(OBJDIR)\directory_iterator_warc.obj			\
	$(OBJDIR)\directory_iterator_warc_gz_recursive.obj		\
	$(OBJDIR)\directory_iterator_pkzip.obj			\
	$(OBJDIR)\directory_iterator_file.obj			\
	$(OBJDIR)\directory_iterator_csv.obj			\
	$(OBJDIR)\directory_iterator_recursive.obj		\
	$(DIRECTORY_ITERATOR_MYSQL)						\
	$(OBJDIR)\btree_iterator.obj 					\
	$(OBJDIR)\stemmer.obj							\
	$(OBJDIR)\stemmer_term_similarity.obj			\
	$(OBJDIR)\stemmer_term_similarity_threshold.obj	\
	$(OBJDIR)\stemmer_term_similarity_weighted.obj	\
	$(OBJDIR)\stem_porter.obj						\
	$(STEM_LOVINS)									\
	$(STEM_PAICE_HUSK)								\
	$(OBJDIR)\stemmer_factory.obj					\
	$(OBJDIR)\stem_otago.obj						\
	$(OBJDIR)\stem_s.obj							\
	$(OBJDIR)\relevant_topic.obj					\
	$(OBJDIR)\bitstream.obj							\
	$(OBJDIR)\compress_elias_gamma.obj				\
	$(OBJDIR)\compress_elias_delta.obj				\
	$(OBJDIR)\compress_golomb.obj					\
	$(OBJDIR)\compress_simple9.obj					\
	$(OBJDIR)\compress_relative10.obj				\
	$(OBJDIR)\compress_carryover12.obj				\
	$(OBJDIR)\compress_variable_byte.obj			\
	$(OBJDIR)\compress_none.obj						\
	$(OBJDIR)\compress_sigma.obj					\
	$(OBJDIR)\compression_factory.obj				\
	$(OBJDIR)\compress_text_none.obj				\
	$(OBJDIR)\compress_text_bz2.obj					\
	$(OBJDIR)\compress_text_deflate.obj				\
	$(OBJDIR)\compression_text_factory.obj			\
	$(OBJDIR)\readability_dale_chall.obj			\
	$(OBJDIR)\readability_factory.obj				\
	$(OBJDIR)\search_engine_readability.obj			\
	$(OBJDIR)\plugin_manager.obj					\
	$(OBJDIR)\indexer_param_block_rank.obj			\
	$(OBJDIR)\indexer_param_block_stem.obj			\
	$(OBJDIR)\indexer_param_block.obj				\
	$(OBJDIR)\ant_param_block.obj					\
	$(OBJDIR)\version.obj							\
	$(OBJDIR)\ranking_function.obj					\
	$(OBJDIR)\ranking_function_impact.obj			\
	$(OBJDIR)\ranking_function_similarity.obj		\
	$(OBJDIR)\ranking_function_readability.obj		\
	$(OBJDIR)\ranking_function_lmd.obj				\
	$(OBJDIR)\ranking_function_lmjm.obj				\
	$(OBJDIR)\ranking_function_bose_einstein.obj	\
	$(OBJDIR)\ranking_function_divergence.obj		\
	$(OBJDIR)\ranking_function_bm25.obj				\
	$(OBJDIR)\ranking_function_term_count.obj		\
	$(OBJDIR)\ranking_function_inner_product.obj	\
	$(OBJDIR)\ranking_function_kbtfidf.obj			\
	$(OBJDIR)\instream_file.obj						\
	$(OBJDIR)\instream_deflate.obj					\
	$(OBJDIR)\instream_bz2.obj						\
	$(OBJDIR)\instream_buffer.obj					\
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
	$(OBJDIR)\unicode_case.obj						
	
PHP_EXT_OBJ	= \
	$(OBJDIR)\atire_api_remote.obj			\
	$(OBJDIR)\str.obj						\
	$(OBJDIR)\sockets.obj					\
	$(OBJDIR)\ctypes.obj					\
	$(OBJDIR)\atire_php_ext.obj		

#
#	Targets
#
ANT_TARGETS = \
	$(BINDIR)\index.exe 				\
	$(BINDIR)\ant.exe 					\
	$(BINDIR)\atire.exe 				\
	$(BINDIR)\atire_client.exe			\
	$(BINDIR)\ant_dictionary.exe

OTHER_TARGETS = \
	$(BINDIR)\link_extract_inex_wikipedia.exe \
	$(BINDIR)\link_compute_inlink.exe \
	$(BINDIR)\link_analyse_inex_wikipedia.exe \
	$(BINDIR)\make_case_conversion_table.exe 		\
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
	$(LIBIDR)\atire.dll

#
#	List of objects to build
#
all : $(PARTS) \
      $(ANT_TARGETS) \
      $(OTHER_TARGETS)
      
php_ext :\
	$(PHP_EXT_OBJ)\
	$(LIBDIR)\atire.dll

bin\link_index_merge.exe : bin\link_index_merge.obj
bin\make_case_conversion_table.exe : bin\make_case_conversion_table.obj
bin\test_unicode_case_convert.exe : bin\test_unicode_case_convert.obj
bin\test_boolean_parser.exe : bin\test_boolean_parser.obj
bin\link_extract_index_wikipedia.exe : bin\link_extract_index_wikipedia.obj

#
#	Default dependency rules
#
{$(SRCDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /EHsc /c /Tp $< /Fo$@

{$(PHPDIR)\}.c{$(OBJDIR)\}.obj:
	@echo Compiling $@..
	$(CC) $(PHP_EXT_MINUS_D) $(PHP_EXT_INCLUDES) /EHsc /W4 /Od /c /Tp $< /Fo$@

{$(LTWDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(TOOLDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@
	
{$(OBJDIR)\}.obj{$(LIBDIR)\}.dll:
	@echo Building $@..
	$(CC) /LD $(PHP_EXT_OBJ) $(PHP_EXT_LIB) $(WINDOWS_LIBS) /Fe$@ /link /FORCE:MULTIPLE /LTCG
	
	
{$(OBJDIR)\}.obj{$(BINDIR)\}.exe:
	@echo Building $@...
	$(CC) $(CFLAGS) $*.obj $(PARTS) $(WINDOWS_LIBS) $(EXTRA_LIBS) /Fe$@  $(FIXED)


$(ANT_TARGETS) : $(PARTS) 
$(OTHER_TARGETS) : $(OBJDIR)\disk.obj
$(PARTS) : makefile $(EXTRA_LIBS)

#
#	Make the external libraries
#
bzip\bzip2-1.0.5\libbz2.lib :
	@cd bzip\bzip2-1.0.5
	@nmake -nologo -f ..\makefile.msc COMPILER=$(COMPILER) $DEBUG=$(DEBUG)
	@cd ..\..

zlib\zlib-1.2.3\zlib.lib : 
	@cd zlib\zlib-1.2.3
	@nmake -nologo -f ..\makefile.msc COMPILER=$(COMPILER) $DEBUG=$(DEBUG)
	@cd ..\..

#
#	Management
#
clean :
	del *.obj *.exe *.ilk *.pdb *.suo *.lib *.dll /s

depend:
	makedepend  -f- -Y -o.obj -w1024 -pbin/ source/*.c tools/*.c Link-The-Wiki/*.c | sed -e "s/bin\/source/bin/" | sed -e "s/bin\/tools/bin/" | sed -e "s/bin\/Link-The-Wiki/bin/" > makefile.dependencies

#
#	And include the dependencie generated using makedepend from cygwin and "make depend"
#
!include makefile.dependencies
