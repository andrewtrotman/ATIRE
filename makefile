SRCDIR = source
OBJDIR = bin
BINDIR = bin

#CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi  -DHEADER_HASHER /O2
CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi -DHASHER=1 -DHEADER_HASHER=1 /O2
#CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi /O2
CC = @cl

PARTS = \
	$(OBJDIR)\disk.obj \
	$(OBJDIR)\disk_internals.obj \
	$(OBJDIR)\file.obj\
	$(OBJDIR)\parser.obj \
	$(OBJDIR)\memory_index_hash_node.obj\
	$(OBJDIR)\memory.obj \
	$(OBJDIR)\memory_index.obj \
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
	$(OBJDIR)\relevant_document.obj \
	$(OBJDIR)\disk.obj \
	$(OBJDIR)\disk_internals.obj \
	$(OBJDIR)\btree_iterator.obj \
	$(OBJDIR)\relevant_topic.obj
	

{$(SRCDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

all : $(BINDIR)\index.exe $(BINDIR)\ant.exe $(BINDIR)\ant_dictionary.exe

$(BINDIR)\index.exe : $(PARTS) $(OBJDIR)\index.obj
	$(CC) $(CFLAGS) $(OBJDIR)\index.obj $(PARTS) /Fe$@ /link /fixed:no /incremental:no /profile

$(BINDIR)\ant.exe : $(ANT_PARTS) $(OBJDIR)\ant.obj
	$(CC) $(CFLAGS) $(OBJDIR)\ant.obj $(ANT_PARTS) /Fe$@ 

$(BINDIR)\ant_dictionary.exe : $(ANT_PARTS) $(OBJDIR)\ant_dictionary.obj
	$(CC) $(CFLAGS) $(OBJDIR)\ant_dictionary.obj $(ANT_PARTS) /Fe$@ 

$(OBJDIR)\index.obj : $(SRCDIR)\index.c

$(OBJDIR)\main.obj : $(SRCDIR)\main.c

$(OBJDIR)\disk_internals.obj : $(SRCDIR)\disk_internals.c $(SRCDIR)\disk_internals.h

$(OBJDIR)\disk.obj : $(SRCDIR)\disk.c  $(SRCDIR)\disk.h $(SRCDIR)\disk_internals.h

$(OBJDIR)\parse.obj : $(SRCDIR)\parse.c $(SRCDIR)\parse.h $(SRCDIR)\string_pair.h

clean :
	del $(OBJDIR)\*.obj $(BINDIR)\*.exe $(BINDIR)\*.ilk $(BINDIR)\*.pdb $(BINDIR)\*.suo *.pdb
