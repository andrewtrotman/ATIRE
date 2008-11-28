SRCDIR = source
OBJDIR = bin
BINDIR = bin

LDFLAGS = -g
CFLAGS = -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1  -O2

CC = g++

OBJECTS = \
	parser.o \
	memory_index_hash_node.o \
	memory.o \
	memory_index_stats.o \
	ctypes.o \
	hash_table.o \
	postings_piece.o \
	memory_index.o \
	disk.o \
	file.o \
	disk_internals.o \
	ant.o \
	index.o \
	main.o \
	search_engine.o \
	search_engine_accumulator.o

INDEX_PARTS = \
	parser.o \
	memory_index_hash_node.o \
	memory.o \
	memory_index_stats.o \
	ctypes.o \
	hash_table.o \
	postings_piece.o \
	memory_index.o \
	disk.o \
	file.o \
	disk_internals.o

ANT_PARTS = \
	search_engine.o \
	search_engine_accumulator.o \
	memory.o \
	ctypes.o \
	file.o 

all : $(BINDIR)/index $(BINDIR)/ant

$(OBJECTS) :
	$(CC) $(CFLAGS) -c $(SRCDIR)/$*.c -o $(OBJDIR)/$@

$(BINDIR)/index : $(INDEX_PARTS) index.o
	$(CC) $(LDFLAGS) -o $@ $(OBJDIR)/index.o $(INDEX_PARTS:%=$(OBJDIR)/%)

$(BINDIR)/ant : $(ANT_PARTS) ant.o 
	$(CC) $(CFLAGS) -o $@ $(OBJDIR)/ant.o $(ANT_PARTS:%=$(OBJDIR)/%)

$(OBJDIR)/index.o : $(SRCDIR)/index.c

$(OBJDIR)/main.o : $(SRCDIR)/main.c

$(OBJDIR)/ant.o : $(SRCDIR)/ant.c

$(OBJDIR)/disk_internals.o : $(SRCDIR)/disk_internals.c $(SRCDIR)/disk_internals.h

$(OBJDIR)/disk.o : $(SRCDIR)/disk.c $(SRCDIR)/disk.h $(SRCDIR)/disk_internals.h

$(OBJDIR)/parser.o : $(SRCDIR)/parser.c $(SRCDIR)/parser.h $(SRCDIR)/string_pair.h


.PHONY : clean
clean :
	rm $(OBJDIR)/*.o $(BINDIR)/index $(BINDIR)/ant $(BINDIR)/*.ilk $(BINDIR)/*.pdb $(BINDIR)/*.suo *.pdb
