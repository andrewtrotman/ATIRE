#
#	Windows makefile for ANT and the ANT tools
#

#
#	Directories
#
SRCDIR = source
OBJDIR = bin
BINDIR = bin
LTWDIR = Link-The-Wiki
TOOLDIR = tools

#
#	Compiler and flags
#
CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi -DHASHER=1 -DHEADER_HASHER=1 /O2
CC = @cl

#
#	Libraries
#
WINDOWS_LIBS = user32.lib advapi32.lib kernel32.lib

#
#	Objects
#
PARTS = \
	$(OBJDIR)\parser.obj \
	$(OBJDIR)\memory_index_hash_node.obj\
	$(OBJDIR)\memory_index.obj \
	$(OBJDIR)\memory_index_stats.obj\
	$(OBJDIR)\hash_table.obj\
	$(OBJDIR)\postings_piece.obj	\
	$(OBJDIR)\ctypes.obj \
	$(OBJDIR)\file.obj\
	$(OBJDIR)\file_internals.obj\
	$(OBJDIR)\memory.obj \
	$(OBJDIR)\search_engine.obj \
	$(OBJDIR)\search_engine_accumulator.obj \
	$(OBJDIR)\mean_average_precision.obj \
	$(OBJDIR)\INEX_assessment.obj \
	$(OBJDIR)\relevant_document.obj \
	$(OBJDIR)\stats.obj\
	$(OBJDIR)\time_stats.obj\
	$(OBJDIR)\search_engine_stats.obj\
	$(OBJDIR)\search_engine_forum.obj\
	$(OBJDIR)\search_engine_forum_INEX.obj\
	$(OBJDIR)\search_engine_forum_TREC.obj\
	$(OBJDIR)\str.obj \
	$(OBJDIR)\stop_word.obj \
	$(OBJDIR)\disk.obj \
	$(OBJDIR)\disk_internals.obj \
	$(OBJDIR)\btree_iterator.obj \
	$(OBJDIR)\top_k_sort.obj \
	$(OBJDIR)\stemmer.obj	\
	$(OBJDIR)\porter.obj	\
	$(OBJDIR)\learned_wikipedia_stem.obj	\
	$(OBJDIR)\relevant_topic.obj

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
	$(CC) $(CFLAGS) $*.obj $(PARTS) $(WINDOWS_LIBS) /Fe$@

#
#	List of objects to build
#
all : $(PARTS)							\
      $(BINDIR)\index.exe 				\
	  $(BINDIR)\ant.exe 				\
	  $(BINDIR)\ant_dictionary.exe 		\
	  $(BINDIR)\link_extract.exe 		\
	  $(BINDIR)\link_index.exe 			\
	  $(BINDIR)\remove_head.exe 		\
	  $(BINDIR)\link_this.exe			\
	  $(BINDIR)\link_index_merge.exe	\
	  $(BINDIR)\link_extract_pass2.exe	\
	  $(BINDIR)\link_length_correlate.exe	\
	  $(BINDIR)\topic_tree_cas.exe		\
	  $(BINDIR)\bindiff.exe		\
	  $(BINDIR)\term_frequencies.exe	\
	  $(BINDIR)\topic_tree.exe		\
	  $(BINDIR)\INEXqrels_to_run.exe		\
	  $(BINDIR)\foltbl_to_aspt.exe		\
	  $(BINDIR)\zipf_graph.exe

#
#	Some dependencies need to be explicit (is this an nmake bug)?
#
$(BINDIR)\link_index_merge.exe : $(OBJDIR)\link_index_merge.obj
$(BINDIR)\link_length_correlate.exe : $(OBJDIR)\link_length_correlate.obj
$(BINDIR)\bindiff.exe : $(OBJDIR)\bindiff.obj



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
