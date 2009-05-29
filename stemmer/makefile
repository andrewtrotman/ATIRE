EXTDIR = .
SRCDIR = ../source
OBJDIR = ../bin
BINDIR = ../bin

EXTFLAGS = -I$(SRCDIR)
LDFLAGS = -g
CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
	-Wno-endif-labels -Wno-missing-braces

CC = g++


OBJECTS = \
    parser.o \
    memory_index_hash_node.o \
    memory_index.o \
    memory_index_stats.o \
    hash_table.o \
    postings_piece.o \
    ctypes.o \
    file.o \
    file_internals.o \
    maths.o \
    memory.o \
    search_engine.o \
    search_engine_accumulator.o \
    mean_average_precision.o \
    assessment.o \
    assessment_ANT.o \
    assessment_INEX.o \
    assessment_factory.o \
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
	directory_recursive_iterator.o \
    btree_iterator.o \
    top_k_sort.o \
    stemmer.o	\
    porter.o \
    lovins.o \
    stemmer_factory.o \
    paice_husk.o \
    learned_wikipedia_stem.o \
    relevant_topic.o \
	bitstream.o \
	compress_elias_gamma.o \
	compress_elias_delta.o \
	compress_golomb.o \
	compress_simple9.o \
	compress_relative10.o \
	compress_carryover12.o \
	compress_variable_byte.o \
	compress_none.o \
	compress_sigma.o \
	compression_factory.o \
	indexer_param_block.o \
    ant_param_block.o \
    version.o

EXTENSION = \
	ga_ant.o \
    ga.o \
    ga_stemmer.o \
    ga_individual.o \
	ga_function.o \
	vocab.o 

all : $(BINDIR)/index $(BINDIR)/ant $(BINDIR)/ga_ant

%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$*.c -o $(OBJDIR)/$*.o

$(EXTENSION) : 
	$(CC) $(CFLAGS) $(EXTFLAGS) -c $*.c -o $(OBJDIR)/$@

$(BINDIR)/index : $(OBJECTS) index.o
	$(CC) $(LDFLAGS) -o $@ $(OBJDIR)/index.o $(OBJECTS:%=$(OBJDIR)/%)

$(BINDIR)/ant : $(OBJECTS) ant.o
	$(CC) $(LDFLAGS) -o $@ $(OBJDIR)/ant.o $(OBJECTS:%=$(OBJDIR)/%) 

$(BINDIR)/ant_dict : $(OBJECTS) ant_dictionary.o
	$(CC) $(LDFLAGS) -o $@ $(OBJDIR)/ant_dictionary.o $(OBJECTS:%=$(OBJDIR)/%) 

$(BINDIR)/ga_ant : $(OBJECTS) $(EXTENSION) 
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS:%=$(OBJDIR)/%) $(EXTENSION:%=$(OBJDIR)/%)

.PHONY : clean
clean :

