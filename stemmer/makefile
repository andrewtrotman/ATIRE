EXTDIR = .
SRCDIR = ../source
OBJDIR = ../bin
BINDIR = ../bin

EXTFLAGS = -I$(SRCDIR)
LDFLAGS = -g
CFLAGS = -g -O3 -Wall -DFIT_BM25 -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
	-Wno-endif-labels

CC = g++

INDEX_PARTS = \
	disk.o \
	disk_internals.o \
	file.o \
	parser.o \
	memory_index_hash_node.o \
	memory.o \
	memory_index.o \
	stats.o \
	time_stats.o \
	memory_index_stats.o \
	ctypes.o \
	hash_table.o \
	postings_piece.o

ANT_PARTS = \
	ctypes.o \
    file.o \
    memory.o \
    search_engine.o \
    search_engine_accumulator.o \
    mean_average_precision.o \
    INEX_assessment.o \
    relevant_document.o \
    stats.o \
    time_stats.o \
    search_engine_stats.o \
    search_engine_forum.o \
	search_engine_forum_INEX.o \
	search_engine_forum_TREC.o \
	str.o \
	stop_word.o \
    disk.o \
    disk_internals.o \
    btree_iterator.o \
    top_k_sort.o \
    stemmer.o	\
	porter.o \
    learned_wikipedia_stem.o \
    relevant_topic.o

EXTENSION = \
    ant.o \
    ga.o \
    ga_stemmer.o \
    ga_individual.o \
	ga_function.o \
	strgen.o \
	counter.o

all : $(BINDIR)/index $(BINDIR)/ant

%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$*.c -o $(OBJDIR)/$*.o

$(EXTENSION) : 
	$(CC) $(CFLAGS) $(EXTFLAGS) -c $*.c -o $(OBJDIR)/$@

$(BINDIR)/index : $(INDEX_PARTS) index.o
	$(CC) $(LDFLAGS) -o $@ $(OBJDIR)/index.o $(INDEX_PARTS:%=$(OBJDIR)/%)

$(BINDIR)/ant : $(ANT_PARTS) $(EXTENSION) 
	$(CC) $(LDFLAGS) -o $@ $(ANT_PARTS:%=$(OBJDIR)/%) $(EXTENSION:%=$(OBJDIR)/%)

.PHONY : clean
clean :
	-rm $(OBJDIR)/*.o $(BINDIR)/index $(BINDIR)/ant
