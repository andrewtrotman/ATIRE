SRCDIR = ../source
OBJDIR = ../bin
BINDIR = ../bin
EXTSRCDIR = .
EXTOBJDIR = ./obj

#PROFILE = -fprofile-generate
#PROFILE = -fprofile-use

#OPTIM_OR_DEBUG = -g
OPTIM_OR_DEBUG = -O3 

EXTFLAGS = -I$(SRCDIR)
LDFLAGS = $(OPTIM_OR_DEBUG) $(PROFILE) -ldl -pthread
DEFINES = $(OPTIM_OR_DEBUG) $(PROFILE) -DSPECIAL_COMPRESSION -D_CRT_SECURE_NO_WARNINGS \
	-DONE_PARSER -DHASHER=1 \
	-DHEADER_HASHER=1 -DDO_TOP_SCORES 
CFLAGS = -Wall $(DEFINES) -Wno-endif-labels -Wno-missing-braces -I$(EXTSRCDIR) -I$(SRCDIR)
# -DUSE_FLOATED_TF -DGA_STEMMER_STATS

#CC = /home/user/usr/intel/bin/ia32/icpc
#CC = /home/user/usr/sunstudio12.1/bin/CC
#CC = /home/user/usr/bin/openCC
#CC = llvm-g++
CC = g++
#CC = icpc 


OBJECTS = \
	sockets.o \
	channel_file.o \
	channel_socket.o \
	parser.o \
	parser_readability.o \
	memory_index_hash_node.o \
	memory_index.o \
	stats_memory_index.o \
	hash_table.o \
	postings_piece.o \
	ctypes.o \
	file.o \
	file_memory.o \
	file_internals.o \
	maths.o \
	memory.o \
	search_engine.o \
	search_engine_accumulator.o \
	search_engine_result.o \
	search_engine_result_iterator.o \
	mean_average_precision.o \
	assessment.o \
	assessment_ANT.o \
	assessment_INEX.o \
	assessment_TREC.o \
	assessment_factory.o \
	relevant_document.o \
	stats.o \
	stats_time.o \
	stats_search_engine.o \
	search_engine_forum.o \
	search_engine_forum_INEX.o \
	search_engine_forum_INEX_bep.o \
	search_engine_forum_INEX_focus.o \
	search_engine_forum_INEX_efficiency.o \
	search_engine_forum_TREC.o \
	str.o \
	stop_word.o \
	disk.o \
	directory_iterator.o \
	directory_iterator_compressor.o \
	directory_iterator_multiple.o \
	directory_iterator_internals.o \
	directory_iterator_tar.o \
	directory_iterator_warc.o \
	directory_iterator_pkzip.o \
	directory_iterator_file.o \
	directory_iterator_recursive.o \
	btree_iterator.o \
	stemmer.o \
	stemmer_term_similarity.o \
	stemmer_term_similarity_threshold.o \
	stemmer_term_similarity_weighted.o \
	stem_porter.o \
	stem_lovins.o \
	stemmer_factory.o \
	stem_paice_husk.o \
	stem_otago.o \
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
	compress_text_none.o \
	compress_text_bz2.o \
	compress_text_deflate.o \
	compression_text_factory.o \
	readability_dale_chall.o \
	readability_factory.o \
	search_engine_readability.o \
	plugin_manager.o \
	indexer_param_block_rank.o \
	indexer_param_block.o \
	indexer_param_block_rank.o \
	ant_param_block.o \
	version.o \
	ranking_function.o \
	ranking_function_impact.o \
	ranking_function_similarity.o \
	ranking_function_readability.o \
	ranking_function_lmd.o \
	ranking_function_lmjm.o \
	ranking_function_bose_einstein.o \
	ranking_function_divergence.o \
	ranking_function_bm25.o \
	ranking_function_inner_product.o \
	ranking_function_term_count.o \
	instream_file.o \
	instream_deflate.o \
	instream_bz2.o \
	instream_buffer.o \
	nexi.o \
	NEXI_term_iterator.o \
	NEXI_term_ant.o \
	NEXI_term.o \
	semaphores.o \
	critical_section.o \
	focus.o \
	focus_article.o \
	focus_lowest_tag.o \
	focus_results_list.o \
	unicode_case.o \
	threads.o


EXTENSION = \
    ga.o \
    ga_stemmer.o \
    ga_individual.o \
	ga_function.o \
	vocab.o

OBJECTS_W_DIR = $(OBJECTS:%=$(OBJDIR)/%)
EXTENSION_W_DIR = $(EXTENSION:%=$(EXTOBJDIR)/%)

all : $(BINDIR)/index $(BINDIR)/ant $(BINDIR)/ga_ant $(BINDIR)/vocab_tool $(BINDIR)/qpp

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$*.c -o $(OBJDIR)/$*.o

$(EXTOBJDIR)/%.o : $(EXTSRCDIR)/%.c
	$(CC) $(CFLAGS) $(EXTFLAGS) -c $*.c -o $@

