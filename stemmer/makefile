EXTDIR = .
SRCDIR = ../source
OBJDIR = ../bin
BINDIR = ../bin

EXTFLAGS = -I$(SRCDIR)
LDFLAGS = -g
CFLAGS = -g -Wall -D_CRT_SECURE_NO_WARNINGS -DHASHER=1 -DHEADER_HASHER=1 \
	-Wno-endif-labels -Wno-missing-braces -I$(EXTDIR) -I$(SRCDIR)

CC = g++


OBJECTS = \
    encoding_utf8.o \
    encoding_factory.o \
    parser.o \
    universal_parser.o \
    parser_readability.o \
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
    readability_none.o \
    readability_dale_chall.o \
    readability_factory.o \
	search_engine_readability.o \
    plugin_manager.o \
	indexer_param_block.o \
    ant_param_block.o \
	thesaurus_engine.o \
    version.o \
    ranking_function.o \
    ranking_function_impact.o \
    ranking_function_readability.o \
    ranking_function_lmd.o \
    ranking_function_lmjm.o \
    ranking_function_bose_einstein.o \
    ranking_function_bm25.o

EXTENSION = \
    ga.o \
    ga_stemmer.o \
    ga_individual.o \
	ga_function.o \
	vocab.o 

all : index ant ga_ant vocab_tool

%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$*.c -o $(OBJDIR)/$*.o

$(EXTENSION) : 
	$(CC) $(CFLAGS) $(EXTFLAGS) -c $*.c -o $(OBJDIR)/$@

index : $(OBJECTS) index.o
	$(CC) $(LDFLAGS) -o $(BINDIR)/$@ $(OBJDIR)/index.o $(OBJECTS:%=$(OBJDIR)/%)

ant : $(OBJECTS) ant.o 
	$(CC) $(LDFLAGS) -o $(BINDIR)/$@ $(OBJDIR)/ant.o $(OBJECTS:%=$(OBJDIR)/%) 

ant_dict : $(OBJECTS) ant_dictionary.o
	$(CC) $(LDFLAGS) -o $(BINDIR)/$@ $(OBJDIR)/ant_dictionary.o $(OBJECTS:%=$(OBJDIR)/%) 

ga_ant.o : $(EXTDIR)/ga_ant.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -c $(EXTDIR)/ga_ant.c -o $(OBJDIR)/$@
ga_ant : $(OBJECTS) $(EXTENSION) ga_ant.o
	$(CC) $(LDFLAGS) -o $(BINDIR)/$@ $(OBJECTS:%=$(OBJDIR)/%) $(EXTENSION:%=$(OBJDIR)/%) $(OBJDIR)/ga_ant.o

vocab_tool.o : $(EXTDIR)/ga_ant.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DVOCAB_TOOL -c $(EXTDIR)/ga_ant.c -o $(OBJDIR)/$@
vocab_tool : $(OBJECTS) $(EXTENSION) vocab_tool.o
	$(CC) $(LDFLAGS) -o $(BINDIR)/$@ $(OBJECTS:%=$(OBJDIR)/%) $(EXTENSION:%=$(OBJDIR)/%) $(OBJDIR)/vocab_tool.o

.PHONY : clean
clean :
	-rm $(BINDIR)/index $(BINDIR)/ant $(BINDIR)/ga_ant $(BINDIR)/vocab_tool $(OBJECTS:%=$(OBJDIR)/%) $(EXTENSION:%=$(OBJDIR)/%) $(OBJDIR)/ga_ant.o $(OBJDIR)/ant.o $(OBJDIR)/index.o $(OBJDIR)/vocab_tool.o
