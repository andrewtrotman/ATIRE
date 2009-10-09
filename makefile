#
#	Windows makefile for ANT and the ANT tools
#

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

#
#	Directories
#
SRCDIR = source
OBJDIR = bin
BINDIR = bin
LTWDIR = Link-The-Wiki
TOOLDIR = tools

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
#	Post configuration, so back to the hard work...
#
FIXED = /link /fixed:no

MINUS_D = $(EXTRA_MINUS_D) -DHASHER=1 -DHEADER_HASHER=1 -DSPECIAL_COMPRESSION=1
#MINUS_D = $(MINUS_D) -DPURIFY

#
#	Compiler and flags (the top line is debug, the bottom is release)
#
#CFLAGS = /Od /W4 -D_CRT_SECURE_NO_WARNINGS -D_DEBUG /nologo /Zi $(MINUS_D) $(EXTRA_INCLUDE)
CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi $(MINUS_D) /Ox /fp:fast /GL /Gy $(EXTRA_INCLUDE)
CC = @cl

#
#	Libraries
#
WINDOWS_LIBS = user32.lib advapi32.lib kernel32.lib shlwapi.lib

#
#	Objects
#
PARTS = \
	$(OBJDIR)\parser.obj 							\
	$(OBJDIR)\parser_readability.obj				\
	$(OBJDIR)\memory_index_hash_node.obj			\
	$(OBJDIR)\memory_index.obj 						\
	$(OBJDIR)\memory_index_stats.obj				\
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
	$(OBJDIR)\mean_average_precision.obj 			\
	$(OBJDIR)\assessment.obj						\
	$(OBJDIR)\assessment_ANT.obj					\
	$(OBJDIR)\assessment_INEX.obj					\
	$(OBJDIR)\assessment_factory.obj				\
	$(OBJDIR)\relevant_document.obj 				\
	$(OBJDIR)\stats.obj								\
	$(OBJDIR)\time_stats.obj						\
	$(OBJDIR)\search_engine_stats.obj				\
	$(OBJDIR)\search_engine_forum.obj				\
	$(OBJDIR)\search_engine_forum_INEX.obj			\
	$(OBJDIR)\search_engine_forum_INEX_efficiency.obj			\
	$(OBJDIR)\search_engine_forum_TREC.obj			\
	$(OBJDIR)\str.obj 								\
	$(OBJDIR)\stop_word.obj 						\
	$(OBJDIR)\disk.obj 								\
	$(OBJDIR)\disk_internals.obj 					\
	$(OBJDIR)\directory_iterator_tar.obj		\
	$(OBJDIR)\directory_recursive_iterator.obj		\
	$(OBJDIR)\btree_iterator.obj 					\
	$(OBJDIR)\top_k_sort.obj 						\
	$(OBJDIR)\stemmer.obj							\
	$(OBJDIR)\stemmer_term_similarity.obj			\
	$(OBJDIR)\stemmer_term_similarity_threshold.obj	\
	$(OBJDIR)\stemmer_term_similarity_weighted.obj	\
	$(OBJDIR)\porter.obj							\
	$(OBJDIR)\lovins.obj							\
	$(OBJDIR)\stemmer_factory.obj					\
	$(OBJDIR)\paice_husk.obj						\
	$(OBJDIR)\learned_wikipedia_stem.obj			\
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
	$(OBJDIR)\readability_dale_chall.obj			\
	$(OBJDIR)\readability_factory.obj				\
	$(OBJDIR)\search_engine_readability.obj			\
	$(OBJDIR)\plugin_manager.obj					\
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
	$(OBJDIR)\instream_file.obj						\
	$(OBJDIR)\instream_deflate.obj					\
	$(OBJDIR)\instream_bz2.obj						\
	$(OBJDIR)\nexi.obj								\
	$(OBJDIR)\nexi_term_iterator.obj				\
	$(OBJDIR)\nexi_term.obj
	

#
#	Targets
#
ANT_TARGETS = \
	$(BINDIR)\index.exe 				\
	$(BINDIR)\ant.exe 					\
	$(BINDIR)\ant_dictionary.exe

OTHER_TARGETS = \
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
	$(BINDIR)\test_tar.exe

#
#	Default dependency rules
#
{$(SRCDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(LTWDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(TOOLDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(OBJDIR)\}.obj{$(BINDIR)\}.exe:
	@echo Building $@...
	$(CC) $(CFLAGS) $*.obj $(PARTS) $(WINDOWS_LIBS) $(EXTRA_LIBS) /Fe$@  $(FIXED)

#
#	List of objects to build
#
all : $(PARTS)		\
      $(ANT_TARGETS)	\
      $(OTHER_TARGETS)

$(ANT_TARGETS) : $(PARTS) 
$(OTHER_TARGETS) : $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj
$(PARTS) : makefile $(EXTRA_LIBS)

#
#	Some dependencies need to be explicit (is this an nmake bug)?
#
$(BINDIR)\link_index_merge.exe : $(OBJDIR)\link_index_merge.obj
$(BINDIR)\link_length_correlate.exe : $(OBJDIR)\link_length_correlate.obj
$(BINDIR)\bindiff.exe : $(OBJDIR)\bindiff.obj
$(BINDIR)\filelist.exe : $(OBJDIR)\filelist.obj
$(BINDIR)\index.exe : $(OBJDIR)\index.obj
$(BINDIR)\ant.exe : $(OBJDIR)\ant.obj
$(BINDIR)\test_tar.exe : $(OBJDIR)\test_tar.obj

#
#	Make the external libraries
#
bzip\bzip2-1.0.5\libbz2.lib :
	cd bzip\bzip2-1.0.5
	make -f ..\makefile.msc
	cd ..\..

zlib\zlib-1.2.3\zlib.lib : 
	cd zlib\zlib-1.2.3
	make -f ..\makefile.msc
	cd ..\..

#
#	Management
#
clean :
	del $(OBJDIR)\*.obj $(BINDIR)\*.exe $(BINDIR)\*.ilk $(BINDIR)\*.pdb $(BINDIR)\*.suo *.pdb

depend:
	makedepend  -f- -Y -o.obj -w1024 -pbin/ source/*.c tools/*.c Link-The-Wiki/*.c | sed -e "s/bin\/source/bin/" | sed -e "s/bin\/tools/bin/" | sed -e "s/bin\/Link-The-Wiki/bin/" > makefile.dependencies

#
#	And include the dependencie generated using makedepend from cygwin and "make depend"
#
!include makefile.dependencies
