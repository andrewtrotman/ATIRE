SRCDIR = source
OBJDIR = bin
BINDIR = bin
LTWDIR = Link-The-Wiki
TOOLDIR = tools

CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi -DHASHER=1 -DHEADER_HASHER=1  /O2
CC = @cl

PARTS = \
	$(OBJDIR)\disk.obj \
	$(OBJDIR)\disk_internals.obj \
	$(OBJDIR)\file.obj\
	$(OBJDIR)\parser.obj \
	$(OBJDIR)\memory_index_hash_node.obj\
	$(OBJDIR)\memory.obj \
	$(OBJDIR)\memory_index.obj \
	$(OBJDIR)\stats.obj\
	$(OBJDIR)\time_stats.obj\
	$(OBJDIR)\memory_index_stats.obj\
	$(OBJDIR)\ctypes.obj \
	$(OBJDIR)\hash_table.obj\
	$(OBJDIR)\postings_piece.obj

ANT_PARTS = \
	$(OBJDIR)\ctypes.obj \
	$(OBJDIR)\file.obj\
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
	$(OBJDIR)\str.obj \
	$(OBJDIR)\stop_word.obj \
	$(OBJDIR)\disk.obj \
	$(OBJDIR)\disk_internals.obj \
	$(OBJDIR)\btree_iterator.obj \
	$(OBJDIR)\top_k_sort.obj \
	$(OBJDIR)\stemmer.obj	\
	$(OBJDIR)\porter.obj	\
	$(OBJDIR)\stemmer_porter.obj	\
	$(OBJDIR)\relevant_topic.obj
	
{$(SRCDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(LTWDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

{$(TOOLDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@


all : $(BINDIR)\index.exe 				\
	  $(BINDIR)\ant.exe 				\
	  $(BINDIR)\ant_dictionary.exe 		\
	  $(BINDIR)\link_extract.exe 		\
	  $(BINDIR)\link_index.exe 			\
	  $(BINDIR)\remove_head.exe 		\
	  $(BINDIR)\link_this.exe			\
	  $(BINDIR)\link_index_merge.exe	\
	  $(BINDIR)\link_extract_pass2.exe	\
	  $(BINDIR)\link_length_correlate.exe \
	  $(BINDIR)\topic_tree.exe

$(BINDIR)\index.exe : $(PARTS) $(OBJDIR)\index.obj
	$(CC) $(CFLAGS) $(OBJDIR)\index.obj $(PARTS) /Fe$@ /link /fixed:no /incremental:no /profile

$(BINDIR)\ant.exe : $(ANT_PARTS) $(OBJDIR)\ant.obj
	$(CC) $(CFLAGS) $(OBJDIR)\ant.obj $(ANT_PARTS) /Fe$@

$(BINDIR)\ant_dictionary.exe : $(ANT_PARTS) $(OBJDIR)\ant_dictionary.obj
	$(CC) $(CFLAGS) $(OBJDIR)\ant_dictionary.obj $(ANT_PARTS) /Fe$@

$(BINDIR)\link_extract.exe : $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj $(OBJDIR)\link_extract.obj
	$(CC) $(CFLAGS) $(OBJDIR)\link_extract.obj $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj /Fe$@

$(BINDIR)\link_extract_pass2.exe : $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj $(OBJDIR)\link_extract_pass2.obj
	$(CC) $(CFLAGS) $(OBJDIR)\link_extract_pass2.obj $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj /Fe$@

$(BINDIR)\link_index.exe : $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj $(OBJDIR)\link_index.obj
	$(CC) $(CFLAGS) $(OBJDIR)\link_index.obj $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj /Fe$@

$(BINDIR)\link_index_merge.exe : $(OBJDIR)\link_index_merge.obj
	$(CC) $(CFLAGS) $(OBJDIR)\link_index_merge.obj /Fe$@

$(BINDIR)\link_length_correlate.exe : $(OBJDIR)\link_length_correlate.obj
	$(CC) $(CFLAGS) $(OBJDIR)\link_length_correlate.obj /Fe$@

$(BINDIR)\link_this.exe : $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj $(OBJDIR)\link_this.obj
	$(CC) $(CFLAGS) $(OBJDIR)\link_this.obj $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj /Fe$@

$(BINDIR)\remove_head.exe : $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj $(OBJDIR)\remove_head.obj
	$(CC) $(CFLAGS) $(OBJDIR)\remove_head.obj $(OBJDIR)\disk.obj $(OBJDIR)\disk_internals.obj /Fe$@

$(BINDIR)\topic_tree.exe : $(ANT_PARTS) $(OBJDIR)\topic_tree.obj
	$(CC) $(CFLAGS) $(OBJDIR)\topic_tree.obj $(ANT_PARTS) /Fe$@

clean :
	del $(OBJDIR)\*.obj $(BINDIR)\*.exe $(BINDIR)\*.ilk $(BINDIR)\*.pdb $(BINDIR)\*.suo *.pdb