$(BINDIR)/index : $(OBJECTS_W_DIR) $(OBJDIR)/index.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/ant : $(OBJECTS_W_DIR) $(OBJDIR)/ant.o 
	$(CC) $(LDFLAGS) -o $@ $^


$(EXTOBJDIR)/qpp_stemmer.o : $(EXTSRCDIR)/ga_stemmer.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DGA_STEMMER_STATS -c $^ -o $@
$(EXTOBJDIR)/qpp_individual.o : $(EXTSRCDIR)/ga_individual.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DGA_STEMMER_STATS -c $^ -o $@
$(BINDIR)/qpp : $(OBJECTS_W_DIR) $(EXTOBJDIR)/vocab.o $(EXTOBJDIR)/qpp_stemmer.o $(EXTOBJDIR)/qpp_individual.o $(EXTOBJDIR)/qpp.o 
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/ant_dict : $(OBJECTS_W_DIR) $(OBJDIR)/ant_dictionary.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/ga_ant : $(OBJECTS_W_DIR) $(EXTENSION_W_DIR) $(EXTOBJDIR)/ga_ant.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/ga_ant_darwin : $(OBJECTS_W_DIR) $(EXTENSION_W_DIR) $(EXTOBJDIR)/ga_ant.o
	$(CC) $(LDFLAGS) -o $@ $^

$(EXTOBJDIR)/vocab_tool.o : $(EXTSRCDIR)/ga_ant.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DVOCAB_TOOL -c $^ -o $@
$(BINDIR)/vocab_tool : $(OBJECTS_W_DIR) $(EXTENSION_W_DIR) $(EXTOBJDIR)/vocab_tool.o
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY : depend
depend : 
	$(CC) -MM -MF makefile.dep $(CFLAGS) $(EXTFLAGS) $(EXTENSION:%.o=$(EXTSRCDIR)/%.c) $(OBJECTS:%.o=$(SRCDIR)/%.c)

.PHONY : clean
clean :
	-rm $(BINDIR)/index $(BINDIR)/ant $(BINDIR)/ga_ant $(BINDIR)/vocab_tool $(OBJECTS_W_DIR) $(EXTENSION_W_DIR) $(EXTOBJDIR)/ga_ant.o $(OBJDIR)/ant.o $(OBJDIR)/index.o $(EXTOBJDIR)/vocab_tool.o $(BINDIR)/qpp $(EXTOBJDIR)/qpp.o $(EXTOBJDIR)/qpp_individual.o $(EXTOBJDIR)/qpp_stemmer.o
	-rm $(EXTOBJDIR)/ga_ant_no_m.o $(EXTOBJDIR)/ga_ant_no_sep.o $(EXTOBJDIR)/ga_ant_no_m_or_sep.o
	-rm $(BINDIR)/ga_ant_no_m $(BINDIR)/ga_ant_no_sep $(BINDIR)/ga_ant_no_m_or_sep 


#
# For experiments in chapter 3
#
$(EXTOBJDIR)/ga_individual_no_m.o : $(EXTSRCDIR)/ga_individual.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DNO_MEASURE -c $^ -o $@
$(BINDIR)/ga_ant_no_m : $(OBJECTS_W_DIR) $(EXTOBJDIR)/ga.o $(EXTOBJDIR)/ga_stemmer.o $(EXTOBJDIR)/ga_function.o $(EXTOBJDIR)/vocab.o $(EXTOBJDIR)/ga_individual_no_m.o $(EXTOBJDIR)/ga_ant.o
	$(CC) $(LDFLAGS) -o $@ $^

$(EXTOBJDIR)/ga_individual_no_sep.o : $(EXTSRCDIR)/ga_individual.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DNO_SEPARATORS -c $^ -o $@
$(BINDIR)/ga_ant_no_sep : $(OBJECTS_W_DIR) $(EXTOBJDIR)/ga.o $(EXTOBJDIR)/ga_stemmer.o $(EXTOBJDIR)/ga_function.o $(EXTOBJDIR)/vocab.o $(EXTOBJDIR)/ga_individual_no_sep.o $(EXTOBJDIR)/ga_ant.o
	$(CC) $(LDFLAGS) -o $@ $^

$(EXTOBJDIR)/ga_individual_no_m_or_sep.o : $(EXTSRCDIR)/ga_individual.c
	$(CC) $(EXTFLAGS) $(CFLAGS) -DNO_MEASURE -DNO_SEPARATORS -c $^ -o $@
$(BINDIR)/ga_ant_no_m_or_sep : $(OBJECTS_W_DIR) $(EXTOBJDIR)/ga.o $(EXTOBJDIR)/ga_stemmer.o $(EXTOBJDIR)/ga_function.o $(EXTOBJDIR)/vocab.o $(EXTOBJDIR)/ga_individual_no_m_or_sep.o $(EXTOBJDIR)/ga_ant.o
	$(CC) $(LDFLAGS) -o $@ $^
